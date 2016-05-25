#ifndef _ARP_H_
#define _ARP_H_

#include "types.h"
#include "errno.h"

err_t arp_input(const uint8_t *pbuff, uint16_t size,struct net_card *net_card);

#endif /* _ARP_H_ */
