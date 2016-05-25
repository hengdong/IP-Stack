#ifndef _DEF_H_
#define _DEF_H_

#include "opt.h"

#ifndef ETH_MTU
    #define ETH_MTU 1500
#endif

#if CONFIG_ETH_PREAMBLES && !define(PREAMBLE_BYTES)
    #define PREAMBLE_BYTES 8
#endif

#ifndef MAC_ADRR_BYTES
    #define MAC_ADRR_BYTES 6
#endif

#define STRUCT_BEGIN_FIRST
#define STRUCT_END_FIRST

#define ntohs(x) (((x & 0xff00) >> 8) | ((x & 0x00ff) << 8))

#ifndef POSIX_FREE
    #define POSIX_FREE(x)
#endif

#endif
