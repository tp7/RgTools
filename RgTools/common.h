#ifndef __COMMON_H__
#define __COMMON_H__

typedef unsigned char Byte;

#define RG_FORCEINLINE __forceinline

#define USE_MOVPS

enum InstructionSet {
    SSE2,
    SSE3
};


#endif