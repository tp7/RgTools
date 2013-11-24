#ifndef __VERTICAL_CLEANER_H__
#define __VERTICAL_CLEANER_H__

#include "common.h"

typedef void (VCleanerProcessor)(Byte* pDst, const Byte *pSrc, int dstPitch, int srcPitch, int width, int height, IScriptEnvironment *env);

class VerticalCleaner : public GenericVideoFilter {
public:
    VerticalCleaner(PClip child, int mode, int modeU, int modeV, IScriptEnvironment* env);

    PVideoFrame __stdcall GetFrame(int n, IScriptEnvironment* env);

    const static int UNDEFINED_MODE = -2;

private:
    int mode_;
    int modeU_;
    int modeV_;
};


AVSValue __cdecl Create_VerticalCleaner(AVSValue args, void*, IScriptEnvironment* env);

#endif