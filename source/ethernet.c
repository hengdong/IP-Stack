/*
 * File         : ethernet.c
 * This file is part of POSIX-RTOS
 * COPYRIGHT (C) 2015 - 2016, DongHeng
 *
 * Change Logs:
 * DATA             Author          Note
 * 2015-05-28       DongHeng        create, just fir DXI-II frame
 */
#include "ethernet.h"
#include "arp.h"

/*********************************************************************************/
/*********************************************************************************/
/* the checking algorithm is CRC-32 */
#define ETH_PKG_CRC_BYTES 4

/* Ethernet frame head region, just for DXI-II frame */
STRUCT_BEGIN_FIRST
struct eth_fhr {
#if CONFIG_ETH_PREAMBLES
	uint8_t preamble[PREAMBLE_BYTES];
#endif

	uint8_t dest_mac[MAC_ADRR_BYTES];
	uint8_t src_mac[MAC_ADRR_BYTES];

	uint16_t type;
};
STRUCT_END_FIRST

/* the minimum size of the Ethernet package is the size of head plus CRC-32 */
#define ETH_PKG_SIZE (sizeof(struct eth_fh) + ETH_PKG_CRC_BYTES)

/* the first 24bit data of the MAC address of the multicast package */
#define MAC_MULTICAST_ADDR0 0x01
#define MAC_MULTICAST_ADDR1 0x00
#define MAC_MULTICAST_ADDR2 0x5e

/* the upper layer frame type of the Ethernet frame */
#define ETHERNET_IP_PACKAGE 0x0800
#define ETHERNET_ARP_PACKAGE 0x0806

/*********************************************************************************/
/*********************************************************************************/

#define PRINT_SHOW_MAC(arg) arg[0], arg[1], arg[2], arg[3], arg[4], arg[5]

#define SHOW_DEST_MAC(eth) PRINT_SHOW_MAC((eth)->dest_mac)
#define SHOW_SRC_MAC(eth) PRINT_SHOW_MAC((eth)->src_mac)

#define GET_DEST_MAC_OFF_ADDR(eth, off) (eth)->dest_mac[off]
#define GET_SRC_MAC_OFF_ADDR(eth, off) (eth)->src_mac[off]

#define MAC_IS_NOT_FIX(eth) \
	(GET_SRC_MAC_OFF_ADDR((eth), 0) & 0x01)

#define MAC_IS_MULTICAST(eth) \
	(GET_SRC_MAC_OFF_ADDR((eth), 0) == MAC_MULTICAST_ADDR0 && \
	 GET_SRC_MAC_OFF_ADDR((eth), 1) == MAC_MULTICAST_ADDR1 && \
	 GET_SRC_MAC_OFF_ADDR((eth), 2) == MAC_MULTICAST_ADDR2)

#define MAC_IS_BROADCAST(eth) \
		(GET_SRC_MAC_OFF_ADDR((eth), 0) == 0xff && \
		 GET_SRC_MAC_OFF_ADDR((eth), 1) == 0xff && \
		 GET_SRC_MAC_OFF_ADDR((eth), 2) == 0xff && \
		 GET_SRC_MAC_OFF_ADDR((eth), 3) == 0xff && \
		 GET_SRC_MAC_OFF_ADDR((eth), 4) == 0xff && \
		 GET_SRC_MAC_OFF_ADDR((eth), 5) == 0xff )

#define GET_ETH_PKG_TYPE(eth) ((eth)->type)

/*********************************************************************************/
/*********************************************************************************/

err_t ethernet_input(const uint8_t *pbuff, uint16_t size,struct net_card *net_card)
{
	struct eth_fhr *eth_fhr;
	err_t ret;
	uint16_t plen;
	const char *payload;

	if (!net_card || !pbuff || size <= ETH_PKG_SIZE ||
		size > (ETH_PKG_SIZE + ETH_MTU)) {
		DEBUG_ETH_LAYER("input ARG invaluable, card:[%x], buff:[%x], size:[%d]\n",
				(phy_addr_t)net_card, (phy_addr_t)pbuff, size);
		ret = -EINVAL;
		goto gret;
	}

	eth_fhr = (struct eth_fhr *)pbuff;
	DEBUG_ETH_LAYER("desertion MAC address is [%x.%x.%x.%x.%x.%x], source MAC address"
			" is [%x.%x.%x.%x.%x.%x]\n", SHOW_DEST_MAC(eth_fhr), SHOW_SRC_MAC(eth_fhr));
	if (MAC_IS_NOT_FIX(eth_fhr)) {
		if (MAC_IS_MULTICAST(eth_fhr)) {
			DEBUG_ETH_LAYER("it is multicast frame\n");
		} else if (MAC_IS_BROADCAST(eth_fhr)) {
			DEBUG_ETH_LAYER("it is broadcast frame\n");
		}
	}

	DEBUG_ETH_LAYER("Ethernet package type is [%d] with net endian\n", GET_ETH_PKG_TYPE(eth_fhr));
	payload = pbuff + sizeof(struct eth_fh);
	plen = size - ETH_PKG_SIZE;
	switch(GET_ETH_PKG_TYPE(eth_fhr)) {
		case ETHERNET_IP_PACKAGE:
			DEBUG_ETH_LAYER("input IP-layer size is [%d]\n", plen);
			ret = ip_input(payload, plen, net_card);
			break;

		case ETHERNET_ARP_PACKAGE:
			DEBUG_ETH_LAYER("input ARP-layer size is [%d]\n", plen);
			ret = arp_input(payload, plen, net_card);
			break;

		default :
			ret = -EPROTONOSUPPORT;
			break;
	}

gret:
    DEBUG_ETH_LAYER("Ethernet package handled is [%d]\n", ret);
	POSIX_FREE(pbuff);
	return ret;
}
