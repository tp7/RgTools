#include "clense.h"
#include <xutility>

static void check_if_match(const VideoInfo &vi, const VideoInfo &otherVi, IScriptEnvironment* env) {
    if (otherVi.height != vi.height || otherVi.width != vi.width) {
        env->ThrowError("Frame dimensions do not match");
    }
    if (!otherVi.IsSameColorspace(vi)) {
        env->ThrowError("Colorspaces do not match");
    }
}


Clense::Clense(PClip child, PClip previous, PClip next, bool grey, ClenseMode mode, IScriptEnvironment* env)
    : GenericVideoFilter(child), previous_(previous), next_(next), grey_(grey), mode_(mode) {
        if(!vi.IsPlanar()) {
            env->ThrowError("Clense works only with planar colorspaces");
        }

        if (previous_ != nullptr) {
           check_if_match(vi, previous_->GetVideoInfo(), env);
        }
        if (next_ != nullptr) {
            check_if_match(vi, next_->GetVideoInfo(), env);
        }
        sse2_ = vi.width > 16 && (env->GetCPUFlags() & CPUF_SSE2) ;
}

static void process_plane_c(Byte* pDst, const Byte *pSrc, const Byte* pRef1, const Byte* pRef2, int dstPitch, int srcPitch, int ref1Pitch, int ref2Pitch, int width, int height, IScriptEnvironment *env) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            pDst[x] = std::min(std::max(pSrc[x], std::min(pRef1[x],pRef2[x])), std::max(pRef1[x],pRef2[x]));
        }
        pDst += dstPitch;
        pSrc += srcPitch;
        pRef1 += ref1Pitch;
        pRef2 += ref2Pitch;
    }
}

static RG_FORCEINLINE void process_line_sse2(Byte* pDst, const Byte *pSrc, const Byte* pRef1, const Byte* pRef2, int width) {
        for (int x = 0; x < width; x+=16) {
            auto src = _mm_load_si128(reinterpret_cast<const __m128i*>(pSrc+x));
            auto ref1 = _mm_load_si128(reinterpret_cast<const __m128i*>(pRef1+x));
            auto ref2 = _mm_load_si128(reinterpret_cast<const __m128i*>(pRef2+x));

            auto minref = _mm_min_epu8(ref1, ref2);
            auto maxref = _mm_max_epu8(ref1, ref2);
            auto dst = simd_clip(src, minref, maxref);

            _mm_store_si128(reinterpret_cast<__m128i*>(pDst+x), dst);
        }
}

static void process_plane_sse2(Byte* pDst, const Byte *pSrc, const Byte* pRef1, const Byte* pRef2, int dstPitch, int srcPitch, int ref1Pitch, int ref2Pitch, int width, int height, IScriptEnvironment *env) {
    if (!is_16byte_aligned(pSrc) || !is_16byte_aligned(pRef1) || !is_16byte_aligned(pRef2)) {
        env->ThrowError("Invalid memory alignment. Used unaligned crop?"); //omg I feel so dumb
    }
    auto mod16Width = (width / 16) * 16;

    for (int y = 0; y < height; ++y) {
        process_line_sse2(pDst, pSrc, pRef1, pRef2, mod16Width);

        if (mod16Width != width) {
            process_line_sse2(pDst + mod16Width, pSrc + mod16Width, pRef1 + mod16Width, pRef2 + mod16Width, 16);
        }
        pDst += dstPitch;
        pSrc += srcPitch;
        pRef1 += ref1Pitch;
        pRef2 += ref2Pitch;
    }
}


PVideoFrame Clense::GetFrame(int n, IScriptEnvironment* env) {
    auto srcFrame = child->GetFrame(n, env);

    if (n == 0 || n == vi.num_frames-1 || (mode_ == ClenseMode::BACKWARD && n == 1) || (mode_ == ClenseMode::FORWARD && n == vi.num_frames - 2)) {
        return srcFrame;
    }

    PVideoFrame frame1, frame2;

    if (mode_ == ClenseMode::BACKWARD) {
        frame1 = child->GetFrame(n-1, env);
        frame2 = child->GetFrame(n-2, env);
    } else if (mode_ == ClenseMode::FORWARD) {
        frame1 = child->GetFrame(n+1, env);
        frame2 = child->GetFrame(n+2, env);
    } else {
        frame1 = previous_ == nullptr ? child->GetFrame(n-1, env) : previous_->GetFrame(n-1, env);
        frame2 = next_ == nullptr ? child->GetFrame(n+1, env) : next_->GetFrame(n+1, env);
    }

    auto dstFrame = env->NewVideoFrame(vi);
    
    auto processor = sse2_ ? process_plane_sse2 : process_plane_c;

    processor(dstFrame->GetWritePtr(PLANAR_Y), srcFrame->GetReadPtr(PLANAR_Y), frame1->GetReadPtr(PLANAR_Y), frame2->GetReadPtr(PLANAR_Y),
        dstFrame->GetPitch(PLANAR_Y), srcFrame->GetPitch(PLANAR_Y), frame1->GetPitch(PLANAR_Y), frame2->GetPitch(PLANAR_Y),
        srcFrame->GetRowSize(PLANAR_Y), srcFrame->GetHeight(PLANAR_Y), env);
        
    if (vi.IsY8() || grey_) {
        return dstFrame;
    }

    processor(dstFrame->GetWritePtr(PLANAR_U), srcFrame->GetReadPtr(PLANAR_U), frame1->GetReadPtr(PLANAR_U), frame2->GetReadPtr(PLANAR_U),
        dstFrame->GetPitch(PLANAR_U), srcFrame->GetPitch(PLANAR_U), frame1->GetPitch(PLANAR_U), frame2->GetPitch(PLANAR_U), 
        srcFrame->GetRowSize(PLANAR_U), srcFrame->GetHeight(PLANAR_U), env);

    processor(dstFrame->GetWritePtr(PLANAR_V), srcFrame->GetReadPtr(PLANAR_V), frame1->GetReadPtr(PLANAR_V), frame2->GetReadPtr(PLANAR_V),
        dstFrame->GetPitch(PLANAR_V), srcFrame->GetPitch(PLANAR_V), frame1->GetPitch(PLANAR_V), frame2->GetPitch(PLANAR_V), 
        srcFrame->GetRowSize(PLANAR_V), srcFrame->GetHeight(PLANAR_V), env);
    return dstFrame;
}

AVSValue __cdecl Create_Clense(AVSValue args, void*, IScriptEnvironment* env) {
    enum { CLIP, PREVIOUS, NEXT, GREY };
    return new Clense(args[CLIP].AsClip(), args[PREVIOUS].Defined() ? args[PREVIOUS].AsClip() : nullptr, args[NEXT].Defined() ? args[NEXT].AsClip() : nullptr, args[GREY].AsBool(false), ClenseMode::BOTH, env);
}

//a bit wrong
AVSValue __cdecl Create_ForwardClense(AVSValue args, void*, IScriptEnvironment* env) {
    enum { CLIP, GREY };
    return new Clense(args[CLIP].AsClip(), nullptr, nullptr, args[GREY].AsBool(false), ClenseMode::FORWARD, env);
}

//a bit wrong
AVSValue __cdecl Create_BackwardClense(AVSValue args, void*, IScriptEnvironment* env) {
    enum { CLIP, GREY };
    return new Clense(args[CLIP].AsClip(), nullptr, nullptr, args[GREY].AsBool(false), ClenseMode::BACKWARD, env);
}