#include "gd32f10x.h"
#include "secstcp.h"
//#include "GXUHF.h"
#include "sio.h"
#include "queue.h"
uint32 secsrxLen=0;
uint32 secsLen=0;

uint8  secsbuf[256];
int secstcpstatus=-1;

/*
*@¹¦Ĝ£º  ±¾µع̼þ¸üЂ
*@²Ίý£º  Ξ
*@·µ»ؖµ£ºΞ
***/
void do_secstcp_cmd(void)
{
	extern sio_byte_buf_type 	secs_rx_buf;

  uint32 len=0;
  switch(getSn_SR(SOCK_SECS))        //»񈡳ocket SOCK_FWµė´̬
  {
  case SOCK_ESTABLISHED:           //socket Á¬½ӽ¨Á¢
    if(getSn_IR(SOCK_SECS) & Sn_IR_CON)
    {
      setSn_IR(SOCK_SECS, Sn_IR_CON);  //ǥ³ýsocket֐¶ύ
    }
		secstcpstatus=SOCK_ESTABLISHED;
    //some timeout function should be added here
    if((len=getSn_RX_RSR(SOCK_SECS))>0)
    {
			  int i=0;
        recv(SOCK_SECS, secsbuf, len);
				//if(ConfigMsg.debug>0)
					printf(".");

        secsrxLen+=len;
        //echo to PC program
        //send(SOCK_SECS, (uint8*)atbuf, (uint16)len);
				if(ConfigMsg.debug>0)
					printf(">secs rec len:%d\n",len);
#if SECS
				for(i=0;i<len;i++)
			  {
					uint8  inbyte=secsbuf[i];
					/*
					*  Length        			HSMS Message              Message Text in SECS-II 	
					* (4 Bytes)					Header (10 Bytes)            Encoding (0-4G Bytes)
					* 00 00 00 0D   00 01 81 01 00 00 00 00 00 01      21 01 66
					*/
					uint16_t pl_len=0;
					if ( secs_rx_buf.len  >= (SIO_BYTE_BUF_MAX - 1) ) 
          {
            secs_rx_buf.len = 0;
			      sio_byte_buf_flush(&secs_rx_buf);
          }

		      sio_byte_buf_enqueue(&secs_rx_buf, inbyte);

          if(secs_rx_buf.len >=14 )//\r\n
	        {
						pl_len=(secs_rx_buf.data[0]<<24)+(secs_rx_buf.data[1]<<16)+
							(secs_rx_buf.data[2]<<8)+secs_rx_buf.data[3]+4;
						 //printf("hsms len:%d %d S%dF%d\n",secs_rx_buf.len,pl_len,secs_rx_buf.data[6]&0x7f,secs_rx_buf.data[7]);
			      if(secs_rx_buf.len==pl_len)
			      {
				      secs_post_received_data(&secs_rx_buf.data[0],secs_rx_buf.len);
				      pl_len=0;
			      }
          }
	
			  }
#else
				for(i=0;i<len;i++)
			  {
					uint8  inbyte=secsbuf[i];
					/*
					*  
					* 5A 00 00 00 00 00 00
					* 
					*/
					uint16_t pl_len=0;
					if ( secs_rx_buf.len  >= (SIO_BYTE_BUF_MAX - 1) ) 
          {
            secs_rx_buf.len = 0;
			      sio_byte_buf_flush(&secs_rx_buf);
          }
					if ( (secs_rx_buf.len == 0 ) && (inbyte!=0x5A) ) 
		        return;
		      sio_byte_buf_enqueue(&secs_rx_buf, inbyte);

          if(secs_rx_buf.len ==7 )//\r\n
	        {
						pl_len=7;
						 //printf("hsms len:%d %d S%dF%d\n",secs_rx_buf.len,pl_len,secs_rx_buf.data[6]&0x7f,secs_rx_buf.data[7]);
			      if(secs_rx_buf.len==pl_len)
			      {
				      secs_post_received_data(&secs_rx_buf.data[0],secs_rx_buf.len);
				      pl_len=0;
			      }
          }
	
			  }
#endif/**/
    }
    break;
  case SOCK_CLOSE_WAIT:
    //disconnect(SOCK_FW);
		disconnect(SOCK_SECS);
		//printf("SOCK_CLOSE_WAIT\n");
    break;
  case SOCK_CLOSED:
		//printf("SOCK_CLOSED\n");
    close(SOCK_SECS);  
#if SECS
    socket(SOCK_SECS,Sn_MR_TCP,TCP_SECS_PORT,Sn_MR_ND);//½¨Á¢TCP·þαƷ
#else
	socket(SOCK_SECS,Sn_MR_TCP,TCP_AMKOR_NC01_PORT,Sn_MR_ND);//½¨Á¢TCP·þαƷ
	
#endif/***/
		secstcpstatus=SOCK_CLOSED;
    break;
  case SOCK_INIT:
    listen(SOCK_SECS);
		secstcpstatus=SOCK_INIT;
		//keep-alive  10seconds
		IINCHIP_WRITE(Sn_KPALVTR(SOCK_SECS),0x02);
    break;
  }
}

void send_secs_socket(uint8_t *rdata,uint16_t len)
{
	//printf("send data secstcpstatus:0x%x\n",secstcpstatus);
	if(secstcpstatus==SOCK_ESTABLISHED)
	{
		int ret=send(SOCK_SECS, (uint8*)rdata, (uint16)len);
		if(ret==0)
		{
			uint8_t sr=getSn_SR(SOCK_SECS);
			//printf("send data error:0x%x\n",sr);
			//do_attcp_cmd();
			//disconnect(SOCK_SECS);
			secstcpstatus=-1;	
		}
		
	}
}

//disconnect(SOCK_SECS);
void close_secssocket()
{
	if((secstcpstatus==SOCK_ESTABLISHED)&&(getSn_SR(SOCK_SECS)==SOCK_ESTABLISHED))
	{
		disconnect(SOCK_SECS);
		//printf("tcp time out\n");
	}
	else
	{
		//printf("close_atsocket tcpstatus:0x%x\n",tcpstatus);
	}
}
