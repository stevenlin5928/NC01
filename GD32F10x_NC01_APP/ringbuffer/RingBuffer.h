#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__ 
#include <stdint.h>

#define USER_RINGBUFF  1  //
#if  USER_RINGBUFF
#define  RINGBUFF_LEN         8192 //8192     // 200//1024
#define  FALSE   1 
#define  TRUE    0 
void RingBuff_Init(void);
uint16_t Write_RingBuff(uint8_t data);
uint16_t Read_RingBuff(uint8_t *rData);
uint16_t Write_RingBuff_str(uint8_t *buf,uint8_t len);
uint16_t ringbuffer_avail(void);
uint16_t Read_RingBuff_packet(uint8_t *rData,int maxlen);
uint16_t ringbuffer_free(void);
void ringbuffer_reset(void);
#endif/*USER_RINGBUFF*/
 
#endif/**/
