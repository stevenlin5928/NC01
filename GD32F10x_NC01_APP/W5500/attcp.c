#include "gd32f10x.h"
#include "attcp.h"

uint32 atrxLen=0;
uint32 atLen=0;

uint8  atbuf[256];
#include "sio.h"

int tcpstatus=-1;

/*
*@功能：  本地固件更新
*@参数：  无
*@返回值：无
***/
void do_attcp_cmd(void)
{
	extern sio_byte_buf_type 	sio_rx_buf;

  uint32 len=0;
  switch(getSn_SR(SOCK_AT))        //获取socket SOCK_FW的状态
  {
  case SOCK_ESTABLISHED:           //socket 连接建立
    if(getSn_IR(SOCK_AT) & Sn_IR_CON)
    {
      setSn_IR(SOCK_AT, Sn_IR_CON);  //清除socket中断
    }
		tcpstatus=SOCK_ESTABLISHED;
    //some timeout function should be added here
    if((len=getSn_RX_RSR(SOCK_AT))>0)
    {
			  int i=0;
        recv(SOCK_AT, atbuf, len);
//				if(ConfigMsg.debug>0)
//					printf(".");

        atrxLen+=len;
        //echo to PC program
        //send(SOCK_AT, (uint8*)atbuf, (uint16)len);
//				if(ConfigMsg.debug>0)
//					printf(">Tcp rec len:%d\n",len);
			  if(atbuf[0]!=(uint8_t)0xff)
				{
					if(ConfigMsg.debug>0)
						printf("invalid command:%02X",atbuf[0]);
					break;
				}

				for(i=0;i<len;i++)
			  {
					uint8  inbyte=atbuf[i];
					//printf("%02X ",inbyte);
#if 1
					//FF 00 00 01 5B 82
					uint16_t pl_len=0;
					if ( sio_rx_buf.len  >= (SIO_BYTE_BUF_MAX - 1) ) 
          {
            sio_rx_buf.len = 0;
			      sio_byte_buf_flush(&sio_rx_buf);
          }

          if ( (sio_rx_buf.len == 0 ) && (inbyte!=SOH) ) 
		        return;

		      sio_byte_buf_enqueue(&sio_rx_buf, inbyte);
          //nuhfdatabuflen = 7 + dlen + 2;//add 7-byte header and 2-byte crc
          if(sio_rx_buf.len >5 )//\r\n
	        {
			      pl_len=((sio_rx_buf.data[1]<<8)|sio_rx_buf.data[2])+6;
			      if(sio_rx_buf.len==pl_len)
			      {
				      sio_post_received_data(&sio_rx_buf.data[0],sio_rx_buf.len);
				      pl_len=0;
			      }
          }
#else				
				  if ( sio_rx_buf.len  >= (SIO_BYTE_BUF_MAX - 1) ) 
          {
            sio_rx_buf.len = 0;
			      sio_byte_buf_flush(&sio_rx_buf);
          }

          if ( (sio_rx_buf.len == 0 ) && (inbyte!='A') ) 
					  return;
	        if ( (sio_rx_buf.len == 1 ) && (inbyte!='T') ) 
					  return;
					
				  sio_byte_buf_enqueue(&sio_rx_buf, inbyte);

				  

				  if((sio_rx_buf.len < SIO_BYTE_BUF_MAX)&&(inbyte!='\r'))//\r\n
				  {
				    sio_post_received_data(&sio_rx_buf.data[0],sio_rx_buf.len);
   			  }
#endif/**/	
			  }
    }
    break;
  case SOCK_CLOSE_WAIT:
    //disconnect(SOCK_FW);
		disconnect(SOCK_AT);
		//printf("SOCK_CLOSE_WAIT\n");
    break;
  case SOCK_CLOSED:
		//printf("SOCK_CLOSED\n");
    close(SOCK_AT);  
    socket(SOCK_AT,Sn_MR_TCP,TCP_AT_PORT,Sn_MR_ND);//建立TCP服务器
		tcpstatus=SOCK_CLOSED;
    break;
  case SOCK_INIT:
    listen(SOCK_AT);
		tcpstatus=SOCK_INIT;
    break;
  }
}


void send_tcp_socket(uint8_t *rdata,uint16_t len)
{
	if(tcpstatus==SOCK_ESTABLISHED)
	{
		int ret=send(SOCK_AT, (uint8*)rdata, (uint16)len);
		if(ret==0)
		{
			uint8_t sr=getSn_SR(SOCK_AT);
			//printf("send data error:0x%x\n",sr);
			//do_attcp_cmd();
			//disconnect(SOCK_AT);
			tcpstatus=-1;	
		}
		
	}
}

//disconnect(SOCK_AT);
void close_atsocket()
{
	if((tcpstatus==SOCK_ESTABLISHED)&&(getSn_SR(SOCK_AT)==SOCK_ESTABLISHED))
	{
		disconnect(SOCK_AT);
		//printf("tcp time out\n");
	}
	else
	{
		//printf("close_atsocket tcpstatus:0x%x\n",tcpstatus);
	}
}
