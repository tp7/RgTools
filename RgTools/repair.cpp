#include "repair_functions_c.h"
#include "repair.h"


template<CModeProcessor processor>
static void process_plane_c(IScriptEnvironment* env, BYTE* pDst, const BYTE* pSrc, const BYTE* pRef, int dstPitch, int srcPitch, int refPitch, int width, int height) {
  env->BitBlt(pDst, dstPitch, pSrc, srcPitch, width, 1);

  pSrc += srcPitch;
  pDst += dstPitch;
  pRef += refPitch;
  for (int y = 1; y < height-1; ++y) {
    pDst[0] = pSrc[0];
    for (int x = 1; x < width-1; x+=1) {
      BYTE result = processor(pRef + x, pSrc[x], srcPitch);
      pDst[x] = result;
    }
    pDst[width-1] = pSrc[width-1];

    pSrc += srcPitch;
    pDst += dstPitch;
    pRef += refPitch;
  }

  env->BitBlt(pDst, dstPitch, pSrc, srcPitch, width, 1);
}


static void doNothing(IScriptEnvironment* env, BYTE* pDst, const BYTE* pSrc, const BYTE* pRef, int dstPitch, int srcPitch, int refPitch, int width, int height) {

}

static void copyPlane(IScriptEnvironment* env, BYTE* pDst, const BYTE* pSrc, const BYTE* pRef, int dstPitch, int srcPitch, int refPitch, int width, int height) {
  env->BitBlt(pDst, dstPitch, pSrc, srcPitch, width, height);
}


RepairPlaneProcessor* c_functions[] = {
  doNothing,
  copyPlane,
  process_plane_c<repair_mode1_cpp>,
  process_plane_c<repair_mode2_cpp>,
  process_plane_c<repair_mode3_cpp>,
  process_plane_c<repair_mode4_cpp>,
  process_plane_c<repair_mode5_cpp>, 
  process_plane_c<repair_mode6_cpp>, 
  process_plane_c<repair_mode7_cpp>, 
  process_plane_c<repair_mode8_cpp>, 
  process_plane_c<repair_mode9_cpp>, 
  process_plane_c<repair_mode10_cpp>, 
  process_plane_c<repair_mode1_cpp>,
  process_plane_c<repair_mode12_cpp>,
  process_plane_c<repair_mode13_cpp>,
  process_plane_c<repair_mode14_cpp>,
  process_plane_c<repair_mode15_cpp>,
  process_plane_c<repair_mode16_cpp>,
  process_plane_c<repair_mode17_cpp>,
  process_plane_c<repair_mode18_cpp>
};

auto sse3_functions = c_functions;
auto sse2_functions = c_functions;

Repair::Repair(PClip child, PClip ref, int mode, int modeU, int modeV, const char* optimization, IScriptEnvironment* env)
  : GenericVideoFilter(child), ref_(ref), mode_(mode), modeU_(modeU), modeV_(modeV), functions(nullptr) {
    if(!vi.IsPlanar()) {
      env->ThrowError("Repair works only with planar colorspaces");
    }

    if (mode_ > 18 || modeU_ > 18 || modeV_ > 18) {
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

    if (vi.width < 17) { //not enough for XMM
      functions = c_functions;
    }
}


PVideoFrame Repair::GetFrame(int n, IScriptEnvironment* env) {
  auto srcFrame = child->GetFrame(n, env);
  auto refFrame = ref_->GetFrame(n, env);
  auto dstFrame = env->NewVideoFrame(vi);

  functions[mode_+1](env, dstFrame->GetWritePtr(PLANAR_Y), srcFrame->GetReadPtr(PLANAR_Y), refFrame->GetReadPtr(PLANAR_Y),
      dstFrame->GetPitch(PLANAR_Y), srcFrame->GetPitch(PLANAR_Y), refFrame->GetPitch(PLANAR_Y),
      srcFrame->GetRowSize(PLANAR_Y), srcFrame->GetHeight(PLANAR_Y));

  if (!vi.IsY8()) {
      functions[mode_+1](env, dstFrame->GetWritePtr(PLANAR_U), srcFrame->GetReadPtr(PLANAR_U), refFrame->GetReadPtr(PLANAR_U),
          dstFrame->GetPitch(PLANAR_U), srcFrame->GetPitch(PLANAR_U), refFrame->GetPitch(PLANAR_U),
          srcFrame->GetRowSize(PLANAR_U), srcFrame->GetHeight(PLANAR_U));

      functions[mode_+1](env, dstFrame->GetWritePtr(PLANAR_V), srcFrame->GetReadPtr(PLANAR_V), refFrame->GetReadPtr(PLANAR_V),
          dstFrame->GetPitch(PLANAR_V), srcFrame->GetPitch(PLANAR_V), refFrame->GetPitch(PLANAR_V),
          srcFrame->GetRowSize(PLANAR_V), srcFrame->GetHeight(PLANAR_V));
  }
  return dstFrame;
}


AVSValue __cdecl Create_Repair(AVSValue args, void*, IScriptEnvironment* env) {
  enum { CLIP, REF, MODE, MODEU, MODEV, OPTIMIZATION };
  return new Repair(args[CLIP].AsClip(), args[REF].AsClip(), args[MODE].AsInt(1), args[MODEU].AsInt(Repair::UNDEFINED_MODE), args[MODEV].AsInt(Repair::UNDEFINED_MODE), args[OPTIMIZATION].AsString(nullptr), env);
}
