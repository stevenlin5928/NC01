#include <stdio.h>
#include <string.h>
#include "RingBuffer.h"


typedef struct
{
    uint16_t Head;
    uint16_t Tail;
    uint16_t Lenght;
    uint8_t Ring_Buff[RINGBUFF_LEN];
}RingBuff_t;
RingBuff_t ringBuff;//

/**
* @brief  RingBuff_Init
* @param  void
* @return void
* @date   2018
* @version v1.0
* @note   
*/
void RingBuff_Init(void)
{
   //
   ringBuff.Head = 0;
   ringBuff.Tail = 0;
   ringBuff.Lenght = 0;
}

/**
* @brief  Write_RingBuff
* @param  u8 data
* @return FALSE:Fail;TRUE:Success
* @author 
* @date   2018
* @version v1.0
* @note  
*/
uint16_t Write_RingBuff(uint8_t data)
{
    ringBuff.Ring_Buff[ringBuff.Tail]=data;
//    ringBuff.Tail++;
    ringBuff.Tail = (ringBuff.Tail+1)%RINGBUFF_LEN;//
    ringBuff.Lenght++;
    return TRUE;
}

uint16_t Write_RingBuff_str(uint8_t *buf,uint8_t len)
{
	int i=0;
	uint16_t leftspace=ringbuffer_free();
	if(leftspace<len)
	{
		printf("o");
		return FALSE;
	}
	for(i=0;i<len;i++)
	{
		Write_RingBuff(buf[i]);
	}
	return TRUE; 
}

/**
* @brief  Read_RingBuff
* @param  u8 *rData,
* @return FALSE:Fail;TRUE:Success
* @author 
* @date   2018
* @version v1.0
* @note   
*/
uint16_t Read_RingBuff(uint8_t *rData)
{
   if(ringbuffer_avail() == 0)//????
    {
       return FALSE;
    }
   *rData = ringBuff.Ring_Buff[ringBuff.Head];//????FIFO,??????
//   ringBuff.Head++;
   ringBuff.Head = (ringBuff.Head+1)%RINGBUFF_LEN;//????????
   return TRUE;
}

uint16_t Read_RingBuff_packet(uint8_t *rData,int maxlen)
{
	 uint16_t pklen=0;
	 uint16_t i=0;
	 uint8_t bHeader=0;
	
   if(ringbuffer_avail() <9)//????
   {
       return FALSE;
   }
		//find header
		for(i=0;i<9;i++)
		{
			uint8_t header= ringBuff.Ring_Buff[ringBuff.Head];

			if(header==0x5A)
			{
				bHeader=1;
				ringBuff.Head = (ringBuff.Head+1)%RINGBUFF_LEN;
				break;
			}
			ringBuff.Head = (ringBuff.Head+1)%RINGBUFF_LEN;
		}
		if(bHeader==0)
		{
			return FALSE;
		}

		rData[0]=0x5A;
		if(ringbuffer_avail() <9)//????
    {
       return FALSE;
    }
		for(i=1;i<9;i++)
		{
			rData[i] = ringBuff.Ring_Buff[ringBuff.Head];//????FIFO,??????
			ringBuff.Head = (ringBuff.Head+1)%RINGBUFF_LEN;//????????
		}
		pklen=((rData[5]<<8)|rData[6]);
		if(ringbuffer_avail() <pklen)//????
    {
       return FALSE;
    }
		for(i=0;i<pklen;i++)
		{
			rData[9+i] = ringBuff.Ring_Buff[ringBuff.Head];//????FIFO,??????
			ringBuff.Head = (ringBuff.Head+1)%RINGBUFF_LEN;//????????
		}
		
   return TRUE;
}

//have space
uint16_t ringbuffer_free()
{
	int16_t free;
#if 1
	free =  ringBuff.Head-ringBuff.Tail;
	if (ringBuff.Head <=ringBuff.Tail)
	{
		free = RINGBUFF_LEN-ringBuff.Tail+ringBuff.Head;
	}
	else
		free =ringBuff.Head-ringBuff.Tail;
#else
	free =  ringBuff.Head-ringBuff.Tail;
	if (free <=0)
		free += RINGBUFF_LEN;
#endif/**/	
	return free-1;
}

//avail data
uint16_t ringbuffer_avail()
{
	int16_t avail;

	avail = ringBuff.Tail - ringBuff.Head;
	if (avail < 0)
		avail += RINGBUFF_LEN;
	return avail;
}

uint8_t ringbuffer_empty()
{
	return (ringBuff.Tail== ringBuff.Head);
}

void ringbuffer_reset()
{
	ringBuff.Tail=ringBuff.Head = 0;
	ringBuff.Lenght=0;
}

void dump_ringbuffer()
{
	printf("rb.Tail:%d,Head:%d,%d",ringBuff.Tail,ringBuff.Head,ringbuffer_avail());
}
