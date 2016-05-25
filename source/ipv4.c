/*
 * File         : ipv4.c
 * This file is part of POSIX-RTOS
 * COPYRIGHT (C) 2015 - 2016, DongHeng
 *
 * Change Logs:
 * DATA             Author          Note
 * 2015-05-28       DongHeng        create, just fir DXI-II frame
 */
#include "ipv4.h"

err_t ipv4_find_node(uint32_t ip_addr, struct net_card *net_card)
{
	err_t ret = -1;
	struct ipv4_node *pnode;

	LIST_FOR_EACH_ENTRY(pnode, net_card->ipv4_list, struct ipv4_node, list) {
		if (pnode->addr == ip_addr) {
			ret = 0;
			break;
		}
	}


	return ret;
}
