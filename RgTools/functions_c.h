#ifndef __FUNCTIONS_C_H__
#define __FUNCTIONS_C_H__

#include "common.h"
#include <xutility>
#include <algorithm>  
#include <utility>


typedef Byte (CModeProcessor)(const Byte*, int);

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


static RG_FORCEINLINE Byte clip(Byte val, Byte minimum, Byte maximum) {
    return std::max(std::min(val, maximum), minimum);
}


RG_FORCEINLINE Byte rg_nothing_cpp(const Byte*, int) {
    return 0;
}

RG_FORCEINLINE Byte rg_mode1_cpp(const Byte* pSrc, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    const int mi = std::min (
        std::min (std::min (a1, a2), std::min (a3, a4)),
        std::min (std::min (a5, a6), std::min (a7, a8))
        );
    const int ma = std::max (
        std::max (std::max (a1, a2), std::max (a3, a4)),
        std::max (std::max (a5, a6), std::max (a7, a8))
        );

    return clip(c, mi, ma);
}

RG_FORCEINLINE Byte rg_mode2_cpp(const Byte* pSrc, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    int a [8] = { a1, a2, a3, a4, a5, a6, a7, a8 };

    std::sort(&a [0], (&a [7]) + 1);

    return clip(c, a [2-1], a [7-1]);
}

RG_FORCEINLINE Byte rg_mode3_cpp(const Byte* pSrc, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    int	a [8] = { a1, a2, a3, a4, a5, a6, a7, a8 };

    std::sort (&a [0], (&a [7]) + 1);

    return clip(c, a [3-1], a [6-1]);
}

RG_FORCEINLINE Byte rg_mode4_cpp(const Byte* pSrc, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    int	a [8] = { a1, a2, a3, a4, a5, a6, a7, a8 };

    std::sort (&a [0], (&a [7]) + 1);

    return clip(c, a [4-1], a [5-1]);
}

RG_FORCEINLINE Byte rg_mode5_cpp(const Byte* pSrc, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    auto mal1 = std::max(a1, a8);
    auto mil1 = std::min(a1, a8);

    auto mal2 = std::max(a2, a7);
    auto mil2 = std::min(a2, a7);

    auto mal3 = std::max(a3, a6);
    auto mil3 = std::min(a3, a6);

    auto mal4 = std::max(a4, a5);
    auto mil4 = std::min(a4, a5);

    Byte c1 = std::abs(c-clip(c, mil1, mal1));
    Byte c2 = std::abs(c-clip(c, mil2, mal2));
    Byte c3 = std::abs(c-clip(c, mil3, mal3));
    Byte c4 = std::abs(c-clip(c, mil4, mal4));

    auto mindiff = std::min(std::min(std::min(c1, c2), c3), c4);

    if (mindiff == c4) return clip(c, mil4, mal4);
    if (mindiff == c2) return clip(c, mil2, mal2);
    if (mindiff == c3) return clip(c, mil3, mal3);
    return clip(c, mil1, mal1);
}

RG_FORCEINLINE Byte rg_mode6_cpp(const Byte* pSrc, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    auto mal1 = std::max(a1, a8);
    auto mil1 = std::min(a1, a8);

    auto mal2 = std::max(a2, a7);
    auto mil2 = std::min(a2, a7);

    auto mal3 = std::max(a3, a6);
    auto mil3 = std::min(a3, a6);

    auto mal4 = std::max(a4, a5);
    auto mil4 = std::min(a4, a5);

    auto d1 = mal1 - mil1;
    auto d2 = mal2 - mil2;
    auto d3 = mal3 - mil3;
    auto d4 = mal4 - mil4;

    int c1 = (std::abs(c-clip(c, mil1, mal1))<<1)+d1;
    int c2 = (std::abs(c-clip(c, mil2, mal2))<<1)+d2;
    int c3 = (std::abs(c-clip(c, mil3, mal3))<<1)+d3;
    int c4 = (std::abs(c-clip(c, mil4, mal4))<<1)+d4;


    auto mindiff = std::min(std::min(std::min(c1, c2), c3), c4);

    if (mindiff == c4) return clip(c, mil4, mal4);
    if (mindiff == c2) return clip(c, mil2, mal2);
    if (mindiff == c3) return clip(c, mil3, mal3);
    return clip(c, mil1, mal1);
}

RG_FORCEINLINE Byte rg_mode7_cpp(const Byte* pSrc, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    auto mal1 = std::max(a1, a8);
    auto mil1 = std::min(a1, a8);

    auto mal2 = std::max(a2, a7);
    auto mil2 = std::min(a2, a7);

    auto mal3 = std::max(a3, a6);
    auto mil3 = std::min(a3, a6);

    auto mal4 = std::max(a4, a5);
    auto mil4 = std::min(a4, a5);

    auto d1 = mal1 - mil1;
    auto d2 = mal2 - mil2;
    auto d3 = mal3 - mil3;
    auto d4 = mal4 - mil4;

    auto clipped1 = clip(c, mil1, mal1);
    auto clipped2 = clip(c, mil2, mal2);
    auto clipped3 = clip(c, mil3, mal3);
    auto clipped4 = clip(c, mil4, mal4);

    int c1 = std::abs(c-clipped1)+d1;
    int c2 = std::abs(c-clipped2)+d2;
    int c3 = std::abs(c-clipped3)+d3;
    int c4 = std::abs(c-clipped4)+d4;

    auto mindiff = std::min(std::min(std::min(c1, c2), c3), c4);

    if (mindiff == c4) return clipped4;
    if (mindiff == c2) return clipped2;
    if (mindiff == c3) return clipped3;
    return clipped1;
}

RG_FORCEINLINE Byte rg_mode8_cpp(const Byte* pSrc, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    auto mal1 = std::max(a1, a8);
    auto mil1 = std::min(a1, a8);

    auto mal2 = std::max(a2, a7);
    auto mil2 = std::min(a2, a7);

    auto mal3 = std::max(a3, a6);
    auto mil3 = std::min(a3, a6);

    auto mal4 = std::max(a4, a5);
    auto mil4 = std::min(a4, a5);

    auto d1 = mal1 - mil1;
    auto d2 = mal2 - mil2;
    auto d3 = mal3 - mil3;
    auto d4 = mal4 - mil4;

    int c1 = std::abs(c-clip(c, mil1, mal1))+(d1<<1);
    int c2 = std::abs(c-clip(c, mil2, mal2))+(d2<<1);
    int c3 = std::abs(c-clip(c, mil3, mal3))+(d3<<1);
    int c4 = std::abs(c-clip(c, mil4, mal4))+(d4<<1);


    auto mindiff = std::min(std::min(std::min(c1, c2), c3), c4);

    if (mindiff == c4) return clip(c, mil4, mal4);
    if (mindiff == c2) return clip(c, mil2, mal2);
    if (mindiff == c3) return clip(c, mil3, mal3);
    return clip(c, mil1, mal1);
}

RG_FORCEINLINE Byte rg_mode9_cpp(const Byte* pSrc, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    auto mal1 = std::max(a1, a8);
    auto mil1 = std::min(a1, a8);

    auto mal2 = std::max(a2, a7);
    auto mil2 = std::min(a2, a7);

    auto mal3 = std::max(a3, a6);
    auto mil3 = std::min(a3, a6);

    auto mal4 = std::max(a4, a5);
    auto mil4 = std::min(a4, a5);

    auto d1 = mal1 - mil1;
    auto d2 = mal2 - mil2;
    auto d3 = mal3 - mil3;
    auto d4 = mal4 - mil4;

    auto mindiff = std::min(std::min(std::min(d1, d2), d3), d4);

    if (mindiff == d4) return clip(c, mil4, mal4);
    if (mindiff == d2) return clip(c, mil2, mal2);
    if (mindiff == d3) return clip(c, mil3, mal3);
    return clip(c, mil1, mal1);
}

RG_FORCEINLINE Byte rg_mode10_cpp(const Byte* pSrc, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    auto d1 = std::abs(c - a1);
    auto d2 = std::abs(c - a2);
    auto d3 = std::abs(c - a3);
    auto d4 = std::abs(c - a4);
    auto d5 = std::abs(c - a5);
    auto d6 = std::abs(c - a6);
    auto d7 = std::abs(c - a7);
    auto d8 = std::abs(c - a8);

    auto mindiff = std::min(std::min(std::min(std::min(std::min(std::min(std::min(d1, d2), d3), d4), d5), d6), d7), d8);
    
    if (mindiff == d7) return a7;
    if (mindiff == d8) return a8;
    if (mindiff == d6) return a6;
    if (mindiff == d2) return a2;
    if (mindiff == d3) return a3;
    if (mindiff == d1) return a1;
    if (mindiff == d5) return a5;
    return a4;
}


RG_FORCEINLINE Byte rg_mode11_cpp(const Byte* pSrc, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    int	sum = 4 * c + 2 * (a2 + a4 + a5 + a7) + a1 + a3 + a6 + a8;
    int	val = (sum + 8) >> 4;

    return val;
}

//rounding does not match
RG_FORCEINLINE Byte rg_mode12_cpp(const Byte* pSrc, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    int	sum = 4 * c + 2 * (a2 + a4 + a5 + a7) + a1 + a3 + a6 + a8;
    int	val = (sum + 8) >> 4;

    return val;
}


RG_FORCEINLINE Byte rg_mode13_and14_cpp(const Byte* pSrc, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    auto d1 = std::abs(a1 - a8);
    auto d2 = std::abs(a2 - a7);
    auto d3 = std::abs(a3 - a6);

    auto mindiff = std::min(std::min(d1, d2), d3);
    
    if (mindiff == d2) return (a2 + a7 + 1) / 2;
    if (mindiff == d3) return (a3 + a6 + 1) / 2;
    return (a1 + a8 + 1) / 2;
}

//rounding does not match
RG_FORCEINLINE Byte rg_mode15_and16_cpp(const Byte* pSrc, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    auto d1 = std::abs(a1 - a8);
    auto d2 = std::abs(a2 - a7);
    auto d3 = std::abs(a3 - a6);

    auto mindiff = std::min(std::min(d1, d2), d3);

    auto average = (a1 + 2*a2 + a3 + a6 + 2*a7 + a8 + 4) / 8;

    if (mindiff == d2) return clip(average, std::min(a2, a7), std::max(a2, a7));
    if (mindiff == d3) return clip(average, std::min(a3, a6), std::max(a3, a6));
    return clip(average, std::min(a1, a8), std::max(a1, a8));
}


RG_FORCEINLINE Byte rg_mode17_cpp(const Byte* pSrc, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    auto mal1 = std::max(a1, a8);
    auto mil1 = std::min(a1, a8);

    auto mal2 = std::max(a2, a7);
    auto mil2 = std::min(a2, a7);

    auto mal3 = std::max(a3, a6);
    auto mil3 = std::min(a3, a6);

    auto mal4 = std::max(a4, a5);
    auto mil4 = std::min(a4, a5);

    auto lower = std::max(std::max(std::max(mil1, mil2), mil3), mil4);
    auto upper = std::min(std::min(std::min(mal1, mal2), mal3), mal4);

    return clip(c, std::min(lower, upper), std::max(lower, upper));
}


RG_FORCEINLINE Byte rg_mode18_cpp(const Byte* pSrc, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    auto d1 = std::max(std::abs(c - a1), std::abs(c - a8));
    auto d2 = std::max(std::abs(c - a2), std::abs(c - a7));
    auto d3 = std::max(std::abs(c - a3), std::abs(c - a6));
    auto d4 = std::max(std::abs(c - a4), std::abs(c - a5));

    auto mindiff = std::min(std::min(std::min(d1, d2), d3), d4);

    if (mindiff == d4) return clip(c, std::min(a4, a5),std::max(a4, a5));
    if (mindiff == d2) return clip(c, std::min(a2, a7),std::max(a2, a7));
    if (mindiff == d3) return clip(c, std::min(a3, a6),std::max(a3, a6));
    return clip(c, std::min(a1, a8), std::max(a1, a8));
}

RG_FORCEINLINE Byte rg_mode19_cpp(const Byte* pSrc, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    //int sum = a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8;
    //int val = (sum + 4) >> 3;

    int	val = ((((((a1 + a3 + 1) >> 1) + ((a6 + a8 + 1) >> 1) + 1) >> 1) - 1)
    						       +  ((((a2 + a5 + 1) >> 1) + ((a4 + a7 + 1) >> 1) + 1) >> 1)
    						       + 1) >> 1;

    return val;
}


RG_FORCEINLINE Byte rg_mode20_cpp(const Byte* pSrc, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    int sum = a1 + a2 + a3 + a4 + c + a5 + a6 + a7 + a8;
    int val = (sum + 4) / 9;

    return val;
}


RG_FORCEINLINE Byte rg_mode21_cpp(const Byte* pSrc, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    auto l1a = (a1 + a8) / 2;
    auto l2a = (a2 + a7) / 2;
    auto l3a = (a3 + a6) / 2;
    auto l4a = (a4 + a5) / 2;

    auto l1b = (a1 + a8 + 1) / 2;
    auto l2b = (a2 + a7 + 1) / 2;
    auto l3b = (a3 + a6 + 1) / 2;
    auto l4b = (a4 + a5 + 1) / 2;

    auto ma = std::max(std::max(std::max(l1b, l2b), l3b), l4b);
    auto mi = std::min(std::min(std::min(l1a, l2a), l3a), l4a);

    return clip(c, mi, ma);
}


RG_FORCEINLINE Byte rg_mode22_cpp(const Byte* pSrc, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    auto l1 = (a1 + a8 + 1) / 2;
    auto l2 = (a2 + a7 + 1) / 2;
    auto l3 = (a3 + a6 + 1) / 2;
    auto l4 = (a4 + a5 + 1) / 2;

    auto ma = std::max(std::max(std::max(l1, l2), l3), l4);
    auto mi = std::min(std::min(std::min(l1, l2), l3), l4);

    return clip(c, mi, ma);
}

RG_FORCEINLINE Byte rg_mode23_cpp(const Byte* pSrc, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    auto mal1 = std::max(a1, a8);
    auto mil1 = std::min(a1, a8);

    auto mal2 = std::max(a2, a7);
    auto mil2 = std::min(a2, a7);

    auto mal3 = std::max(a3, a6);
    auto mil3 = std::min(a3, a6);

    auto mal4 = std::max(a4, a5);
    auto mil4 = std::min(a4, a5);

    auto u1 = std::min(c - mal1, mal1 - mil1); // note that we can use saturating subtraction here because we don't care if u* is negative
    auto u2 = std::min(c - mal2, mal2 - mil2);
    auto u3 = std::min(c - mal3, mal3 - mil3);
    auto u4 = std::min(c - mal4, mal4 - mil4);
    auto u =  std::max(std::max(std::max(std::max(u1, u2), u3), u4), 0);

    auto d1 = std::min(mil1 - c, mal1 - mil1); // likewise, saturating subtraction here too
    auto d2 = std::min(mil2 - c, mal2 - mil2);
    auto d3 = std::min(mil3 - c, mal3 - mil3);
    auto d4 = std::min(mil4 - c, mal4 - mil4);
    auto d = std::max(std::max(std::max(std::max(d1, d2), d3), d4), 0);

    return c - u + d; // this probably will never overflow.
}


RG_FORCEINLINE Byte rg_mode24_cpp(const Byte* pSrc, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    auto mal1 = std::max(a1, a8);
    auto mil1 = std::min(a1, a8);

    auto mal2 = std::max(a2, a7);
    auto mil2 = std::min(a2, a7);

    auto mal3 = std::max(a3, a6);
    auto mil3 = std::min(a3, a6);

    auto mal4 = std::max(a4, a5);
    auto mil4 = std::min(a4, a5);

    auto u1 = std::min(c - mal1, mal1 - mil1 -(c - mal1)); // as above, satsub
    auto u2 = std::min(c - mal2, mal2 - mil2 -(c - mal2));
    auto u3 = std::min(c - mal3, mal3 - mil3 -(c - mal3));
    auto u4 = std::min(c - mal4, mal4 - mil4 -(c - mal4));
    auto u = std::max(std::max(std::max(std::max(u1, u2), u3), u4), 0);

    auto d1 = std::min(mil1 - c, mal1 - mil1 - (mil1 - c)); // satsub etc.
    auto d2 = std::min(mil2 - c, mal2 - mil2 - (mil2 - c));
    auto d3 = std::min(mil3 - c, mal3 - mil3 - (mil3 - c));
    auto d4 = std::min(mil4 - c, mal4 - mil4 - (mil4 - c));
    auto d = std::max(std::max(std::max(std::max(d1, d2), d3), d4), 0);

    return c - u + d;
}


#endif