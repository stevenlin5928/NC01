/*
**************************************************************************************************
* @file    		w5500_conf.c
* @author  		WisIOE Software Team
* @version 		V1.1
* @date    		2017-08-01
* @brief  		配置MCU，移植W5500程序需要修改的文件，配置W5500的MAC和IP地址
**************************************************************************************************
* @company  		深圳炜世科技有限公司
* @website  		www.wisioe.com
* @forum        www.w5500.cn
* @qqGroup      383035001
**************************************************************************************************
*/
#include <stdio.h> 
#include <string.h>

#include "w5500_conf.h"
#include "bsp_spi_eth.h"
#include "utility.h"
#include "w5500.h"
#include "dhcp.h"

extern uint32  dhcp_time;
DHCP_Get DHCP_GET;
CONFIG_MSG  ConfigMsg,RecvMsg;																	/*配置结构体*/
EEPROM_MSG_STR EEPROM_MSG;															/*EEPROM存储信息结构体*/

/*定义MAC地址,如果多块W5500网络适配板在同一现场工作，请使用不同的MAC地址*/
uint8 mac[6]={0x00,0x08,0xdc,0x11,0x11,0x11};

/*定义默认IP信息*/
uint8 local_ip[4]={192,168,100,168};												/*定义W5500默认IP地址*/
uint8 subnet[4]={255,255,255,0};												/*定义W5500默认子网掩码*/
uint8 gateway[4]={192,168,100,1};													/*定义W5500默认网关*/
uint8 dns_server[4]={114,114,114,114};									/*定义W5500默认DNS*/

uint16 local_port=8080;	                       					/*定义本地端口*/

/*定义远端IP信息*/
uint8  remote_ip[4]={192,168,1,111};										/*远端IP地址*/
uint16 remote_port=8080;																/*远端端口号*/

/*IP配置方法选择，请自行选择*/
uint8	ip_from=IP_FROM_DEFINE;				

uint8 dhcp_ok=0;																				/*dhcp成功获取IP*/
uint32	ms=0;																						/*毫秒计数*/
uint32	dhcp_time= 0;																		/*DHCP运行计数*/
vuint8	ntptimer = 0;																				/*NPT秒计数*/

/**
*@brief		配置W5500的IP地址
*@param		无
*@return	无
*/
void set_w5500_ip(void)
{	
		
 /*复制定义的配置信息到配置结构体*/
	memcpy(ConfigMsg.mac, mac, 6);
	memcpy(ConfigMsg.lip,local_ip,4);
	memcpy(ConfigMsg.sub,subnet,4);
	memcpy(ConfigMsg.gw,gateway,4);
	memcpy(ConfigMsg.dns,dns_server,4);
	if(ip_from==IP_FROM_DEFINE)	
		printf(" 使用定义的IP信息配置W5500\r\n");
	
	/*使用EEPROM存储的IP参数*/	
	if(ip_from==IP_FROM_EEPROM)
	{
		/*从EEPROM中读取IP配置信息*/
		read_config_from_eeprom();		
		
		/*如果读取EEPROM中MAC信息,如果已配置，则可使用*/		
		if( *(EEPROM_MSG.mac)==0x00&& *(EEPROM_MSG.mac+1)==0x08&&*(EEPROM_MSG.mac+2)==0xdc)		
		{
			printf(" IP from EEPROM\r\n");
			/*复制EEPROM配置信息到配置的结构体变量*/
			memcpy(ConfigMsg.lip,EEPROM_MSG.lip, 4);				
			memcpy(ConfigMsg.sub,EEPROM_MSG.sub, 4);
			memcpy(ConfigMsg.gw, EEPROM_MSG.gw, 4);
		}
		else
		{
			printf(" EEPROM未配置,使用定义的IP信息配置W5500,并写入EEPROM\r\n");
			write_config_to_eeprom();	/*使用默认的IP信息，并初始化EEPROM中数据*/
		}			
	}

	/*使用DHCP获取IP参数，需调用DHCP子函数*/		
	if(ip_from==IP_FROM_DHCP)								
	{
		/*复制DHCP获取的配置信息到配置结构体*/
		if(dhcp_ok==1)
		{
			printf(" IP from DHCP\r\n");		 
			memcpy(ConfigMsg.lip,DHCP_GET.lip, 4);
			memcpy(ConfigMsg.sub,DHCP_GET.sub, 4);
			memcpy(ConfigMsg.gw,DHCP_GET.gw, 4);
			memcpy(ConfigMsg.dns,DHCP_GET.dns,4);
		}
		else
		{
			printf(" DHCP子程序未运行,或者不成功\r\n");
			printf(" 使用定义的IP信息配置W5500\r\n");
		}
	}
		
//	/*以下配置信息，根据需要选用*/	
//	ConfigMsg.sw_ver[0]=FW_VER_HIGH;
//	ConfigMsg.sw_ver[1]=FW_VER_LOW;	

	/*将IP配置信息写入W5500相应寄存器*/	
	setSUBR(ConfigMsg.sub);
	setGAR(ConfigMsg.gw);
	setSIPR(ConfigMsg.lip);
		//must delay for GD303
	delay_ms(5000);
	getSIPR (local_ip);			
	printf(" W5500 IP地址   : %d.%d.%d.%d\r\n", local_ip[0],local_ip[1],local_ip[2],local_ip[3]);
	getSUBR(subnet);
	printf(" W5500 子网掩码 : %d.%d.%d.%d\r\n", subnet[0],subnet[1],subnet[2],subnet[3]);
	getGAR(gateway);
	printf(" W5500 网关     : %d.%d.%d.%d\r\n", gateway[0],gateway[1],gateway[2],gateway[3]);
}

/**
*@brief		配置W5500的MAC地址
*@param		无
*@return	无
*/
void set_w5500_mac(void)
{
	memcpy(ConfigMsg.mac, mac, 6);
	setSHAR(ConfigMsg.mac);	/**/
	memcpy(DHCP_GET.mac, mac, 6);
}

/**
*@brief		配置W5500的GPIO接口
*@param		无
*@return	无
*/
void gpio_for_w5500_config(void)
{
/* enable the led clock */
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_GPIOA);
	SPI_Eth_Init();																		     /*初始化STM32 SPI接口*/

 //rcu_periph_clock_enable(RCU_AF);
  /*使用SPI1*/
#if  1//def STM32_SPI1
	/*定义RESET引脚*/
	/*选择要控制的GPIO引脚*/	
	/*设置引脚速率为50MHz*/			
	/*设置引脚模式为通用推挽输出*/		
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_9);
	gpio_bit_reset(GPIOB,GPIO_PIN_9);
	/*
	*定义INT引脚
	*/
	gpio_init(GPIOA, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ,GPIO_PIN_4);
#endif
  return;
}

/**
*@brief		W5500片选信号设置函数
*@param		val: 为“0”表示片选端口为低，为“1”表示片选端口为高
*@return	无
*/
void wiz_cs(uint8_t val)
{
//#ifdef  STM32_SPI1
		if (val == LOW) 
		{
			gpio_bit_reset(GPIOB, GPIO_PIN_8); 
			
		}
		else if (val == HIGH)
		{
			gpio_bit_set(GPIOB, GPIO_PIN_8); 
			
		}
//	#endif
//	#ifdef STM32_SPI2
//		if (val == LOW) 
//		{
//			gpio_bit_reset(GPIOB, GPIO_PIN_12); 
//		}
//		else if (val == HIGH)
//		{
//			gpio_bit_set(GPIOB, GPIO_PIN_12); 
//		}
//	#endif
	
}

/**
*@brief		设置W5500的片选端口SCSn为低
*@param		无
*@return	无
*/
void iinchip_csoff(void)
{
	wiz_cs(LOW);
}

/**
*@brief		设置W5500的片选端口SCSn为高
*@param		无
*@return	无
*/
void iinchip_cson(void)
{	
   wiz_cs(HIGH);
}

/**
*@brief		W5500复位设置函数
*@param		无
*@return	无
*/
void reset_w5500(void)
{
			gpio_bit_reset(GPIOB, GPIO_PIN_9);
			delay_ms(200);  
			gpio_bit_set(GPIOB, GPIO_PIN_9);
			delay_ms(600);	
}

/**
*@brief		STM32 SPI1读写8位数据
*@param		dat：写入的8位数据
*@return	无
*/
uint8  IINCHIP_SpiSendData(uint8 dat)
{
   return(SPI_Eth_SendByte(dat));
}

/**
*@brief		写入一个8位数据到W5500
*@param		addrbsb: 写入数据的地址
*@param   data：写入的8位数据
*@return	无
*/
void IINCHIP_WRITE( uint32 addrbsb,  uint8 data)
{
   iinchip_csoff();                              		
   IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);	
   IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
   IINCHIP_SpiSendData( (addrbsb & 0x000000F8) + 4);  
   IINCHIP_SpiSendData(data);                   
   iinchip_cson();                            
}

/**
*@brief		从W5500读出一个8位数据
*@param		addrbsb: 写入数据的地址
*@param   data：从写入的地址处读取到的8位数据
*@return	无
*/
uint8 IINCHIP_READ(uint32 addrbsb)
{
   uint8 data = 0;
   iinchip_csoff();                            
   IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
   IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
   IINCHIP_SpiSendData( (addrbsb & 0x000000F8))    ;
   data = IINCHIP_SpiSendData(0x00);            
   iinchip_cson();                               
   return data;    
}

/**
*@brief		向W5500写入len字节数据
*@param		addrbsb: 写入数据的地址
*@param   buf：写入字符串
*@param   len：字符串长度
*@return	len：返回字符串长度
*/
uint16 wiz_write_buf(uint32 addrbsb,uint8* buf,uint16 len)
{
   uint16 idx = 0;
   if(len == 0) printf("Unexpected2 length 0\r\n");
   iinchip_csoff();                               
   IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
   IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
   IINCHIP_SpiSendData( (addrbsb & 0x000000F8) + 4); 
   for(idx = 0; idx < len; idx++)
   {
     IINCHIP_SpiSendData(buf[idx]);
   }
   iinchip_cson();                           
   return len;  
}

/**
*@brief		从W5500读出len字节数据
*@param		addrbsb: 读取数据的地址
*@param 	buf：存放读取数据
*@param		len：字符串长度
*@return	len：返回字符串长度
*/
uint16 wiz_read_buf(uint32 addrbsb, uint8* buf,uint16 len)
{
  uint16 idx = 0;
  if(len == 0)
  {
    printf("Unexpected2 length 0\r\n");
  }
  iinchip_csoff();                                
  IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
  IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
  IINCHIP_SpiSendData( (addrbsb & 0x000000F8));    
  for(idx = 0; idx < len; idx++)                   
  {
    buf[idx] = IINCHIP_SpiSendData(0x00);
  }
  iinchip_cson();                                  
  return len;
}

/**
*@brief		写配置信息到EEPROM
*@param		无
*@return	无
*/
void write_config_to_eeprom(void)
{
//	uint16 dAddr=0;
//	ee_WriteBytes(ConfigMsg.mac,dAddr,(uint8)EEPROM_MSG_LEN);				
	delay_ms(10);																							
}

/**
*@brief		从EEPROM读配置信息
*@param		无
*@return	无
*/
void read_config_from_eeprom(void)
{
//	ee_ReadBytes(EEPROM_MSG.mac,0,EEPROM_MSG_LEN);
	delay_us(10);
}

/**
*@brief		STM32定时器2初始化
*@param		无
*@return	无
*/
void timer2_init(void)
{
//	TIM2_Configuration();																		/* TIM2 定时配置 */
//	TIM2_NVIC_Configuration();															/* 定时器的中断优先级 */
//	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);		/* TIM2 重新开时钟，开始计时 */
//	timer_enable(TIMER2);
}

/**
*@brief		dhcp用到的定时器初始化
*@param		无
*@return	无
*/
void dhcp_timer_init(void)
{
			timer2_init();																	
}

/**
*@brief		ntp用到的定时器初始化
*@param		无
*@return	无
*/
void ntp_timer_init(void)
{
			timer2_init();																	
}

/**
*@brief		定时器2中断函数
*@param		无
*@return	无
*/
void timer2_isr(void)
{
	ms++;	
  if(ms>=1000)
  {  
    ms=0;
    dhcp_time++;																					/*DHCP定时加1S*/
		#ifndef	__NTP_H__
		ntptimer++;																						/*NTP重试时间加1S*/
		#endif
		//LED1_TOGGLE;
  }

}

void _Get_UUID() {
#if 1
	#define STM32_UUID ((uint32_t *)0x1ffff7e8)
  uint32_t idPart1 = STM32_UUID[0];
  uint32_t idPart2 = STM32_UUID[1];
  uint32_t idPart3 = STM32_UUID[2];
  //do something with the overall 96 bits
  uint32_t temp, mix;
	
	// Set MAC Address
	temp = idPart1 % 13;
	
	mac[0] = (u8)(idPart3 >> temp);;
	mac[0] = mac[0] & 0xfe; // MAC ??????BYTE,?????
	if(mac[0] == 0) mac[0] = 0xfe;
	temp = idPart1 % 29;
	mac[1] = (u8)(idPart3 >> temp);
	
	mix = idPart1 ^ idPart2;
	mac[2] = (u8)(mix >> temp);
	temp = idPart1 % 21;
	mix = idPart1 ^ idPart3;
	mac[3] = (u8)(mix >> temp);
	temp = idPart1 % 27;
	mix = idPart3 ^ idPart2;
	mac[4] = (u8)(mix >> temp);
	temp = (temp*37) % 19;
	mac[5] = (u8)(idPart3 >> temp);
	
	
#else
#define STM32_UUID ((uint32_t *)0x1ffff7e8)
  uint32_t idPart1 = STM32_UUID[0];
  uint32_t idPart2 = STM32_UUID[1];
  uint32_t idPart3 = STM32_UUID[2];
  //do something with the overall 96 bits
  uint32_t temp, mix;
	
	// Set MAC Address
	temp = idPart1 & 0x0000ff00;
	
	mac[0] = (u8)(temp >> 8);
	mac[0] = mac[0] & 0xfe; // MAC ??????BYTE,?????
	if(mac[0] == 0) mac[0] = 0xfe;
	
	mac[1] = (u8)(idPart1 & 0x000000ff);
	
//	temp = idPart3;
//	Phy_Addr[2] = (u8)(temp >> 24);
//	temp = idPart3 & 0x00ff0000;
//	Phy_Addr[3] = (u8)(temp >> 16);
//	temp = idPart3 & 0x0000ff00;
//	Phy_Addr[4] = (u8)(temp >> 8);
//	Phy_Addr[5] = (u8)(idPart3 & 0x000000ff);
	
	mix = idPart3 ^ idPart2;
	mac[2] = (u8)(mix >> 24);
	temp = mix & 0x00ff0000;
	mac[3] = (u8)(temp >> 16);
	temp = mix & 0x0000ff00;
	mac[4] = (u8)(temp >> 8);
	mac[5] = (u8)(mix & 0x000000ff);
#endif/**/	
	printf("UUID:%08X:%08X:%08X, MAC:%02x:%02x:%02x:%02x:%02x:%02x\r\n", idPart1, idPart2, idPart3,
		mac[0], mac[1],mac[2],mac[3],mac[4],mac[5]);
}


void Get_UUID(uint8_t *mac,int len) {

#define STM32_UUID ((uint32_t *)0x1ffff7e8)
  uint32_t idPart1 = STM32_UUID[0];
  uint32_t idPart2 = STM32_UUID[1];
  uint32_t idPart3 = STM32_UUID[2];
  //do something with the overall 96 bits
  uint32_t temp, mix;
	
	// Set MAC Address
	temp = idPart1 % 13;
	
	mac[0] = (u8)(idPart3 >> temp);;
	mac[0] = mac[0] & 0xfe; // MAC ??????BYTE,?????
	if(mac[0] == 0) mac[0] = 0xfe;
	temp = idPart1 % 29;
	mac[1] = (u8)(idPart3 >> temp);
	
	mix = idPart1 ^ idPart2;
	mac[2] = (u8)(mix >> temp);
	temp = idPart1 % 21;
	mix = idPart1 ^ idPart3;
	mac[3] = (u8)(mix >> temp);
	temp = idPart1 % 27;
	mix = idPart3 ^ idPart2;
	mac[4] = (u8)(mix >> temp);
	temp = (temp*37) % 19;
	mac[5] = (u8)(idPart3 >> temp);
	return;
}


/**
*@brief		STM32系统软复位函数
*@param		无
*@return	无
*/
void reboot(void)
{
#if 1
	SCB->AIRCR = (0x5FA << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQ_Msk;
    //
	while (1);	
#else
  pFunction Jump_To_Application;
  uint32 JumpAddress;
  printf(" System restart in progress...\r\n");
  JumpAddress = *(vu32*) (ApplicationAddress + 0x00000004);
	 __set_MSP(*(__IO uint32_t *)ApplicationAddress);
  Jump_To_Application = (pFunction) JumpAddress;
  Jump_To_Application();
#endif/**/
}

void System_Reset(void) {
    
    SCB->AIRCR = (0x5FA << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQ_Msk;
 
    //
    while (1);
}


/**
*@brief		获取配置信息
*@param		无
*@return	无
*/
void get_config(void)
{
  uint16 i;
  uint32 addr = ConfigAddr;
  for (i =0; i < sizeof(ConfigMsg); i++) 
  {
    *(ConfigMsg.mac+i) = *(vu8*)addr;
      addr += 1;
  }
	_Get_UUID();
  //set default mac
  memcpy(ConfigMsg.mac, mac,6);
	addr = verAddr;
  for (i =0; i < 4; i++) 
  {
    *(ConfigMsg.op+i) = *(vu8*)addr;
      addr += 1;
  }
}

void get_ver(void)
{
  uint16 i;
  uint32 addr = verAddr;
  for (i =0; i < 4; i++) 
  {
    *(ConfigMsg.op+i) = *(vu8*)addr;
      addr += 1;
  }
}

/**
*@brief		复制定义的配置信息到配置结构体并写入Flah
*@param		无
*@return	无
*/
void set_default(void)
{
	_Get_UUID();
  //set default mac
  memcpy(ConfigMsg.mac, mac,6);
  memcpy(ConfigMsg.lip, local_ip, 4);
  memcpy(ConfigMsg.sub,  subnet, 4);
  memcpy(ConfigMsg.gw,  gateway, 4);
  memcpy(ConfigMsg.dns,dns_server,4);
//  ConfigMsg.sw_ver[1]=FW_VER_LOW;
//  ConfigMsg.sw_ver[0]=FW_VER_HIGH;
  ConfigMsg.debug = 1;
  ConfigMsg.op[0] = '1';
	ConfigMsg.op[1] = '0';
	ConfigMsg.op[2] = '0';
	ConfigMsg.state=0;
	ConfigMsg.reset_D_latch_time=2000;
	ConfigMsg.tmp_ctrl_floor=200;
	ConfigMsg.tmp_ctrl_ceil=240;
	ConfigMsg.humidity_ctrl_floor=230;
	ConfigMsg.humidity_ctrl_ceil=240;
	ConfigMsg.tmp_warn_floor=190;
	ConfigMsg.tmp_warn_ceil=250;
	ConfigMsg.humidity_warn_floor=200;
	ConfigMsg.humidity_warn_ceil=270;
	ConfigMsg.open_beep_delay_time=3000;
	
	ConfigMsg.barcode_trigger_pressed_time=3000;
	ConfigMsg.endmark=0xAA;

  write_config_to_flash();
	write_ver_to_flash();
}

/**
*@brief		将定义的配置信息到配置结构体写入Flah
*@param		无
*@return	无
*/
void write_config_to_flash(void)//flash
{
  uint8 i, j;
  uint32 dAddr, data;
  int fmc_state=-1;
  // Unlock the Flash Program Erase controller
  //FLASH_Unlock();
	fmc_unlock();
  // Clear All pending flags
  //FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
  fmc_flag_clear(FMC_FLAG_BANK0_END);
  fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
  fmc_flag_clear(FMC_FLAG_BANK0_PGERR);	
  // Erase the FLASH pages 
  //FLASH_ErasePage(ConfigAddr);
  fmc_state = fmc_page_erase(ConfigAddr);//BANK0_WRITE_START_ADDR + (FLASH_PAGE_SIZE * erase_counter
  dAddr = ConfigAddr;

  for (i = 0, j = 0; i < (uint8)(sizeof(ConfigMsg)/4);i++) 
  {
    data = *(u32 *)(ConfigMsg.mac+j);
    //FLASH_ProgramWord(dAddr, data);
		fmc_state=fmc_word_program(dAddr,data);
		if(fmc_state!=FMC_READY)
		{
			printf("write_config_to_flash:%d\n",fmc_state);
		}
    dAddr += 4;
    j +=4;
  }
  fmc_lock();
}


/**
*@brief		版本号信息写入
*@param		无
*@return	无
*/
void write_ver_to_flash(void)//flash
{
  uint32 dAddr, data;
  int fmc_state=0;
  // Unlock the Flash Program Erase controller
  //FLASH_Unlock();
  fmc_unlock();
  // Clear All pending flags
  //FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

  // Erase the FLASH pages 
  //FLASH_ErasePage(verAddr);
	fmc_flag_clear(FMC_FLAG_BANK0_END);
  fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
  fmc_flag_clear(FMC_FLAG_BANK0_PGERR);	
	fmc_state = fmc_page_erase(verAddr);
  dAddr = verAddr;
	data = (ConfigMsg.sw_ver[0]<<8)|(ConfigMsg.sw_ver[1]&0xff);
	//FLASH_ProgramWord(dAddr, data);
	fmc_state=fmc_word_program(dAddr,data);
	fmc_flag_clear(FMC_FLAG_BANK0_END);
  fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
  fmc_flag_clear(FMC_FLAG_BANK0_PGERR);	
	printf("--%x:fmc_state:%d\n",data,fmc_state);
  //FLASH_Lock();
	fmc_lock();
}



/**
*@brief		将IP配置信息写入W5500相应寄存器和初始化8个Socket的发送接收缓存大小
*@param		无
*@return	无
*/
void set_network(void)
{
  //uint8 ip[4]; 
  setSHAR(ConfigMsg.mac);
  setSUBR(ConfigMsg.sub);
  setGAR(ConfigMsg.gw);
  setSIPR(ConfigMsg.lip);

  //Init. TX & RX Memory size
  socket_buf_init(txsize, rxsize); 
  setRTR(6000);
  setRCR(3);
	//must delay for GD303
	//printf(" ConfigMsg.lip:%d\n", ConfigMsg.lip[0]);

	getSIPR (local_ip);	
	
	if(ConfigMsg.debug>0)	
		printf(" LFRM IP   : %d.%d.%d.%d\r\n", local_ip[0],local_ip[1],local_ip[2],local_ip[3]);
	getSUBR(subnet);
	if(ConfigMsg.debug>0)
		printf(" C2C Sub  : %d.%d.%d.%d\r\n", subnet[0],subnet[1],subnet[2],subnet[3]);
	getGAR(gateway);
	if(ConfigMsg.debug>0)
		printf(" C2C GW   : %d.%d.%d.%d\r\n", gateway[0],gateway[1],gateway[2],gateway[3]);
}

/**
*@brief		跳转到APP执行
*@param		无
*@return	无
*/
void reboot_app(void)
{
  pFunction Jump_To_Application;
  uint32 JumpAddress;
  JumpAddress = *(vu32*) (ApplicationAddress + 0x00000004);
  Jump_To_Application = (pFunction) JumpAddress;
  //initialize user application's stack pointer
  __set_MSP(*(__IO uint32_t *)ApplicationAddress);
  Jump_To_Application();
}

/**
*@brief		将更新的APP写入到App执行区间
*@param		无
*@return	无
*/
void copy_app(uint32 fw_len)
{
	vu32 nErasedPage;
	uint32 nPage;
	u32 i;
	int fmc_state=-1;
  if(fw_len>0)
  {
     nPage=FLASH_PagesMask(fw_len);
//    //unlock flash
//    FLASH_Unlock();
		fmc_unlock();
//    // Clear All pending flags
//    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
		/* clear all pending flags */
    fmc_flag_clear(FMC_FLAG_BANK0_END);
    fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
    fmc_flag_clear(FMC_FLAG_BANK0_PGERR);
//    //erase required pages
    for(nErasedPage=0; nErasedPage<nPage; nErasedPage++)
    {
      //FLASH_ErasePage(ApplicationAddress + 0x400*nErasedPage);
			fmc_state=fmc_page_erase(ApplicationAddress + 0x400*nErasedPage);
			if(fmc_state!=FMC_READY)
			{
				printf("copy_app:%d\n",fmc_state);
			}
      fmc_flag_clear(FMC_FLAG_BANK0_END);
      fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
      fmc_flag_clear(FMC_FLAG_BANK0_PGERR);
    }
    //copy app from backup address to real address
    for(i=0; i<fw_len; i+=4)
    {
      //FLASH_ProgramWord(ApplicationAddress+i, *(uint32*)(AppBackupAddress+i));
			fmc_state=fmc_word_program(ApplicationAddress+i, *(uint32*)(AppBackupAddress+i));
    }
    //lock flash again
    fmc_lock();
  }
}

uint32 FLASH_PagesMask(vu32 Size)
{
  u32 pagenumber = 0x0;
  u32 size = Size;

  if((size % 0x400) != 0)
  {
    pagenumber = (size / 0x400) + 1;
  }
  else
  {
    pagenumber = size / 0x400;
  }
  return pagenumber;
}
