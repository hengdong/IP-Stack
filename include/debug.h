#ifndef _DEBUG_H_
#define _DEBUG_H

#include "ops.h"

#define DEBUG_LEVEL_CURRENT 1

#define DEBUG_LEVEL1(...) \
    if(DEBUG_LEVEL_CURRENT < 1) printf(__VA_ARGS__);

#define DEBUG_ETH_LAYER DEBUG_LEVEL1
#define DEBUG_ARP_LAYER DEBUG_LEVEL1


#endif
