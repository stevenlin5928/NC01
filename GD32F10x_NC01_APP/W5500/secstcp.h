#ifndef __SECS_TCP_H
#define	__SECS_TCP_H

#include "types.h"
#include "w5500_conf.h"
#include "string.h"
#include "socket.h"
#include "w5500.h"

//#define TCP_SECS_PORT 5000
//#define TCP_AMKOR_NC01_PORT 1883
#define TCP_SECS_PORT 5000
#define TCP_AMKOR_NC01_PORT 1883
#define SOCK_SECS 3

void do_secstcp_cmd(void);
void close_secssocket(void);
void send_secs_socket(uint8_t *rdata,uint16_t len);
#endif
