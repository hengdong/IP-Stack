#ifndef _NET_CARD_H_

#include "types.h"
#include "errno.h"
#include "list.h"

struct netcard {
	struct list ipv4_list;
};

err_t netcard_find_ip(struct netcard *netcard, uint32_t ip_addr);

#endif /* _NET_CARD_H_ */
