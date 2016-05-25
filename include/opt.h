#ifndef _OPT_H_
#define _OPT_H_

#ifndef CONFIG_ETH_PREAMBLES
    #define CONFIG_ETH_PREAMBLES 0
#endif


#ifndef CONFIG_USING_STRING_LIB
    #include "ops.h"

    #define MEMCMP(src, dest, size) eth_memcmp(src, dest, size)
    #define MEMCPY(src, dest, size) eth_memcpy(src, dest, size)
#endif

#endif
