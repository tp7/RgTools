#ifndef __CLENSE_H__
#define __CLENSE_H__

#include "common.h"


enum class ClenseMode {
    FORWARD,
    BACKWARD,
    BOTH
};

class Clense : public GenericVideoFilter {
public:
    Clense(PClip child, PClip previous, PClip next, bool grey, ClenseMode mode, IScriptEnvironment* env);

    PVideoFrame __stdcall GetFrame(int n, IScriptEnvironment* env);

private:
    PClip previous_;
    PClip next_;
    bool grey_;
    bool sse2_;
    ClenseMode mode_;
};


AVSValue __cdecl Create_Clense(AVSValue args, void*, IScriptEnvironment* env);
AVSValue __cdecl Create_ForwardClense(AVSValue args, void*, IScriptEnvironment* env);
AVSValue __cdecl Create_BackwardClense(AVSValue args, void*, IScriptEnvironment* env);

#endif