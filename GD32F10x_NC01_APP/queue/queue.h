

#ifndef __QUEUE_H
#define __QUEUE_H
#include "gd32f10x.h"
#include "types.h"

#define TYPE_BARCODE  0 //FOR BARCODE DATA
#define TYPE_SECS     1 //FOR SECS
#define TYPE_LE_1     2// LF 1 DATA
#define TYPE_LF_2     3//LF 2 DATA
#define TYPE_TCP_AT  	4//TCP AT
#define TYPE_COM1     5 //FOR COM1 test
#define TYPE_COM2     6 //FOR COM2 test
#define TYPE_AMKOR_NC01 7 //for nc01 amkor cmd data
#define TYPE_AMKOR_NC01_RS232  8//for nc01 amkor rs232 cmd data
typedef struct _USERCMD
{
	uint8_t type;
	uint8_t len;
	uint8_t data[128+1];
}USERCMD;



#define MAX_RX_USERCMD_PACKETS_LEN 8
typedef  struct _USERCMD_PACKETS_LIST{
	u8 Done[MAX_RX_USERCMD_PACKETS_LEN];
	USERCMD usercmd[MAX_RX_USERCMD_PACKETS_LEN];
}USERCMD_PACKETS_LIST;

int usercmd_enque(USERCMD *rx,uint16_t len);

USERCMD * usercmd_deque(u8 *pkg_index );

int eth_rx_enque(USERCMD *rx,uint16_t len);

USERCMD * eth_rx_deque(u8 *pkg_index );

#endif



