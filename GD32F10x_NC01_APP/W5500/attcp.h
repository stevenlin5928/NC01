#ifndef __AT_TCP_H
#define	__AT_TCP_H

#include "types.h"
#include "w5500_conf.h"
#include "string.h"
#include "socket.h"
#include "w5500.h"
#define SOH (0xFF)
#define TCP_AT_PORT 20001

#define SOCK_AT 2

void do_attcp_cmd(void);
void close_atsocket(void);
void send_tcp_socket(uint8_t *rdata,uint16_t len);
#endif
