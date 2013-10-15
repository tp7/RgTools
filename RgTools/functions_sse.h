#ifndef __FUNCTIONS_SSE_H__
#define __FUNCTIONS_SSE_H__

#include "common.h"
#include <pmmintrin.h>

typedef __m128i (SseModeProcessor)(const Byte*, int);

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



RG_FORCEINLINE void sort_pair(__m128i &a1, __m128i &a2)
{
    const __m128i tmp = _mm_min_epu8 (a1, a2);
    a2 = _mm_max_epu8 (a1, a2);
    a1 = tmp;
}


template<InstructionSet optLevel>
RG_FORCEINLINE __m128i simd_loadu_si128(const Byte* ptr) {
    if (optLevel == SSE2) {
#ifdef USE_MOVPS
        return _mm_castps_si128(_mm_loadu_ps(reinterpret_cast<const float*>(ptr)));
#else
        return _mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr));
#endif
    }
    return _mm_lddqu_si128(reinterpret_cast<const __m128i*>(ptr));
}



RG_FORCEINLINE __m128i rg_nothing_sse(const Byte*, int) {
    return _mm_set1_epi32(0xBAD);
}


template<InstructionSet optLevel>
RG_FORCEINLINE __m128i rg_mode1_sse(const Byte* pSrc, int srcPitch) {
    LOAD_SQUARE_SSE(optLevel, pSrc, srcPitch);

    __m128i mi = _mm_min_epu8 (
        _mm_min_epu8(_mm_min_epu8(a1, a2), _mm_min_epu8(a3, a4)),
        _mm_min_epu8(_mm_min_epu8(a5, a6), _mm_min_epu8(a7, a8))
        );
    __m128i ma = _mm_max_epu8 (
        _mm_max_epu8(_mm_max_epu8(a1, a2), _mm_max_epu8(a3, a4)),
        _mm_max_epu8(_mm_max_epu8(a5, a6), _mm_max_epu8(a7, a8))
        );

    return _mm_min_epu8(_mm_max_epu8(c, mi), ma);
}

template<InstructionSet optLevel>
RG_FORCEINLINE __m128i rg_mode2_sse(const Byte* pSrc, int srcPitch) {
    LOAD_SQUARE_SSE(optLevel, pSrc, srcPitch);

    sort_pair(a1, a2);
    sort_pair(a3, a4);
    sort_pair(a5, a6);
    sort_pair(a7, a8);

    sort_pair(a1, a3);
    sort_pair(a2, a4);
    sort_pair(a5, a7);
    sort_pair(a6, a8);

    sort_pair(a2, a3);
    sort_pair(a6, a7);

    a5 = _mm_max_epu8 (a1, a5);	// sort_pair (a1, a5);
    sort_pair (a2, a6);
    sort_pair (a3, a7);
    a4 = _mm_min_epu8 (a4, a8);	// sort_pair (a4, a8);

    a3 = _mm_min_epu8 (a3, a5);	// sort_pair (a3, a5);
    a6 = _mm_max_epu8 (a4, a6);	// sort_pair (a4, a6);

    a2 = _mm_min_epu8 (a2, a3);	// sort_pair (a2, a3);
    a7 = _mm_max_epu8 (a6, a7);	// sort_pair (a6, a7);

    return (_mm_min_epu8 (_mm_max_epu8 (c, a2), a7));
}

template<InstructionSet optLevel>
RG_FORCEINLINE __m128i rg_mode3_sse(const Byte* pSrc, int srcPitch) {
    LOAD_SQUARE_SSE(optLevel, pSrc, srcPitch);

    sort_pair(a1, a2);
    sort_pair(a3, a4);
    sort_pair(a5, a6);
    sort_pair(a7, a8);

    sort_pair(a1, a3);
    sort_pair(a2, a4);
    sort_pair(a5, a7);
    sort_pair(a6, a8);

    sort_pair(a2, a3);
    sort_pair(a6, a7);

    a5 = _mm_max_epu8(a1, a5);	// sort_pair (a1, a5);
    sort_pair (a2, a6);
    sort_pair (a3, a7);
    a4 = _mm_min_epu8(a4, a8);	// sort_pair (a4, a8);

    a3 = _mm_min_epu8(a3, a5);	// sort_pair (a3, a5);
    a6 = _mm_max_epu8(a4, a6);	// sort_pair (a4, a6);

    a3 = _mm_max_epu8(a2, a3);	// sort_pair (a2, a3);
    a6 = _mm_min_epu8(a6, a7);	// sort_pair (a6, a7);

    return _mm_min_epu8(_mm_max_epu8(c, a3), a6);
}

template<InstructionSet optLevel>
RG_FORCEINLINE __m128i rg_mode4_sse(const Byte* pSrc, int srcPitch) {
    LOAD_SQUARE_SSE(optLevel, pSrc, srcPitch);

    sort_pair (a1, a2);
    sort_pair (a3, a4);
    sort_pair (a5, a6);
    sort_pair (a7, a8);

    sort_pair (a1, a3);
    sort_pair (a2, a4);
    sort_pair (a5, a7);
    sort_pair (a6, a8);

    sort_pair (a2, a3);
    sort_pair (a6, a7);

    a5 = _mm_max_epu8 (a1, a5);	// sort_pair (a1, a5);
    a6 = _mm_max_epu8 (a2, a6);	// sort_pair (a2, a6);
    a3 = _mm_min_epu8 (a3, a7);	// sort_pair (a3, a7);
    a4 = _mm_min_epu8 (a4, a8);	// sort_pair (a4, a8);

    a5 = _mm_max_epu8 (a3, a5);	// sort_pair (a3, a5);
    a4 = _mm_min_epu8 (a4, a6);	// sort_pair (a4, a6);

    // sort_pair (au82, a3);
    sort_pair (a4, a5);
    // sort_pair (a6, a7);

    return _mm_min_epu8 (_mm_max_epu8 (c, a4), a5);
}


#endif