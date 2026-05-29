#include<string.h>
#include "stdio.h"
#include "queue.h"


USERCMD_PACKETS_LIST usercmd_packets_list={
	{0,0,0,0},
	{0},
};


USERCMD_PACKETS_LIST eth_rx_packets_list={
	{0,0,0,0},
	{0},
};


int usercmd_enque(USERCMD *rx,uint16_t len)
{
	int i=0;
	for(i=0;i<MAX_RX_USERCMD_PACKETS_LEN;i++)
	{
		if(usercmd_packets_list.Done[i]==1)
		{
			continue;
		}
		else
		{
			//usercmd_packets_list.usercmd[i].DLC=rx->DLC;
			
			//memcpy(rx_packets_list.rxmsg[i].Data,rx->Data,rx->DLC);
			memcpy(usercmd_packets_list.usercmd[i].data,rx->data,(uint32_t)len);
			usercmd_packets_list.Done[i]=1;
			usercmd_packets_list.usercmd[i].type=rx->type;
			usercmd_packets_list.usercmd[i].len=len;
			//_Debug2(9,"%s index:%d\n",__func__,i);
			break;
		}
	}
	return 0;
}

USERCMD * usercmd_deque(u8 *pkg_index )
{
	int i=0;
	for(i=0;i<MAX_RX_USERCMD_PACKETS_LEN;i++)
	{
		if(usercmd_packets_list.Done[i]==1)
		{
			*pkg_index=i;
			return &usercmd_packets_list.usercmd[i];
		}
		else
		{

		}
	}
	return NULL;
}


int eth_rx_enque(USERCMD *rx,uint16_t len)
{
	int i=0;
	for(i=0;i<MAX_RX_USERCMD_PACKETS_LEN;i++)
	{
		if(eth_rx_packets_list.Done[i]==1)
		{
			continue;
		}
		else
		{
			//usercmd_packets_list.usercmd[i].DLC=rx->DLC;
			
			//memcpy(rx_packets_list.rxmsg[i].Data,rx->Data,rx->DLC);
			memcpy(eth_rx_packets_list.usercmd[i].data,rx->data,len);
			eth_rx_packets_list.usercmd[i].len=len;
			eth_rx_packets_list.Done[i]=1;
			break;
		}
	}
	return 0;
}

USERCMD * eth_rx_deque(u8 *pkg_index )
{
	int i=0;
	for(i=0;i<MAX_RX_USERCMD_PACKETS_LEN;i++)
	{
		if(eth_rx_packets_list.Done[i]==1)
		{
			*pkg_index=i;
			return &eth_rx_packets_list.usercmd[i];
		}
		else
		{

		}
	}
	return NULL;
}
