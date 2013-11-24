#include "removegrain.h"
#include "clense.h"
#include "repair.h"
#include "vertical_cleaner.h"



const AVS_Linkage *AVS_linkage = nullptr;

extern "C" __declspec(dllexport) const char* __stdcall AvisynthPluginInit3(IScriptEnvironment* env, const AVS_Linkage* const vectors) {
    AVS_linkage = vectors;

    env->AddFunction("RemoveGrain2", "c[mode]i[modeU]i[modeV]i[opt]s", Create_RemoveGrain, 0);
    env->AddFunction("Repair2", "cc[mode]i[modeU]i[modeV]i[opt]s", Create_Repair, 0);
    env->AddFunction("Clense2", "c[previous]c[next]c[grey]b", Create_Clense, 0);
    env->AddFunction("ForwardClense2", "c[grey]b", Create_ForwardClense, 0);
    env->AddFunction("BackwardClense2", "c[grey]b", Create_BackwardClense, 0);
    env->AddFunction("VerticalCleaner2", "c[mode]i[modeU]i[modeV]i", Create_VerticalCleaner, 0);
    return "Itai, onii-chan!";
}
