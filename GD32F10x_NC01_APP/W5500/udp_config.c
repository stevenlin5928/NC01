#include "udp_config.h"

 uint8 sckState;
 uint16 len;
 uint8 rIP[4];     // 远程UDP的IP
 uint16 rPort;     // 远程UDP的端口 
 uint8 bIP[4]={255,255,255,255};
 
void do_udp_config(void)
{
 
  //clear RecvMsg
  memset(&RecvMsg,0,sizeof(RecvMsg));
  
  sckState=getSn_SR(SOCK_CONFIG);
	
	
  switch(sckState)
  {
  case SOCK_UDP:
    if((len=getSn_RX_RSR(SOCK_CONFIG))>0)
    {
			//printf("udp len:%d\n",len);
      if(len>MAX_BUF_SIZE) len=MAX_BUF_SIZE;
      recvfrom(SOCK_CONFIG, (uint8 *)&RecvMsg, len, rIP, &rPort);
      //FIND: searching, SETT: setting, FIRM: firmware uploading
	    //printf("op:%c %c %c %c \n",RecvMsg.op[0],RecvMsg.op[1],RecvMsg.op[2],RecvMsg.op[3]);
      if((RecvMsg.op[0]=='F') && (RecvMsg.op[1]=='I') && (RecvMsg.op[2]=='N') && (RecvMsg.op[3]=='D'))//FIND??,????????????
      {
        RecvMsg = ConfigMsg;
        RecvMsg.op[0]='F';RecvMsg.op[1]='I';RecvMsg.op[2]='N';RecvMsg.op[3]='D';
       
        //response
				strncpy((char*)RecvMsg.model,"CTC",8);
        sendto(SOCK_CONFIG, ( uint8 *)&RecvMsg, sizeof(RecvMsg), bIP, rPort);
        //printf("Find\r\n");
      }
      else if((RecvMsg.op[0]=='S') && (RecvMsg.op[1]=='E') && (RecvMsg.op[2]=='T') && (RecvMsg.op[3]=='T'))//SETT??,????????????
      {
				//printf("SEET\r\n");
        if((RecvMsg.mac[0]==ConfigMsg.mac[0]) && (RecvMsg.mac[1]==ConfigMsg.mac[1]) && (RecvMsg.mac[2]==ConfigMsg.mac[2]) && 
           (RecvMsg.mac[3]==ConfigMsg.mac[3]) && (RecvMsg.mac[4]==ConfigMsg.mac[4]) && (RecvMsg.mac[5]==ConfigMsg.mac[5]))
        {
					uint8 msg[]="OK+SETT";
					
					memcpy(ConfigMsg.lip, RecvMsg.lip, 4);
					memcpy(ConfigMsg.sub, RecvMsg.sub, 4);
					memcpy(ConfigMsg.gw, RecvMsg.gw, 4);
					memcpy(ConfigMsg.dns, RecvMsg.dns, 4);
          //ConfigMsg = RecvMsg;//获取的配置信息复制到配置结构体
          write_config_to_flash();//将配置信息写入到Flash
					
					sendto(SOCK_CONFIG, msg, strlen(msg),bIP, rPort);
					delay_ms(300);
          reboot();//软件复位
        }
      }
      else if((RecvMsg.op[0]=='F') && (RecvMsg.op[1]=='I') && (RecvMsg.op[2]=='R') && (RecvMsg.op[3]=='M'))//FIRM??,?????,??????
      {
				//printf("FIRM\r\n");
        //printf("firmware upload\n");
        if((RecvMsg.mac[0]==ConfigMsg.mac[0]) && (RecvMsg.mac[1]==ConfigMsg.mac[1]) && (RecvMsg.mac[2]==ConfigMsg.mac[2]) && 
           (RecvMsg.mac[3]==ConfigMsg.mac[3]) && (RecvMsg.mac[4]==ConfigMsg.mac[4]) && (RecvMsg.mac[5]==ConfigMsg.mac[5]))
        {    
          //response
          sendto(SOCK_CONFIG, (uint8 *)&RecvMsg, sizeof(ConfigMsg),bIP, rPort);
        }
      }
			else if((RecvMsg.op[0]=='R') && (RecvMsg.op[1]=='E') && (RecvMsg.op[2]=='S') && (RecvMsg.op[3]=='E'))//RESET??,?????,??????
      {
				//printf("RESET\r\n");
        //printf("firmware upload\n");
        if((RecvMsg.mac[0]==ConfigMsg.mac[0]) && (RecvMsg.mac[1]==ConfigMsg.mac[1]) && (RecvMsg.mac[2]==ConfigMsg.mac[2]) && 
           (RecvMsg.mac[3]==ConfigMsg.mac[3]) && (RecvMsg.mac[4]==ConfigMsg.mac[4]) && (RecvMsg.mac[5]==ConfigMsg.mac[5]))
        {    
          //response
          sendto(SOCK_CONFIG, (uint8 *)&RecvMsg, sizeof(ConfigMsg),bIP, rPort);
					delay_ms(300);
					reboot();//软件复位
        }
      }
			else if((RecvMsg.op[0]=='T') && (RecvMsg.op[1]=='R') && (RecvMsg.op[2]=='I') && (RecvMsg.op[3]=='G'))//route to rs232 or ethernet
      {
				//printf("Route to serial or ethernet\r\n");
        if((RecvMsg.mac[0]==ConfigMsg.mac[0]) && (RecvMsg.mac[1]==ConfigMsg.mac[1]) && (RecvMsg.mac[2]==ConfigMsg.mac[2]) && 
           (RecvMsg.mac[3]==ConfigMsg.mac[3]) && (RecvMsg.mac[4]==ConfigMsg.mac[4]) && (RecvMsg.mac[5]==ConfigMsg.mac[5]))
        {    
          uint8 * buf=(uint8 *)&RecvMsg;
					//offset 5: rs232 or ethernet ,baudrate
					uint8 route=buf[10];
					if(route=='?')
					{
						ConfigMsg.op[0]='T';ConfigMsg.op[1]='R';ConfigMsg.op[2]='I';ConfigMsg.op[3]='G';
						sendto(SOCK_CONFIG, ( uint8 *)&ConfigMsg, sizeof(ConfigMsg), bIP, rPort);
						RecvMsg = ConfigMsg;
						
						delay_ms(80);	
						return;
					}
					else
					{
						uint32 reset_d_latch=(uint32)((buf[11]<<24)|(buf[12]<<16)|(buf[13]<<8)|(buf[14]<<0));
						if((reset_d_latch<500)&&(reset_d_latch>10000))
						{//
							//skip
							return ;
						}
						uint32 bar_trig_pressed_time=(uint32)((buf[15]<<24)|(buf[16]<<16)|(buf[17]<<8)|(buf[18]<<0));
						ConfigMsg.reset_D_latch_time=reset_d_latch;
						ConfigMsg.barcode_trigger_pressed_time=bar_trig_pressed_time;
						

						buf[10] = (uint8)(reset_d_latch<<24)&0xff;//reset_d_latch
						buf[11] = (uint8)(reset_d_latch<<16)&0xff;//
						buf[12] = (uint8)(reset_d_latch<<8)&0xff;//
						buf[13] = (uint8)(reset_d_latch<<0)&0xff;//
						buf[14] = (uint8)(bar_trig_pressed_time<<24)&0xff;//Baudrate MSB
						buf[15] = (uint8)(bar_trig_pressed_time<<16)&0xff;//
						buf[16] = (uint8)(bar_trig_pressed_time<<8)&0xff;//
						buf[17] = (uint8)(bar_trig_pressed_time<<0)&0xff;//Baudrate LSB
						ConfigMsg.op[0]='T';ConfigMsg.op[1]='R';ConfigMsg.op[2]='I';ConfigMsg.op[3]='G';
						sendto(SOCK_CONFIG, ( uint8 *)&ConfigMsg, sizeof(ConfigMsg), bIP, rPort);
						RecvMsg = ConfigMsg;
						
						write_config_to_flash();//将配置信息写入到Flash
						delay_ms(80);							
					}
        }
      }
			else if((RecvMsg.op[0]=='D') && (RecvMsg.op[1]=='B') && (RecvMsg.op[2]=='U') && (RecvMsg.op[3]=='G'))//route to rs232 or ethernet
      {
				//printf("Route to serial or ethernet\r\n");
        if((RecvMsg.mac[0]==ConfigMsg.mac[0]) && (RecvMsg.mac[1]==ConfigMsg.mac[1]) && (RecvMsg.mac[2]==ConfigMsg.mac[2]) && 
           (RecvMsg.mac[3]==ConfigMsg.mac[3]) && (RecvMsg.mac[4]==ConfigMsg.mac[4]) && (RecvMsg.mac[5]==ConfigMsg.mac[5]))
        {    
				
					if(RecvMsg.debug=='?')
					{
						ConfigMsg.op[0]='D';ConfigMsg.op[1]='B';ConfigMsg.op[2]='U';ConfigMsg.op[3]='G';
						sendto(SOCK_CONFIG, ( uint8 *)&ConfigMsg, sizeof(ConfigMsg), bIP, rPort);
						
						delay_ms(80);	
						return;
					}
					else
					{
						if(RecvMsg.debug>10)
							return;
						ConfigMsg.debug=RecvMsg.debug;
						ConfigMsg.op[0]='D';ConfigMsg.op[1]='B';ConfigMsg.op[2]='U';ConfigMsg.op[3]='G';
						sendto(SOCK_CONFIG, ( uint8 *)&ConfigMsg, sizeof(ConfigMsg), bIP, rPort);

						write_config_to_flash();//将配置信息写入到Flash
						delay_ms(80);							
					}
        }
      }
    }
    break;
  case SOCK_CLOSED:
		
    socket(SOCK_CONFIG, Sn_MR_UDP, UDP_PORT, 0);
    break;
  }
}
