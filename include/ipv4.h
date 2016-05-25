#ifndef _IPV4_H_

#include "list.h"

struct ipv4_node{
	struct list list;

	uint32_t addr;
	uint32_t mask;
	uint32_t gate;
};

err_t ipv4_find_node(uint32_t ip_addr, struct net_card *net_card);

#endif
