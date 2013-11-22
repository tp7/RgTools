#ifndef __REPAIR_FUNCTIONS_C_H__
#define __REPAIR_FUNCTIONS_C_H__

#include "common.h"
#include <xutility>
#include <algorithm>  
#include <utility>

typedef Byte (CModeProcessor)(const Byte*, Byte val, int);

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


template<typename T>
static RG_FORCEINLINE Byte clip(T val, T minimum, T maximum) {
    return std::max(std::min(val, maximum), minimum);
}

RG_FORCEINLINE Byte repair_mode1_cpp(const Byte* pSrc, Byte val, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    Byte mi = std::min(std::min (
        std::min(std::min(a1, a2), std::min(a3, a4)),
        std::min(std::min(a5, a6), std::min(a7, a8))
        ), c);
    Byte ma = std::max(std::max(
        std::max(std::max(a1, a2), std::max(a3, a4)),
        std::max(std::max(a5, a6), std::max(a7, a8))
        ), c);

    return clip(val, mi, ma);
}

RG_FORCEINLINE Byte repair_mode2_cpp(const Byte* pSrc, Byte val, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    Byte a[9] = { a1, a2, a3, a4, c, a5, a6, a7, a8 };

    std::sort(std::begin(a), std::end(a));

    return clip(val, a[1], a[7]);
}

RG_FORCEINLINE Byte repair_mode3_cpp(const Byte* pSrc, Byte val, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    Byte a[9] = { a1, a2, a3, a4, c, a5, a6, a7, a8 };

    std::sort(std::begin(a), std::end(a));

    return clip(val, a[2], a[6]);
}

RG_FORCEINLINE Byte repair_mode4_cpp(const Byte* pSrc, Byte val, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    Byte a[9] = { a1, a2, a3, a4, c, a5, a6, a7, a8 };

    std::sort(std::begin(a), std::end(a));

    return clip(val, a[3], a[5]);
}

RG_FORCEINLINE Byte repair_mode5_cpp(const Byte* pSrc, Byte val, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    auto mal1 = std::max(std::max(a1, a8), c);
    auto mil1 = std::min(std::min(a1, a8), c);

    auto mal2 = std::max(std::max(a2, a7), c);
    auto mil2 = std::min(std::min(a2, a7), c);

    auto mal3 = std::max(std::max(a3, a6), c);
    auto mil3 = std::min(std::min(a3, a6), c);

    auto mal4 = std::max(std::max(a4, a5), c);
    auto mil4 = std::min(std::min(a4, a5), c);

    Byte c1 = std::abs(val-clip(val, mil1, mal1));
    Byte c2 = std::abs(val-clip(val, mil2, mal2));
    Byte c3 = std::abs(val-clip(val, mil3, mal3));
    Byte c4 = std::abs(val-clip(val, mil4, mal4));

    auto mindiff = std::min(std::min(std::min(c1, c2), c3), c4);

    if (mindiff == c4) return clip(val, mil4, mal4);
    if (mindiff == c2) return clip(val, mil2, mal2);
    if (mindiff == c3) return clip(val, mil3, mal3);
    return clip(val, mil1, mal1);
}

RG_FORCEINLINE Byte repair_mode6_cpp(const Byte* pSrc, Byte val, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    auto mal1 = std::max(std::max(a1, a8), c);
    auto mil1 = std::min(std::min(a1, a8), c);

    auto mal2 = std::max(std::max(a2, a7), c);
    auto mil2 = std::min(std::min(a2, a7), c);

    auto mal3 = std::max(std::max(a3, a6), c);
    auto mil3 = std::min(std::min(a3, a6), c);

    auto mal4 = std::max(std::max(a4, a5), c);
    auto mil4 = std::min(std::min(a4, a5), c);

    int d1 = mal1 - mil1;
    int d2 = mal2 - mil2;
    int d3 = mal3 - mil3;
    int d4 = mal4 - mil4;

    Byte clipped1 = clip(val, mil1, mal1);
    Byte clipped2 = clip(val, mil2, mal2);
    Byte clipped3 = clip(val, mil3, mal3);
    Byte clipped4 = clip(val, mil4, mal4);

    int c1 = clip((std::abs(val-clipped1)<<1)+d1, 0, 255);
    int c2 = clip((std::abs(val-clipped2)<<1)+d2, 0, 255);
    int c3 = clip((std::abs(val-clipped3)<<1)+d3, 0, 255);
    int c4 = clip((std::abs(val-clipped4)<<1)+d4, 0, 255);

    int mindiff = std::min(std::min(std::min(c1, c2), c3), c4);

    if (mindiff == c4) return clip(val, mil4, mal4);
    if (mindiff == c2) return clip(val, mil2, mal2);
    if (mindiff == c3) return clip(val, mil3, mal3);
    return clip(val, mil1, mal1);
}

RG_FORCEINLINE Byte repair_mode7_cpp(const Byte* pSrc, Byte val, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    auto mal1 = std::max(std::max(a1, a8), c);
    auto mil1 = std::min(std::min(a1, a8), c);

    auto mal2 = std::max(std::max(a2, a7), c);
    auto mil2 = std::min(std::min(a2, a7), c);

    auto mal3 = std::max(std::max(a3, a6), c);
    auto mil3 = std::min(std::min(a3, a6), c);

    auto mal4 = std::max(std::max(a4, a5), c);
    auto mil4 = std::min(std::min(a4, a5), c);

    auto d1 = mal1 - mil1;
    auto d2 = mal2 - mil2;
    auto d3 = mal3 - mil3;
    auto d4 = mal4 - mil4;

    auto clipped1 = clip(val, mil1, mal1);
    auto clipped2 = clip(val, mil2, mal2);
    auto clipped3 = clip(val, mil3, mal3);
    auto clipped4 = clip(val, mil4, mal4);

    int c1 = std::abs(val-clipped1)+d1;
    int c2 = std::abs(val-clipped2)+d2;
    int c3 = std::abs(val-clipped3)+d3;
    int c4 = std::abs(val-clipped4)+d4;

    auto mindiff = std::min(std::min(std::min(c1, c2), c3), c4);

    if (mindiff == c4) return clipped4;
    if (mindiff == c2) return clipped2;
    if (mindiff == c3) return clipped3;
    return clipped1;
}

RG_FORCEINLINE Byte repair_mode8_cpp(const Byte* pSrc, Byte val, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    auto mal1 = std::max(std::max(a1, a8), c);
    auto mil1 = std::min(std::min(a1, a8), c);

    auto mal2 = std::max(std::max(a2, a7), c);
    auto mil2 = std::min(std::min(a2, a7), c);

    auto mal3 = std::max(std::max(a3, a6), c);
    auto mil3 = std::min(std::min(a3, a6), c);

    auto mal4 = std::max(std::max(a4, a5), c);
    auto mil4 = std::min(std::min(a4, a5), c);

    Byte d1 = mal1 - mil1;
    Byte d2 = mal2 - mil2;
    Byte d3 = mal3 - mil3;
    Byte d4 = mal4 - mil4;

    Byte clipped1 = clip(val, mil1, mal1);
    Byte clipped2 = clip(val, mil2, mal2);
    Byte clipped3 = clip(val, mil3, mal3);
    Byte clipped4 = clip(val, mil4, mal4);

    int c1 = clip(std::abs(val-clipped1)+(d1<<1), 0, 255);
    int c2 = clip(std::abs(val-clipped2)+(d2<<1), 0, 255);
    int c3 = clip(std::abs(val-clipped3)+(d3<<1), 0, 255);
    int c4 = clip(std::abs(val-clipped4)+(d4<<1), 0, 255);

    Byte mindiff = std::min(std::min(std::min(c1, c2), c3), c4);

    if (mindiff == c4) return clipped4;
    if (mindiff == c2) return clipped2;
    if (mindiff == c3) return clipped3;
    return clipped1;
}

RG_FORCEINLINE Byte repair_mode9_cpp(const Byte* pSrc, Byte val, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    auto mal1 = std::max(std::max(a1, a8), c);
    auto mil1 = std::min(std::min(a1, a8), c);

    auto mal2 = std::max(std::max(a2, a7), c);
    auto mil2 = std::min(std::min(a2, a7), c);

    auto mal3 = std::max(std::max(a3, a6), c);
    auto mil3 = std::min(std::min(a3, a6), c);

    auto mal4 = std::max(std::max(a4, a5), c);
    auto mil4 = std::min(std::min(a4, a5), c);

    auto d1 = mal1 - mil1;
    auto d2 = mal2 - mil2;
    auto d3 = mal3 - mil3;
    auto d4 = mal4 - mil4;

    auto mindiff = std::min(std::min(std::min(d1, d2), d3), d4);

    if (mindiff == d4) return clip(val, mil4, mal4);
    if (mindiff == d2) return clip(val, mil2, mal2);
    if (mindiff == d3) return clip(val, mil3, mal3);
    return clip(val, mil1, mal1);
}

RG_FORCEINLINE Byte repair_mode10_cpp(const Byte* pSrc, Byte val, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    auto d1 = std::abs(val - a1);
    auto d2 = std::abs(val - a2);
    auto d3 = std::abs(val - a3);
    auto d4 = std::abs(val - a4);
    auto d5 = std::abs(val - a5);
    auto d6 = std::abs(val - a6);
    auto d7 = std::abs(val - a7);
    auto d8 = std::abs(val - a8);
    auto dc = std::abs(val - c);

    auto mindiff = std::min(std::min(std::min(std::min(std::min(std::min(std::min(std::min(d1, d2), d3), d4), d5), d6), d7), d8), dc);

    if (mindiff == d7) return a7;
    if (mindiff == d8) return a8;
    if (mindiff == d6) return a6;
    if (mindiff == d2) return a2;
    if (mindiff == d3) return a3;
    if (mindiff == d1) return a1;
    if (mindiff == d5) return a5;
    if (mindiff == dc) return c;
    return a4;
}

RG_FORCEINLINE Byte repair_mode12_cpp(const Byte* pSrc, Byte val, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    Byte a[8] = { a1, a2, a3, a4, a5, a6, a7, a8 };

    std::sort(std::begin(a), std::end(a));
    Byte mi = std::min(a[1], c);
    Byte ma = std::max(a[6], c);

    return clip(val, mi, ma);
}

RG_FORCEINLINE Byte repair_mode13_cpp(const Byte* pSrc, Byte val, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    Byte a[8] = { a1, a2, a3, a4, a5, a6, a7, a8 };

    std::sort(std::begin(a), std::end(a));
    Byte mi = std::min(a[2], c);
    Byte ma = std::max(a[5], c);

    return clip (val, mi, ma);
}

RG_FORCEINLINE Byte repair_mode14_cpp(const Byte* pSrc, Byte val, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    Byte a [8] = { a1, a2, a3, a4, a5, a6, a7, a8 };

    std::sort(std::begin(a), std::end(a));
    Byte mi = std::min(a[3], c);
    Byte ma = std::max(a[4], c);

    return clip (val, mi, ma);
}

RG_FORCEINLINE Byte repair_mode15_cpp(const Byte* pSrc, Byte val, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    auto mal1 = std::max(a1, a8);
    auto mil1 = std::min(a1, a8);

    auto mal2 = std::max(a2, a7);
    auto mil2 = std::min(a2, a7);

    auto mal3 = std::max(a3, a6);
    auto mil3 = std::min(a3, a6);

    auto mal4 = std::max(a4, a5);
    auto mil4 = std::min(a4, a5);

    auto clipped1 = clip(c, mil1, mal1);
    auto clipped2 = clip(c, mil2, mal2);
    auto clipped3 = clip(c, mil3, mal3);
    auto clipped4 = clip(c, mil4, mal4);

    Byte c1 = std::abs (c - clipped1);
    Byte c2 = std::abs (c - clipped2);
    Byte c3 = std::abs (c - clipped3);
    Byte c4 = std::abs (c - clipped4);

    Byte mindiff = std::min (std::min (c1, c2), std::min (c3, c4));

    Byte mi;
    Byte ma;
    if (mindiff == c4)
    {
        mi = mil4;
        ma = mal4;
    }
    else if (mindiff == c2)
    {
        mi = mil2;
        ma = mal2;
    }
    else if (mindiff == c3)
    {
        mi = mil3;
        ma = mal3;
    }
    else
    {
        mi = mil1;
        ma = mal1;
    }

    mi = std::min(mi, c);
    ma = std::max(ma, c);

    return clip(val, mi, ma);
}

RG_FORCEINLINE Byte repair_mode16_cpp(const Byte* pSrc, Byte val, int srcPitch) {
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
     
    auto c1 = clip((std::abs (c - clip(c, mil1, mal1)) << 1) + d1, 0, 0xFF);
    auto c2 = clip((std::abs (c - clip(c, mil2, mal2)) << 1) + d2, 0, 0xFF);
    auto c3 = clip((std::abs (c - clip(c, mil3, mal3)) << 1) + d3, 0, 0xFF);
    auto c4 = clip((std::abs (c - clip(c, mil4, mal4)) << 1) + d4, 0, 0xFF);

    auto mindiff = std::min (std::min (c1, c2), std::min (c3, c4));

    Byte mi;
    Byte ma;
    if (mindiff == c4)
    {
        mi = mil4;
        ma = mal4;
    }
    else if (mindiff == c2)
    {
        mi = mil2;
        ma = mal2;
    }
    else if (mindiff == c3)
    {
        mi = mil3;
        ma = mal3;
    }
    else
    {
        mi = mil1;
        ma = mal1;
    }

    mi = std::min (mi, c);
    ma = std::max (ma, c);

    return clip(val, mi, ma);
}

RG_FORCEINLINE Byte repair_mode17_cpp(const Byte* pSrc, Byte val, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    auto mal1 = std::max(a1, a8);
    auto mil1 = std::min(a1, a8);

    auto mal2 = std::max(a2, a7);
    auto mil2 = std::min(a2, a7);

    auto mal3 = std::max(a3, a6);
    auto mil3 = std::min(a3, a6);

    auto mal4 = std::max(a4, a5);
    auto mil4 = std::min(a4, a5);

    Byte l = std::max (std::max (mil1, mil2), std::max (mil3, mil4));
    Byte u = std::min (std::min (mal1, mal2), std::min (mal3, mal4));
    
	Byte mi = std::min (std::min (l, u), c);
	Byte ma = std::max (std::max (l, u), c);

	return clip(val, mi, ma);
}

RG_FORCEINLINE Byte repair_mode18_cpp(const Byte* pSrc, Byte val, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    auto d1 = std::max(std::abs(c - a1), std::abs(c - a8));
    auto d2 = std::max(std::abs(c - a2), std::abs(c - a7));
    auto d3 = std::max(std::abs(c - a3), std::abs(c - a6));
    auto d4 = std::max(std::abs(c - a4), std::abs(c - a5));

    auto mindiff = std::min(std::min(std::min(d1, d2), d3), d4);

    Byte mi;
    Byte ma;
    if (mindiff == d4)
    {
        mi = std::min (a4, a5);
        ma = std::max (a4, a5);
    }
    else if (mindiff == d2)
    {
        mi = std::min (a2, a7);
        ma = std::max (a2, a7);
    }
    else if (mindiff == d3)
    {
        mi = std::min (a3, a6);
        ma = std::max (a3, a6);
    }
    else
    {
        mi = std::min (a1, a8);
        ma = std::max (a1, a8);
    }

    mi = std::min (mi, c);
    ma = std::max (ma, c);

    return clip(val, mi, ma);
}

RG_FORCEINLINE Byte repair_mode19_cpp(const Byte* pSrc, Byte val, int srcPitch) {
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

    return clip(val, clip(c-mindiff, 0, 255), clip(c+mindiff, 0, 255));
}

RG_FORCEINLINE Byte repair_mode20_cpp(const Byte* pSrc, Byte val, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    Byte d1 = std::abs(c - a1);
    Byte d2 = std::abs(c - a2);
    Byte d3 = std::abs(c - a3);
    Byte d4 = std::abs(c - a4);
    Byte d5 = std::abs(c - a5);
    Byte d6 = std::abs(c - a6);
    Byte d7 = std::abs(c - a7);
    Byte d8 = std::abs(c - a8);

    Byte mindiff = std::min(d1, d2);
    Byte maxdiff = std::max(d1, d2);

    maxdiff = clip(maxdiff, mindiff, d3);
    mindiff = std::min(mindiff, d3);

    maxdiff = clip(maxdiff, mindiff, d4);
    mindiff = std::min(mindiff, d4);

    maxdiff = clip(maxdiff, mindiff, d5);
    mindiff = std::min(mindiff, d5);

    maxdiff = clip(maxdiff, mindiff, d6);
    mindiff = std::min(mindiff, d6);

    maxdiff = clip(maxdiff, mindiff, d7);
    mindiff = std::min(mindiff, d7);

    maxdiff = clip(maxdiff, mindiff, d8);

    return clip(val, clip(c-maxdiff, 0, 255), clip(c+maxdiff, 0, 255));
}

RG_FORCEINLINE Byte repair_mode21_cpp(const Byte* pSrc, Byte val, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    auto mal1 = std::max(a1, a8);
    auto mil1 = std::min(a1, a8);

    auto mal2 = std::max(a2, a7);
    auto mil2 = std::min(a2, a7);

    auto mal3 = std::max(a3, a6);
    auto mil3 = std::min(a3, a6);

    auto mal4 = std::max(a4, a5);
    auto mil4 = std::min(a4, a5);

    auto d1 = clip(mal1 - c, 0, 255);
    auto d2 = clip(mal2 - c, 0, 255);
    auto d3 = clip(mal3 - c, 0, 255);
    auto d4 = clip(mal4 - c, 0, 255);

    auto rd1 = clip(c-mil1, 0, 255);
    auto rd2 = clip(c-mil2, 0, 255);
    auto rd3 = clip(c-mil3, 0, 255);
    auto rd4 = clip(c-mil4, 0, 255);

    auto u1  = std::max(d1, rd1);
    auto u2  = std::max(d2, rd2);
    auto u3  = std::max(d3, rd3);
    auto u4  = std::max(d4, rd4);

    auto u = std::min(std::min(std::min(u1, u2), u3), u4);

    return clip(val, clip(c-u, 0, 255), clip(c+u, 0, 255));
}

RG_FORCEINLINE Byte repair_mode22_cpp(const Byte* pSrc, Byte val, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    auto d1 = std::abs(val - a1);
    auto d2 = std::abs(val - a2);
    auto d3 = std::abs(val - a3);
    auto d4 = std::abs(val - a4);
    auto d5 = std::abs(val - a5);
    auto d6 = std::abs(val - a6);
    auto d7 = std::abs(val - a7);
    auto d8 = std::abs(val - a8);

    auto mindiff = std::min(std::min(std::min(std::min(std::min(std::min(std::min(d1, d2), d3), d4), d5), d6), d7), d8);

    return clip(c, clip(val-mindiff, 0, 255), clip(val+mindiff, 0, 255));
}

RG_FORCEINLINE Byte repair_mode23_cpp(const Byte* pSrc, Byte val, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    Byte d1 = std::abs(val - a1);
    Byte d2 = std::abs(val - a2);
    Byte d3 = std::abs(val - a3);
    Byte d4 = std::abs(val - a4);
    Byte d5 = std::abs(val - a5);
    Byte d6 = std::abs(val - a6);
    Byte d7 = std::abs(val - a7);
    Byte d8 = std::abs(val - a8);

    Byte mindiff = std::min(d1, d2);
    Byte maxdiff = std::max(d1, d2);

    maxdiff = clip(maxdiff, mindiff, d3);
    mindiff = std::min(mindiff, d3);

    maxdiff = clip(maxdiff, mindiff, d4);
    mindiff = std::min(mindiff, d4);

    maxdiff = clip(maxdiff, mindiff, d5);
    mindiff = std::min(mindiff, d5);

    maxdiff = clip(maxdiff, mindiff, d6);
    mindiff = std::min(mindiff, d6);

    maxdiff = clip(maxdiff, mindiff, d7);
    mindiff = std::min(mindiff, d7);

    maxdiff = clip(maxdiff, mindiff, d8);

    return clip(c, clip(val-maxdiff, 0, 255), clip(val+maxdiff, 0, 255));
}

RG_FORCEINLINE Byte repair_mode24_cpp(const Byte* pSrc, Byte val, int srcPitch) {
    LOAD_SQUARE_CPP(pSrc, srcPitch);

    auto mal1 = std::max(a1, a8);
    auto mil1 = std::min(a1, a8);

    auto mal2 = std::max(a2, a7);
    auto mil2 = std::min(a2, a7);

    auto mal3 = std::max(a3, a6);
    auto mil3 = std::min(a3, a6);

    auto mal4 = std::max(a4, a5);
    auto mil4 = std::min(a4, a5);

    auto d1 = clip(mal1 - val, 0, 255);
    auto d2 = clip(mal2 - val, 0, 255);
    auto d3 = clip(mal3 - val, 0, 255);
    auto d4 = clip(mal4 - val, 0, 255);

    auto rd1 = clip(val-mil1, 0, 255);
    auto rd2 = clip(val-mil2, 0, 255);
    auto rd3 = clip(val-mil3, 0, 255);
    auto rd4 = clip(val-mil4, 0, 255);

    auto u1  = std::max(d1, rd1);
    auto u2  = std::max(d2, rd2);
    auto u3  = std::max(d3, rd3);
    auto u4  = std::max(d4, rd4);
    
    auto u = std::min(std::min(std::min(u1, u2), u3), u4);

    return clip(c, clip(val-u, 0, 255), clip(val+u, 0, 255));
}

#undef LOAD_SQUARE_CPP

#endif