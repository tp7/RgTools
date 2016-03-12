#ifndef __COMMON_H__
#define __COMMON_H__

#include <algorithm>
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

template<typename T>
static RG_FORCEINLINE Byte clip(T val, T minimum, T maximum) {
    return std::max(std::min(val, maximum), minimum);
}

static RG_FORCEINLINE bool is_16byte_aligned(const void *ptr) {
    return (((unsigned long)ptr) & 15) == 0;
}

static RG_FORCEINLINE __m128i simd_clip(const __m128i &val, const __m128i &minimum, const __m128i &maximum) {
    return _mm_max_epu8(_mm_min_epu8(val, maximum), minimum);
}

static RG_FORCEINLINE void sort_pair(__m128i &a1, __m128i &a2)
{
    const __m128i tmp = _mm_min_epu8 (a1, a2);
    a2 = _mm_max_epu8 (a1, a2);
    a1 = tmp;
}

template<InstructionSet optLevel>
static RG_FORCEINLINE __m128i simd_loadu_si128(const Byte* ptr) {
    if (optLevel == SSE2) {
#ifdef USE_MOVPS
        return _mm_castps_si128(_mm_loadu_ps(reinterpret_cast<const float*>(ptr)));
#else
        return _mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr));
#endif
    }
    return _mm_lddqu_si128(reinterpret_cast<const __m128i*>(ptr));
}

//mask ? a : b
static RG_FORCEINLINE __m128i blend(__m128i const &mask, __m128i const &desired, __m128i const &otherwise) {
    //return  _mm_blendv_epi8 (otherwise, desired, mask);
    auto andop = _mm_and_si128(mask , desired);
    auto andnop = _mm_andnot_si128(mask, otherwise);
    return _mm_or_si128(andop, andnop);
}

static RG_FORCEINLINE __m128i abs_diff(__m128i a, __m128i b) {
    auto positive = _mm_subs_epu8(a, b);
    auto negative = _mm_subs_epu8(b, a);
    return _mm_or_si128(positive, negative);
}

static RG_FORCEINLINE __m128i select_on_equal(const __m128i &cmp1, const __m128i &cmp2, const __m128i &current, const __m128i &desired) {
    auto eq = _mm_cmpeq_epi8(cmp1, cmp2);
    return blend(eq, desired, current);
}


#define LOAD_SQUARE_SSE(optLevel, ptr, pitch) \
    __m128i a1 = simd_loadu_si128<optLevel>((ptr) - (pitch) - 1); \
    __m128i a2 = simd_loadu_si128<optLevel>((ptr) - (pitch)); \
    __m128i a3 = simd_loadu_si128<optLevel>((ptr) - (pitch) + 1); \
    __m128i a4 = simd_loadu_si128<optLevel>((ptr) - 1); \
    __m128i c  = simd_loadu_si128<optLevel>((ptr) ); \
    __m128i a5 = simd_loadu_si128<optLevel>((ptr) + 1); \
    __m128i a6 = simd_loadu_si128<optLevel>((ptr) + (pitch) - 1); \
    __m128i a7 = simd_loadu_si128<optLevel>((ptr) + (pitch)); \
    __m128i a8 = simd_loadu_si128<optLevel>((ptr) + (pitch) + 1); 


#define LOAD_SQUARE_CPP(ptr, pitch) \
    Byte a1 = *((ptr) - (pitch) - 1); \
    Byte a2 = *((ptr) - (pitch)); \
    Byte a3 = *((ptr) - (pitch) + 1); \
    Byte a4 = *((ptr) - 1); \
    Byte c  = *((ptr) ); \
    Byte a5 = *((ptr) + 1); \
    Byte a6 = *((ptr) + (pitch) - 1); \
    Byte a7 = *((ptr) + (pitch)); \
    Byte a8 = *((ptr) + (pitch) + 1);

#endif