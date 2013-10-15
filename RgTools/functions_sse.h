#ifndef __FUNCTIONS_SSE_H__
#define __FUNCTIONS_SSE_H__

#include "common.h"
#include <smmintrin.h>

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

static RG_FORCEINLINE __m128i clip(__m128i &val, __m128i &minimum, __m128i &maximum) {
    return _mm_max_epu8(_mm_min_epu8(val, maximum), minimum);
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

    return clip(c, mi, ma);
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

    return clip(c, a2, a7);
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

    return clip(c, a3, a6);
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

    return clip(c, a4, a5);
}

static RG_FORCEINLINE __m128i select_on_equal(__m128i &cmp1, __m128i &cmp2, __m128i &current, __m128i &desired) {
    auto eq = _mm_cmpeq_epi8(cmp1, cmp2);
    return blend(eq, desired, current);
}


template<InstructionSet optLevel>
RG_FORCEINLINE __m128i rg_mode6_sse(const Byte* pSrc, int srcPitch) {
    LOAD_SQUARE_SSE(optLevel, pSrc, srcPitch);

    auto mal1 = _mm_max_epu8(a1, a8);
    auto mil1 = _mm_min_epu8(a1, a8);

    auto mal2 = _mm_max_epu8(a2, a7);
    auto mil2 = _mm_min_epu8(a2, a7);

    auto mal3 = _mm_max_epu8(a3, a6);
    auto mil3 = _mm_min_epu8(a3, a6);

    auto mal4 = _mm_max_epu8(a4, a5);
    auto mil4 = _mm_min_epu8(a4, a5);

    auto d1 = _mm_subs_epu8(mal1, mil1);
    auto d2 = _mm_subs_epu8(mal2, mil2);
    auto d3 = _mm_subs_epu8(mal3, mil3);
    auto d4 = _mm_subs_epu8(mal4, mil4);

    auto clipped1 = clip(c, mil1, mal1);
    auto clipped2 = clip(c, mil2, mal2);
    auto clipped3 = clip(c, mil3, mal3);
    auto clipped4 = clip(c, mil4, mal4);

    auto absdiff1 = abs_diff(c, clipped1);
    auto absdiff2 = abs_diff(c, clipped2);
    auto absdiff3 = abs_diff(c, clipped3);
    auto absdiff4 = abs_diff(c, clipped4);
    
    auto c1 = _mm_adds_epu8(_mm_adds_epu8(absdiff1, absdiff1), d1);
    auto c2 = _mm_adds_epu8(_mm_adds_epu8(absdiff2, absdiff2), d2);
    auto c3 = _mm_adds_epu8(_mm_adds_epu8(absdiff3, absdiff3), d3);
    auto c4 = _mm_adds_epu8(_mm_adds_epu8(absdiff4, absdiff4), d4);

    auto mindiff = _mm_min_epu8(c1, c2);
    mindiff = _mm_min_epu8(mindiff, c3);
    mindiff = _mm_min_epu8(mindiff, c4);

    auto result = select_on_equal(mindiff, c1, c, clipped1);
    result = select_on_equal(mindiff, c3, result, clipped3);
    result = select_on_equal(mindiff, c2, result, clipped2);
    return select_on_equal(mindiff, c4, result, clipped4);
}


template<InstructionSet optLevel>
RG_FORCEINLINE __m128i rg_mode7_sse(const Byte* pSrc, int srcPitch) {
    LOAD_SQUARE_SSE(optLevel, pSrc, srcPitch);

    auto mal1 = _mm_max_epu8(a1, a8);
    auto mil1 = _mm_min_epu8(a1, a8);

    auto mal2 = _mm_max_epu8(a2, a7);
    auto mil2 = _mm_min_epu8(a2, a7);

    auto mal3 = _mm_max_epu8(a3, a6);
    auto mil3 = _mm_min_epu8(a3, a6);

    auto mal4 = _mm_max_epu8(a4, a5);
    auto mil4 = _mm_min_epu8(a4, a5);

    auto d1 = _mm_subs_epu8(mal1, mil1);
    auto d2 = _mm_subs_epu8(mal2, mil2);
    auto d3 = _mm_subs_epu8(mal3, mil3);
    auto d4 = _mm_subs_epu8(mal4, mil4);

    auto clipped1 = clip(c, mil1, mal1);
    auto clipped2 = clip(c, mil2, mal2);
    auto clipped3 = clip(c, mil3, mal3);
    auto clipped4 = clip(c, mil4, mal4);
    //todo: what happens when this overflows?
    auto c1 = _mm_adds_epu8(abs_diff(c, clipped1), d1);
    auto c2 = _mm_adds_epu8(abs_diff(c, clipped2), d2);
    auto c3 = _mm_adds_epu8(abs_diff(c, clipped3), d3);
    auto c4 = _mm_adds_epu8(abs_diff(c, clipped4), d4);

    auto mindiff = _mm_min_epu8(c1, c2);
    mindiff = _mm_min_epu8(mindiff, c3);
    mindiff = _mm_min_epu8(mindiff, c4);

    auto result = select_on_equal(mindiff, c1, c, clipped1);
    result = select_on_equal(mindiff, c3, result, clipped3);
    result = select_on_equal(mindiff, c2, result, clipped2);
    return select_on_equal(mindiff, c4, result, clipped4);
}


template<InstructionSet optLevel>
RG_FORCEINLINE __m128i rg_mode8_sse(const Byte* pSrc, int srcPitch) {
    LOAD_SQUARE_SSE(optLevel, pSrc, srcPitch);

    auto mal1 = _mm_max_epu8(a1, a8);
    auto mil1 = _mm_min_epu8(a1, a8);

    auto mal2 = _mm_max_epu8(a2, a7);
    auto mil2 = _mm_min_epu8(a2, a7);

    auto mal3 = _mm_max_epu8(a3, a6);
    auto mil3 = _mm_min_epu8(a3, a6);

    auto mal4 = _mm_max_epu8(a4, a5);
    auto mil4 = _mm_min_epu8(a4, a5);

    auto d1 = _mm_subs_epu8(mal1, mil1);
    auto d2 = _mm_subs_epu8(mal2, mil2);
    auto d3 = _mm_subs_epu8(mal3, mil3);
    auto d4 = _mm_subs_epu8(mal4, mil4);

    auto clipped1 = clip(c, mil1, mal1);
    auto clipped2 = clip(c, mil2, mal2);
    auto clipped3 = clip(c, mil3, mal3);
    auto clipped4 = clip(c, mil4, mal4);

    auto c1 = _mm_adds_epu8(abs_diff(c, clipped1), _mm_adds_epu8(d1, d1));
    auto c2 = _mm_adds_epu8(abs_diff(c, clipped2), _mm_adds_epu8(d2, d2));
    auto c3 = _mm_adds_epu8(abs_diff(c, clipped3), _mm_adds_epu8(d3, d3));
    auto c4 = _mm_adds_epu8(abs_diff(c, clipped4), _mm_adds_epu8(d4, d4));

    auto mindiff = _mm_min_epu8(c1, c2);
    mindiff = _mm_min_epu8(mindiff, c3);
    mindiff = _mm_min_epu8(mindiff, c4);

    auto result = select_on_equal(mindiff, c1, c, clipped1);
    result = select_on_equal(mindiff, c3, result, clipped3);
    result = select_on_equal(mindiff, c2, result, clipped2);
    return select_on_equal(mindiff, c4, result, clipped4);
}


template<InstructionSet optLevel>
RG_FORCEINLINE __m128i rg_mode9_sse(const Byte* pSrc, int srcPitch) {
    LOAD_SQUARE_SSE(optLevel, pSrc, srcPitch);

    auto mal1 = _mm_max_epu8(a1, a8);
    auto mil1 = _mm_min_epu8(a1, a8);

    auto mal2 = _mm_max_epu8(a2, a7);
    auto mil2 = _mm_min_epu8(a2, a7);

    auto mal3 = _mm_max_epu8(a3, a6);
    auto mil3 = _mm_min_epu8(a3, a6);

    auto mal4 = _mm_max_epu8(a4, a5);
    auto mil4 = _mm_min_epu8(a4, a5);

    auto d1 = _mm_subs_epu8(mal1, mil1);
    auto d2 = _mm_subs_epu8(mal2, mil2);
    auto d3 = _mm_subs_epu8(mal3, mil3);
    auto d4 = _mm_subs_epu8(mal4, mil4);

    auto mindiff = _mm_min_epu8(d1, d2);
    mindiff = _mm_min_epu8(mindiff, d3);
    mindiff = _mm_min_epu8(mindiff, d4);

    auto result = select_on_equal(mindiff, d1, c, clip(c, mil1, mal1));
    result = select_on_equal(mindiff, d3, result, clip(c, mil3, mal3));
    result = select_on_equal(mindiff, d2, result, clip(c, mil2, mal2));
    return select_on_equal(mindiff, d4, result, clip(c, mil4, mal4));
}




#endif