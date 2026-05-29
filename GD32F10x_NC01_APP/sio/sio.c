#include <stdio.h>
#include <string.h>
#include "gd32f10x.h"
#include "queue.h"
#include "sio.h"

//!-------------------------------------------------------------------                 
//! Global Data Declaration                                                       
//!-------------------------------------------------------------------  
sio_byte_buf_type 	sio_rx_buf;
//sio_byte_buf_type 	usercmd_rx_buf;
//sio_byte_buf_type 	rs232_rx_buf;
sio_byte_buf_type 	secs_rx_buf;

sio_byte_buf_type 	rs232_rx_buf_console;

//static unsigned char HEX_BUFFER[128]={0};

void sio_byte_buf_flush(sio_byte_buf_type* ptr_self)
{
	memset((void*) ptr_self, 0x00, sizeof(sio_byte_buf_type));
}

void sio_byte_buf_enqueue(sio_byte_buf_type* ptr_self, uint16_t inbyte)
{
	ptr_self->data[ptr_self->len] = inbyte;
	ptr_self->len++;	
}

void sio_post_received_data(uint8_t* data, uint16_t len)
{
	USERCMD ucmd;
	ucmd.type=TYPE_TCP_AT;
	ucmd.len=len;
	memcpy((char *)ucmd.data,data,len);
//	printf("Get data packet:%d\n ",len);
//	for(int i=0;i<len;i++)
//	{
//		printf("%02X ",data[i]);
//	}
//	printf("\n");
	
	eth_rx_enque(&ucmd,len);

	sio_byte_buf_flush(&sio_rx_buf);
	return;
}

//void sio_post_received_data(uint8_t* data, uint16_t len)
//{
//	uint32_t frametype=CAN_FF_STANDARD;
//	uint32_t rtr=CAN_FT_DATA;//ft
//	uint32_t addr=(data[1]<< 24)|(data[2] << 16)|(data[3] << 8)|(data[4] << 0);
//	uint8_t dlc=data[0]&0x0f;
//	int i=0;
//	//_Debug2(9,"id:%x\n",addr);
//	//Send_Frame_CAN(addr,(u8 *)(data+5),8);//

//	if(data[0]&0x80)
//	{
//		frametype=CAN_FF_EXTENDED;
//	}
//	else
//		frametype=CAN_FF_STANDARD;
//	if(data[0]&0x40) //1:remote 
//	{
//		rtr=CAN_FT_REMOTE;
//	}
//	else
//		rtr=CAN_FT_DATA;
//	printf("Receive data:\n");
//	for(i=0;i<len;i++)
//	{
//		printf("%02X ",data[i]);
//	}
//	printf("\n");
//	Send_Frame_CAN_new(addr,frametype,rtr,(u8 *)(data+5),dlc);
//	sio_byte_buf_flush(&sio_rx_buf);
//}
void secs_post_received_data(uint8_t* data, uint16_t len)
{
	USERCMD ucmd;
#if SECS
	ucmd.type=TYPE_SECS;
#else
	ucmd.type=TYPE_AMKOR_NC01;
#endif/**/
	ucmd.len=len;
	memcpy((char *)ucmd.data,data,len);
  usercmd_enque(&ucmd,len);
	sio_byte_buf_flush(&secs_rx_buf);
}

void rs232_post_received_data(uint8_t* data, uint16_t len)
{
	USERCMD ucmd;
	ucmd.type=TYPE_AMKOR_NC01_RS232;

	ucmd.len=len;
	memcpy((char *)ucmd.data,data,len);
//	printf("Get data packet:\n ");
//	for(uint8_t i=0;i<len;i++)
//	{
//		printf("%02X ",data[i]);
//	}
//	printf("\n");
	usercmd_enque(&ucmd,len);
	sio_byte_buf_flush(&rs232_rx_buf_console);

}


