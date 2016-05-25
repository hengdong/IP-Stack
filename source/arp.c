/*
 * File         : arp.c
 * This file is part of POSIX-RTOS
 * COPYRIGHT (C) 2015 - 2016, DongHeng
 *
 * Change Logs:
 * DATA             Author          Note
 * 2015-05-28       DongHeng        create, just fir DXI-II frame
 */
#include "arp.h"
#include "ipv4.h"

/*********************************************************************************/
/*********************************************************************************/

/* ARP frame head */
STRUCT_BEGIN_FIRST
struct arp_fhr {
	uint16_t hw_type;
	uint16_t proto_type;

	uint8_t mac_size;
	uint8_t proto_size;
	uint16_t op_code;

	uint8_t  src_mac0t3[4];

	uint8_t  src_mac4t5[2];
	uint8_t  src_ip0t1[2];

	uint8_t  src_ip2t3[2];
	uint8_t  dest_mac0t1[2];

	uint8_t  dest_mac2t5[4];

	uint8_t  dest_ip[4];
};
STRUCT_END_FIRST

/* the correct ARP package size, it has no payload */
#define ARP_PKG_SIZE sizeof(struct arp_fh)

/* the ARP hardware type  */
#define ARP_HW_TYPE_IS_ETH 0x0001

/* the ARP protocol type */
#define ARP_PROTO_TYPE_IS_IP 0x0800

/* the ARP protocol size */
#define ARP_PROTO_SIZE_NOMAL 4

/* the ARP opcode */
#define ARP_OPCODE_REQUEST 0x0001
#define ARP_OPCODE_REPLY 0x0002

/*********************************************************************************/
/*********************************************************************************/

#define GET_ARP_HW_TYPE(arp) ((arp)->hw_type)
#define GET_ARP_MAC_SIZE(arp) ((arp)->mac_size)
#define GET_ARP_PROTO_TYPE(arp) ((arp)->proto_type)
#define GET_ARP_PROTO_SIZE(arp) ((arp)->proto_size)
#define GET_ARP_OPCODE(arp) ((arp)->op_code)
#define GET_ARP_SRC_MAC_POINT(arp) ((arp)->src_mac0t3)
#define GET_ARP_SRC_IP_POINT(arp) ((arp)->src_ip0t1)
#define GET_ARP_DEST_MAC_POINT(arp) ((arp)->dest_mac0t1)
#define GET_ARP_DEST_IP_POINT(arp) ((arp)->dest_ip)

#define PRINT_SHOW_DEST_MAC(arp) \
	(arp)->dest_mac0t1[0], (arp)->dest_mac0t1[1], (arp)->dest_mac2t5[0], \
	(arp)->dest_mac2t5[1], (arp)->dest_mac2t5[2], (arp)->dest_mac2t5[3]

#define PRINT_SHOW_SRC_MAC(arp) \
		(arp)->src_mac0t3[0], (arp)->src_mac0t3[1], (arp)->src_mac0t3[2], \
		(arp)->src_mac0t3[3], (arp)->src_mac4t5[0], (arp)->src_mac4t5[1]

#define CHECK_ARP_HW_TYPE(arp, type) (GET_ARP_HW_TYPE((arp)) == (type))
#define CHECK_ARP_MAC_SIZE(arp, size) (GET_ARP_MAC_SIZE((arp)) == (size))
#define CHECK_ARP_PROTO_TYPE(arp, type) (GET_ARP_PROTO_TYPE((arp)) == (type))
#define CHECK_ARP_PROTO_SIZE(arp, size) (GET_ARP_PROTO_SIZE((arp)) == (size))

#define SET_ARP_OPCODE(arp, opcode) GET_ARP_OPCODE((arp)) = (opcode)
#define SET_ARP_SRC_MAC(arp, mac) MEMCPY(GET_ARP_SRC_MAC_POINT(arp), (&mac), 6)
#define SET_ARP_SRC_IP(arp, ip) MEMCPY(GET_ARP_SRC_IP_POINT(arp), (&ip), 4)
#define SET_ARP_DEST_MAC(arp, mac) MEMCPY(GET_ARP_DEST_MAC_POINT(arp), (&mac), 6)
#define SET_ARP_DEST_IP(arp, ip) MEMCPY(GET_ARP_DEST_IP_POINT(arp), (&ip), 4)

#define TRANS_ARP_DEST_IPADDR(addr, arp) MEMCPY((&addr), GET_ARP_DEST_IP_POINT(arp), (4))
#define TRANS_ARP_DEST_MAC(mac, arp) MEMCPY((&mac), GET_ARP_DEST_MAC_POINT(arp), (6))

/*********************************************************************************/
/*********************************************************************************/

err_t arp_input(const uint8_t *pbuff, uint16_t size,struct netcard *netcard)
{
	struct arp_fhr *arp_fhr;
	err_t ret;
	uint32_t local_ip;

	if (size < ARP_PKG_SIZE) {
		DEBUG_ARP_LAYER("ARP size error\n");
		ret = 0;
		goto gret;
	}

	arp_fhr = (struct arp_fhr *)pbuff;
	if (!CHECK_ARP_HW_TYPE(arp_fhr, ARP_HW_TYPE_IS_ETH) ||
		!CHECK_ARP_MAC_SIZE(arp_fhr, MAC_ADRR_BYTES) ||
		!CHECK_ARP_PROTO_TYPE(arp_fhr, ARP_PROTO_TYPE_IS_IP) ||
		!CHECK_ARP_PROTO_SIZE(arp_fhr, ARP_PROTO_SIZE_NOMAL)) {
		DEBUG_ARP_LAYER("check ARP error, hardware type:[%d], hardware size:[%d], protocol type:[%d], "
				"protocol size:[%d]\n", GET_ARP_HW_TYPE(arp_fhr), GET_ARP_MAC_SIZE(arp_fhr),
				GET_ARP_PROTO_TYPE(arp_fhr), GET_ARP_PROTO_SIZE(arp_fhr));
		ret = -EINVAL;
		goto gret;
	}

	TRANS_ARP_DEST_IPADDR(local_ip, arp_fhr);
	if (ipv4_find_node(netcard, local_ip)) {
		DEBUG_ARP_LAYER("ARP is not for us, desertion MAC:[%d.%d.%d.%d.%d.%d]\n",
				PRINT_SHOW_DEST_MAC(arp_fhr));
		ret = 0;
		goto gret;
	}

	DEBUG_ARP_LAYER("ARP opcode:[%d]\n", GET_ARP_OPCODE(arp_fhr));
	switch(GET_ARP_OPCODE(arp_fhr)) {
		case ARP_OPCODE_REQUEST:
			DEBUG_ARP_LAYER("send ARP reply to MAC:[%d.%d.%d.%d.%d.%d]\n",
					PRINT_SHOW_SRC_MAC(arp_fhr));

			SET_ARP_OPCODE(arp_fhr, ARP_OPCODE_REPLY);
			SET_ARP_DEST_MAC(arp_fhr, GET_ARP_SRC_MAC_POINT(arp_fhr));
			SET_ARP_DEST_IP(arp_fhr, GET_ARP_SRC_IP_POINT(arp_fhr));
			SET_ARP_SRC_MAC(arp_fhr, netcard_get_mac(netcard));
			SET_ARP_SRC_IP(arp_fhr, &local_ip);

			ret = ethernet_output(pbuff);
			break;

		case ARP_OPCODE_REPLY:
			break;

		default :
			ret = -EPROTONOSUPPORT;
			break;
	}

gret:
    DEBUG_ARP_LAYER("ARP handled is [%d]\n", ret);
	return ret;
}
