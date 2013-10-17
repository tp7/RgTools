#include "functions_c.h"
#include "functions_sse.h"

#include <Windows.h>
#pragma warning(disable: 4512 4244 4100)
#include "avisynth.h"
#pragma warning(default: 4512 4244 4100)




template<SseModeProcessor processor>
void process_plane_sse(IScriptEnvironment* env, const BYTE* pSrc, BYTE* pDst, int width, int height, int srcPitch, int dstPitch) {
    env->BitBlt(pDst, dstPitch, pSrc, srcPitch, width, 1);

    pSrc += srcPitch;
    pDst += dstPitch;
    for (int y = 1; y < height-1; ++y) {
        pDst[0] = pSrc[0];
        for (int x = 1; x < width-1; x+=16) {
            __m128i result = processor(pSrc+x, srcPitch);
            _mm_storeu_si128(reinterpret_cast<__m128i*>(pDst+x), result);
        }
        pDst[width-1] = pSrc[width-1];

        pSrc += srcPitch;
        pDst += dstPitch;
    }

    env->BitBlt(pDst, dstPitch, pSrc, srcPitch, width, 1);
}


template<SseModeProcessor processor>
void process_halfplane_sse(IScriptEnvironment* env, const BYTE* pSrc, BYTE* pDst, int width, int height, int srcPitch, int dstPitch) {
    pSrc += srcPitch;
    pDst += dstPitch;
    for (int y = 1; y < height/2; ++y) {
        pDst[0] = (pSrc[srcPitch] + pSrc[-srcPitch] + 1) / 2;
        for (int x = 1; x < width-1; x+=16) {
            __m128i result = processor(pSrc+x, srcPitch);
            _mm_storeu_si128(reinterpret_cast<__m128i*>(pDst+x), result);
        }
        pDst[width-1] = (pSrc[width-1 + srcPitch] + pSrc[width-1 - srcPitch] + 1) / 2;
        pSrc += srcPitch;
        pDst += dstPitch;

        env->BitBlt(pDst, dstPitch, pSrc, srcPitch, width, 1); //other field

        pSrc += srcPitch;
        pDst += dstPitch;
    }
}

template<SseModeProcessor processor>
void process_even_rows_sse(IScriptEnvironment* env, const BYTE* pSrc, BYTE* pDst, int width, int height, int srcPitch, int dstPitch) {
    env->BitBlt(pDst, dstPitch, pSrc, srcPitch, width, 2); //copy first two lines

    process_halfplane_sse<processor>(env, pSrc+srcPitch, pDst+dstPitch, width, height, srcPitch, dstPitch);
}

template<SseModeProcessor processor>
void process_odd_rows_sse(IScriptEnvironment* env, const BYTE* pSrc, BYTE* pDst, int width, int height, int srcPitch, int dstPitch) {
    env->BitBlt(pDst, dstPitch, pSrc, srcPitch, width, 1); //top border

    process_halfplane_sse<processor>(env, pSrc, pDst, width, height, srcPitch, dstPitch);

    env->BitBlt(pDst+dstPitch*(height-1), dstPitch, pSrc+srcPitch*(height-1), srcPitch, width, 1); //bottom border
}

template<CModeProcessor processor>
void process_plane_c(IScriptEnvironment* env, const BYTE* pSrc, BYTE* pDst, int width, int height, int srcPitch, int dstPitch) {
    env->BitBlt(pDst, dstPitch, pSrc, srcPitch, width, 1);

    pSrc += srcPitch;
    pDst += dstPitch;
    for (int y = 1; y < height-1; ++y) {
        pDst[0] = pSrc[0];
        for (int x = 1; x < width-1; x+=1) {
            BYTE result = processor(pSrc + x, srcPitch);
            pDst[x] = result;
        }
        pDst[width-1] = pSrc[width-1];

        pSrc += srcPitch;
        pDst += dstPitch;
    }

    env->BitBlt(pDst, dstPitch, pSrc, srcPitch, width, 1);
}

template<CModeProcessor processor>
void process_halfplane_c(IScriptEnvironment* env, const BYTE* pSrc, BYTE* pDst, int width, int height, int srcPitch, int dstPitch) {
    pSrc += srcPitch;
    pDst += dstPitch;
    for (int y = 1; y < height/2; ++y) {
        pDst[0] = (pSrc[srcPitch] + pSrc[-srcPitch] + 1) / 2;
        for (int x = 1; x < width-1; x+=1) {
            BYTE result = processor(pSrc + x, srcPitch);
            pDst[x] = result;
        }
        pDst[width-1] = (pSrc[width-1 + srcPitch] + pSrc[width-1 - srcPitch] + 1) / 2;
        pSrc += srcPitch;
        pDst += dstPitch;

        env->BitBlt(pDst, dstPitch, pSrc, srcPitch, width, 1); //other field

        pSrc += srcPitch;
        pDst += dstPitch;
    }
}

template<CModeProcessor processor>
void process_even_rows_c(IScriptEnvironment* env, const BYTE* pSrc, BYTE* pDst, int width, int height, int srcPitch, int dstPitch) {
    env->BitBlt(pDst, dstPitch, pSrc, srcPitch, width, 2); //copy first two lines

    process_halfplane_c<processor>(env, pSrc+srcPitch, pDst+dstPitch, width, height, srcPitch, dstPitch);
}

template<CModeProcessor processor>
void process_odd_rows_c(IScriptEnvironment* env, const BYTE* pSrc, BYTE* pDst, int width, int height, int srcPitch, int dstPitch) {
    env->BitBlt(pDst, dstPitch, pSrc, srcPitch, width, 1); //top border

    process_halfplane_c<processor>(env, pSrc, pDst, width, height, srcPitch, dstPitch);

    env->BitBlt(pDst+dstPitch*(height-1), dstPitch, pSrc+srcPitch*(height-1), srcPitch, width, 1); //bottom border
}

void doNothing(IScriptEnvironment* env, const BYTE* pSrc, BYTE* pDst, int width, int height, int srcPitch, int dstPitch) {

}

void copyPlane(IScriptEnvironment* env, const BYTE* pSrc, BYTE* pDst, int width, int height, int srcPitch, int dstPitch) {
    env->BitBlt(pDst, dstPitch, pSrc, srcPitch, width, height);
}

typedef void (PlaneProcessor)(IScriptEnvironment* env, const BYTE* pSrc, BYTE* pDst, int width, int height, int srcPitch, int dstPitch);


PlaneProcessor* sse2_functions[] = {
    doNothing,
    copyPlane,
    process_plane_sse<rg_mode1_sse<SSE2>>,
    process_plane_sse<rg_mode2_sse<SSE2>>,
    process_plane_sse<rg_mode3_sse<SSE2>>,
    process_plane_sse<rg_mode4_sse<SSE2>>,
    process_plane_sse<rg_mode5_sse<SSE2>>,
    process_plane_sse<rg_mode6_sse<SSE2>>,
    process_plane_sse<rg_mode7_sse<SSE2>>,
    process_plane_sse<rg_mode8_sse<SSE2>>,
    process_plane_sse<rg_mode9_sse<SSE2>>,
    process_plane_sse<rg_mode10_sse<SSE2>>,
    process_plane_sse<rg_mode11_sse<SSE2>>,
    process_plane_sse<rg_mode12_sse<SSE2>>,
    process_even_rows_sse<rg_mode13_and14_sse<SSE2>>,
    process_odd_rows_sse<rg_mode13_and14_sse<SSE2>>,
    process_even_rows_c<rg_mode15_and16_cpp>,
    process_odd_rows_c<rg_mode15_and16_cpp>,
    process_plane_sse<rg_mode17_sse<SSE2>>,
    process_plane_sse<rg_mode18_sse<SSE2>>,
    process_plane_sse<rg_mode19_sse<SSE2>>,
    process_plane_c<rg_mode20_cpp>,
    process_plane_sse<rg_mode21_sse<SSE2>>,
    process_plane_sse<rg_mode22_sse<SSE2>>,
    process_plane_sse<rg_mode23_sse<SSE2>>,
    process_plane_sse<rg_mode24_sse<SSE2>>,
};

PlaneProcessor* sse3_functions[] = {
    doNothing,
    copyPlane,
    process_plane_sse<rg_mode1_sse<SSE3>>,
    process_plane_sse<rg_mode2_sse<SSE3>>,
    process_plane_sse<rg_mode3_sse<SSE3>>,
    process_plane_sse<rg_mode4_sse<SSE3>>,
    process_plane_sse<rg_mode5_sse<SSE3>>,
    process_plane_sse<rg_mode6_sse<SSE3>>,
    process_plane_sse<rg_mode7_sse<SSE3>>,
    process_plane_sse<rg_mode8_sse<SSE3>>,
    process_plane_sse<rg_mode9_sse<SSE3>>,
    process_plane_sse<rg_mode10_sse<SSE3>>,
    process_plane_sse<rg_mode11_sse<SSE3>>,
    process_plane_sse<rg_mode12_sse<SSE3>>,
    process_even_rows_sse<rg_mode13_and14_sse<SSE3>>,
    process_odd_rows_sse<rg_mode13_and14_sse<SSE3>>,
    process_even_rows_c<rg_mode15_and16_cpp>,
    process_odd_rows_c<rg_mode15_and16_cpp>,
    process_plane_sse<rg_mode17_sse<SSE3>>,
    process_plane_sse<rg_mode18_sse<SSE3>>,
    process_plane_sse<rg_mode19_sse<SSE3>>,
    process_plane_c<rg_mode20_cpp>,
    process_plane_sse<rg_mode21_sse<SSE3>>,
    process_plane_sse<rg_mode22_sse<SSE3>>,
    process_plane_sse<rg_mode23_sse<SSE3>>,
    process_plane_sse<rg_mode24_sse<SSE3>>,
};

PlaneProcessor* c_functions[] = {
    doNothing,
    copyPlane,
    process_plane_c<rg_mode1_cpp>,
    process_plane_c<rg_mode2_cpp>,
    process_plane_c<rg_mode3_cpp>,
    process_plane_c<rg_mode4_cpp>,
    process_plane_c<rg_mode5_cpp>,
    process_plane_c<rg_mode6_cpp>,
    process_plane_c<rg_mode7_cpp>,
    process_plane_c<rg_mode8_cpp>,
    process_plane_c<rg_mode9_cpp>,
    process_plane_c<rg_mode10_cpp>,
    process_plane_c<rg_mode11_cpp>,
    process_plane_c<rg_mode12_cpp>,
    process_even_rows_c<rg_mode13_and14_cpp>,
    process_odd_rows_c<rg_mode13_and14_cpp>,
    process_even_rows_c<rg_mode15_and16_cpp>,
    process_odd_rows_c<rg_mode15_and16_cpp>,
    process_plane_c<rg_mode17_cpp>,
    process_plane_c<rg_mode18_cpp>,
    process_plane_c<rg_mode19_cpp>,
    process_plane_c<rg_mode20_cpp>,
    process_plane_c<rg_mode21_cpp>,
    process_plane_c<rg_mode22_cpp>,
    process_plane_c<rg_mode23_cpp>,
    process_plane_c<rg_mode24_cpp>
};

class RemoveGrain2 : public GenericVideoFilter {
public:
    RemoveGrain2(PClip child, int mode, int modeU, int modeV, const char* optimization, IScriptEnvironment* env);

    PVideoFrame __stdcall GetFrame(int n, IScriptEnvironment* env);

    const static int UNDEFINED_MODE = -2;

private:
    int mode_;
    int modeU_;
    int modeV_;

    PlaneProcessor **functions;
};


RemoveGrain2::RemoveGrain2(PClip child, int mode, int modeU, int modeV, const char* optimization, IScriptEnvironment* env)
    : GenericVideoFilter(child), mode_(mode), modeU_(modeU), modeV_(modeV), functions(nullptr) {
        if(!vi.IsPlanar()) {
            env->ThrowError("RemoveGrain2 works only with planar colorspaces");
        }

        if (mode_ > 24 || modeU_ > 24 || modeV_ > 24) {
            env->ThrowError("Sorry, this mode does not exist");
        }

        //now change undefined mode value and EVERYTHING WILL BREAK
        if (modeU_ <= UNDEFINED_MODE) { 
            modeU_ = mode_;
        }
        if (modeV_ <= UNDEFINED_MODE) {
            modeV_ = modeU_;
        }

        functions = (env->GetCPUFlags() & CPUF_SSE3) ? sse3_functions 
            : (env->GetCPUFlags() & CPUF_SSE2) ? sse2_functions
            : c_functions;

        if (optimization != nullptr) {
            if ((lstrcmpi(optimization, "sse2") == 0) && env->GetCPUFlags() & CPUF_SSE2) {
                functions = sse2_functions;
            } else if (lstrcmpi(optimization, "cpp") == 0) {
                functions = c_functions;
            }
        }
}


PVideoFrame RemoveGrain2::GetFrame(int n, IScriptEnvironment* env) {
    auto srcFrame = child->GetFrame(n, env);
    auto dstFrame = env->NewVideoFrame(vi, 16);
    
    functions[mode_+1](env, srcFrame->GetReadPtr(PLANAR_Y), dstFrame->GetWritePtr(PLANAR_Y), srcFrame->GetRowSize(PLANAR_Y), 
        srcFrame->GetHeight(PLANAR_Y), srcFrame->GetPitch(PLANAR_Y), srcFrame->GetPitch(PLANAR_Y));

    if (!vi.IsY8()) {
        functions[modeU_+1](env, srcFrame->GetReadPtr(PLANAR_U), dstFrame->GetWritePtr(PLANAR_U), srcFrame->GetRowSize(PLANAR_U), 
            srcFrame->GetHeight(PLANAR_U), srcFrame->GetPitch(PLANAR_U), srcFrame->GetPitch(PLANAR_U));

        functions[modeV_+1](env, srcFrame->GetReadPtr(PLANAR_V), dstFrame->GetWritePtr(PLANAR_V), srcFrame->GetRowSize(PLANAR_V), 
            srcFrame->GetHeight(PLANAR_V), srcFrame->GetPitch(PLANAR_V), srcFrame->GetPitch(PLANAR_V));
    }
    return dstFrame;
}


AVSValue __cdecl Create_RemoveGrain2(AVSValue args, void*, IScriptEnvironment* env) {
    enum { CLIP, MODE, MODEU, MODEV, OPTIMIZATION };
    return new RemoveGrain2(args[CLIP].AsClip(), args[MODE].AsInt(1), args[MODEU].AsInt(RemoveGrain2::UNDEFINED_MODE), args[MODEV].AsInt(RemoveGrain2::UNDEFINED_MODE), args[OPTIMIZATION].AsString(nullptr), env);
}

const AVS_Linkage *AVS_linkage = nullptr;

extern "C" __declspec(dllexport) const char* __stdcall AvisynthPluginInit3(IScriptEnvironment* env, const AVS_Linkage* const vectors) {
    AVS_linkage = vectors;

    env->AddFunction("RemoveGrain2", "c[order]i[mode]i[modeUV]i[opt]s", Create_RemoveGrain2, 0);
    return "Itai, onii-chan!";
}
