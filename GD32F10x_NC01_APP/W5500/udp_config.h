#ifndef __UDP_CONFIG_H
#define	__UDP_CONFIG_H
#include "gd32f10x.h"
#include "types.h"
#include "w5500_conf.h"
#include "string.h"
#include "socket.h"
#include "w5500.h"
#include "utility.h"
#define UDP_PORT   50000
#define SOCK_CONFIG  0
void do_udp_config(void);
#endif
