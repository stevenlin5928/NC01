#include <stdio.h>
#include "gd32f10x.h"
#include "max7219.h"


//PA15_SPI3_CS 
//PB3_SPI3_SCK [2]
//PB5_SPI3_MOSI [2]

#define Max7219_pinDIN       GPIO_PIN_5   //PB5_SPI3_MOSI
#define Max7219_pinDIN_PORT  GPIOB   //PB5_SPI3_MOSI
#define Max7219_pinCS        GPIO_PIN_15  //PA15_SPI3_CS
#define Max7219_pinCS_PORT    GPIOA  //GPIOA
#define Max7219_pinCLK       GPIO_PIN_3   //PB3_SPI3_SCK
#define Max7219_pinCLK_PORT  GPIOB  //GPIOB
//PA12_DISPLAY_EN
#define Max7219_pinPower    GPIO_PIN_12//PA12_DISPLAY_EN
#define Max7219_pinPower_PORT    GPIOA//PA12_DISPLAY_EN


void Max7219_GPIOConfig(void)
{		
	/* enable the key clock */
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	/* configure beep GPIO port */ 
	gpio_init(Max7219_pinDIN_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,Max7219_pinDIN);
	gpio_init(Max7219_pinCS_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,Max7219_pinCS);
	gpio_init(Max7219_pinCLK_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,Max7219_pinCLK);
	//gpio_init(Max7219_pinPower_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,Max7219_pinPower);

	gpio_bit_set(Max7219_pinCS_PORT,Max7219_pinCS);  //cs HIGH
  gpio_bit_reset(Max7219_pinDIN_PORT,Max7219_pinDIN);	  
  gpio_bit_reset(Max7219_pinCLK_PORT,Max7219_pinCLK);
	//gpio_bit_set(Max7219_pinPower_PORT,Max7219_pinPower);
	return;
}

void Max7219_power_init()
{
	rcu_periph_clock_enable(RCU_GPIOA);
	gpio_init(Max7219_pinPower_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,Max7219_pinPower);
	gpio_bit_set(Max7219_pinPower_PORT,Max7219_pinPower);
	return;
}
void Max7219_power_on_off(int on)
{
	if(on>0)
		gpio_bit_set(Max7219_pinPower_PORT, Max7219_pinPower);
	else
		gpio_bit_reset(Max7219_pinPower_PORT, Max7219_pinPower);
}

void Max7219_pinDIN_HIGH(void)
{
	gpio_bit_set(Max7219_pinDIN_PORT, Max7219_pinDIN);	 
}

void Max7219_pinDIN_LOW(void)
{
	gpio_bit_reset(Max7219_pinDIN_PORT, Max7219_pinDIN);	 
}

void Max7219_pinCS_HIGH(void)
{
	gpio_bit_set(Max7219_pinCS_PORT, Max7219_pinCS);	 
}

void Max7219_pinCS_LOW(void)
{
	gpio_bit_reset(Max7219_pinCS_PORT, Max7219_pinCS);	 
}

void Max7219_pinCLK_HIGH(void)
{
	gpio_bit_set(Max7219_pinCLK_PORT, Max7219_pinCLK);	 
}

void Max7219_pinCLK_LOW(void)
{
	gpio_bit_reset(Max7219_pinCLK_PORT, Max7219_pinCLK);	 
}


//功能：向MAX7219写入字节
//入口参数：DATA 
//出口参数：无
//说明：
void Write_Max7219_byte(uint8_t DATA)         
{
	uint8_t i;    
	Max7219_pinCS_LOW();		
	for(i=8;i>=1;i--)
	{		  
		Max7219_pinCLK_LOW();
		if(DATA&0x80) //high bit
		{
			Max7219_pinDIN_HIGH();
		}
		else // LOW bit
		{
			Max7219_pinDIN_LOW();
		}
		delay_us(2);		
		Max7219_pinCLK_HIGH();
		delay_us(2);
		DATA=DATA<<1;
	}                                 
}

//功能：向MAX7219写入数据
//入口参数：address、dat
//出口参数：无
//说明：
void Write_Max7219(uint8_t address,uint8_t dat)
{ 
   Max7219_pinCS_LOW();
	 delay_us(1);		
	 Write_Max7219_byte(address);           //写入地址，即数码管编号
   Write_Max7219_byte(dat);               //写入数据，即数码管显示数字 
	 delay_us(1);		
	 Max7219_pinCS_HIGH();                        
}

void MAX7219_Clear (void) 
{ 
	uint8_t i; 
	for (i=1; i < 9; i++) 
		Write_Max7219(i, 0x00); // 清除所有位的显示 
} 

//向第一个MAX7219写入数据
void write_MAX7219_1(uint8_t address,uint8_t dat)
{
	Max7219_pinCS_LOW();
	delay_us(1);	
	Write_Max7219_byte(address);
	Write_Max7219_byte(dat);
	delay_us(1);	
	Max7219_pinCS_HIGH();
}

void Init_MAX7219(void)
{
	Max7219_GPIOConfig();
	delay_us(10);

	Write_Max7219(0x09, 0xff); //译码方式：BCD码
	delay_us(10);
	Write_Max7219(0x0a, 0x03); //亮度    	
	delay_us(10);
	Write_Max7219(0x0b, 0x07); //扫描界限；8个数码管显示    
	delay_us(10);
	Write_Max7219(0x0c, 0x01); //掉电模式：0，普通模式：1   
	delay_us(1);
	Write_Max7219(0x0f, 0x01); //显示测试：1；测试结束，正常显示：0   
	//
	delay_ms(800);
	Write_Max7219(0x0f, 0x00); 
	for (int i=1;i<9;i++)
	{
		Write_Max7219(i,0xa);
	}
	return;
}

void MAX7219_display_test(void)
{
	Write_Max7219(1,8);
	Write_Max7219(2,7);
	Write_Max7219(3,6);
	Write_Max7219(4,5); 
	Write_Max7219(5,4);
	Write_Max7219(6,3);
	Write_Max7219(7,2);
	Write_Max7219(8,1);
	return;	 
}

