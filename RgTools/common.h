#ifndef __COMMON_H__
#define __COMMON_H__

#define NOMINMAX
#include <Windows.h>
#pragma warning(disable: 4512 4244 4100)
#include "avisynth.h"
#pragma warning(default: 4512 4244 4100)
#include <smmintrin.h>

typedef unsigned char Byte;

#define RG_FORCEINLINE __forceinline

#define USE_MOVPS

enum InstructionSet {
    SSE2,
    SSE3
};

static RG_FORCEINLINE bool is_16byte_aligned(const void *ptr) {
    return (((unsigned long)ptr) & 15) == 0;
}

static RG_FORCEINLINE __m128i simd_clip(const __m128i &val, const __m128i &minimum, const __m128i &maximum) {
    return _mm_max_epu8(_mm_min_epu8(val, maximum), minimum);
}

#endif