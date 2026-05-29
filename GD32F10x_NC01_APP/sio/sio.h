

#ifndef __SIO_H
#define __SIO_H
#include "gd32f10x.h"
#include "types.h"
#define  SECS  1
#define SIO_BYTE_BUF_MAX 	(128)


typedef struct
{
	uint16_t len;
	uint8_t data[SIO_BYTE_BUF_MAX];
} sio_byte_buf_type;

void sio_byte_buf_flush(sio_byte_buf_type* ptr_self);
void sio_byte_buf_enqueue(sio_byte_buf_type* ptr_self, uint16_t inbyte);
void sio_post_received_data(uint8_t* data, uint16_t len);
void barcode_post_received_data(uint8_t* data, uint16_t len);
void secs_post_received_data(uint8_t* data, uint16_t len);

#endif



