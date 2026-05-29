#include "fw_update.h"

uint32 rxLen=0;
uint32 fLen=0;
uint32 nPage=0;
uint32 nErasedPage=0;
uint32 flashDest = AppBackupAddress;
uint16 pageSize=0x400;
uint8  buf[1024];
uint8  isErased=0;
extern uint8 update_flag ;   
/*
*@功能：  本地固件更新
*@参数：  无
*@返回值：无
***/
void do_fw_update(void)
{
  uint32 len=0;
  uint32 i;
  switch(getSn_SR(SOCK_FW))        //获取socket SOCK_FW的状态
  {
  case SOCK_ESTABLISHED:           //socket 连接建立
    if(getSn_IR(SOCK_FW) & Sn_IR_CON)
    {
      //解锁 Flash
      fmc_unlock();//解锁Flash，为Flash更新做准备
      //清除所有待处理标记
      //fmc_flag_clear(FMC_FLAG_BUSY | FMC_FLAG_END | FMC_FLAG_PGERR | FMC_FLAG_WRPRTERR);
			fmc_flag_clear(FMC_FLAG_BANK0_END);
			fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
			fmc_flag_clear(FMC_FLAG_BANK0_PGERR);	
      setSn_IR(SOCK_FW, Sn_IR_CON);  //清除socket中断
    }
    //some timeout function should be added here
    if((len=getSn_RX_RSR(SOCK_FW))>0)
    {
      if((len==4) && (isErased==0))   
      {
        recv(SOCK_FW, (uint8*)&fLen,4); //升级文件的大小
        
        nPage=FLASH_PagesMask(fLen);    //擦写叶数 1k
//        
        for(nErasedPage=0; nErasedPage<nPage; nErasedPage++)
        {
          //FLASH_ErasePage(flashDest + pageSize*nErasedPage);//擦除Flash中APP程序区域
					fmc_page_erase(flashDest + pageSize*nErasedPage);
        }
        isErased=1;
        //send the len to PC program to tell him flash erased over
        send(SOCK_FW, (uint8*)&fLen, (uint16)4);
        printf(">");
				update_flag=1;
      }
      else
      {
				int fmc_state=-1;				
        recv(SOCK_FW, buf, len);
        printf(".");
        for(i=0;i<len;i+=4)
        {
          //FLASH_ProgramWord(flashDest,*(uint32*)((uint32)buf + i));//将接收到的固件程序双字节写进Flash
					fmc_state=fmc_word_program(flashDest,*(uint32*)((uint32)buf + i));
          flashDest+=4;
        }
        rxLen+=len;
        //echo to PC program
        send(SOCK_FW, (uint8*)&len, (uint16)4);
        
        if(rxLen==fLen)
        {
          disconnect(SOCK_FW);   //断开连接
          //save to flash
					ConfigMsg.fw_len = fLen;
          ConfigMsg.state=NEW_APP_IN_BACK;// 改变状态
					ConfigMsg.endmark=0xAA;
          write_config_to_flash();
          printf("\r\nOK.\r\n");  
					update_flag=2;					
          //jump boot
          for(i=0;i<1000;i++);//wait for a while
          reboot();//引导设备运行app程序
					
        }
      }
    }
    break;
  case SOCK_CLOSE_WAIT:
    //disconnect(SOCK_FW);
    break;
  case SOCK_CLOSED:
    rxLen=0;//初始化参数
    nPage=0;
    nErasedPage=0;
    fLen=0;
    isErased=0;
    close(SOCK_FW);  
    socket(SOCK_FW,Sn_MR_TCP,TCP_RE_PORT,Sn_MR_ND);//建立TCP服务器
    break;
  case SOCK_INIT:
    listen(SOCK_FW);
    break;
  }
}
