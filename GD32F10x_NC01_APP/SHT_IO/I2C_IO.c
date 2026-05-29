//==============================================================================
//    S E N S I R I O N   AG,  Laubisruetistr. 50, CH-8712 Staefa, Switzerland
//==============================================================================
// Project   :  SHT2x Sample Code (V1.2)
// File      :  I2C_HAL.c
// Author    :  MST
// Controller:  NEC V850/SG3 (uPD70F3740)
// Compiler  :  IAR compiler for V850 (3.50A)
// Brief     :  I2C Hardware abstraction layer
//==============================================================================
//https://blog.csdn.net/weixin_52411606/article/details/122743576
//---------- Includes ----------------------------------------------------------
#include "I2C_IO.h"



void SDA_OUT(void)
{
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SDA_PIN); 
	return;                                                                                          
}


unsigned char SDA_IN(void)
{
  SDA_INPUT();
  return gpio_input_bit_get(GPIOB,SDA_PIN);
}

void sht2x_power_init()
{
	gpio_init(SHT21_POWER_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,SHT21_POWER_PIN);
	gpio_bit_set(SHT21_POWER_PORT,SHT21_POWER_PIN);	
	return;
}


void sht2x_power_on_off(int on)
{
	if(on>0)
		gpio_bit_set(SHT21_POWER_PORT,SHT21_POWER_PIN);
	else
		gpio_bit_reset(SHT21_POWER_PORT,SHT21_POWER_PIN);
	return;
}
//==============================================================================
void SHT21_Init ()
//==============================================================================
{/* enable GPIOB clock */
	rcu_periph_clock_enable(RCU_GPIOB);
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SDA_PIN); 
    // SDA????????
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SCL_PIN);  	
 // SCL???????

    // ?????????:ml-citation{ref="4,5" data="citationList"}
	
	SDA_OUTPUT();
	SCL_OUTPUT();
 
	SDA_H();
	SCL_L();

//	SDA_INPUT();
//	SCL_INPUT();
	return;
}

void IIC_Start(void)
{
  SDA_OUT();
  SDA_H();//SHT20_SDA = 1;
  SCL_H();//SHT20_SCL = 1;
  delay_us(20);

  SDA_L();//SHT20_SDA = 0;
  delay_us(20);
  SCL_L();//SHT20_SCL = 0;
}

void IIC_Stop(void)
{
  SDA_OUT();
  SCL_L();//SHT20_SCL = 0;
	SDA_L();//SHT20_SDA = 0;
  delay_us(20);
  SCL_H();//SHT20_SCL = 1;
  SDA_H();//SHT20_SDA = 1;
  delay_us(20);
}
uint8_t IIC_Wait_Ack(void)
{
  uint8_t ucErrTime = 0;
  SDA_IN();
  SDA_H();////SHT20_SDA = 1;
  delay_us(10);
  SCL_H();//SHT20_SCL = 1;
  delay_us(10);
  while(SHT20_SDA_READ())
  {
    ucErrTime++;
    if(ucErrTime > 250)
    {
      IIC_Stop();
      return 1;
    }
  }
  SCL_L();//SHT20_SCL = 0;  
  return 0;
}

void IIC_Ack(void)
{
  SCL_L();//SHT20_SCL = 0;
  SDA_OUTPUT();
  SDA_L();//SHT20_SDA = 0;
  delay_us(20);
  SCL_H();;//SHT20_SCL = 1;
  delay_us(20);
  SCL_L();//SHT20_SCL = 0;
}

void IIC_NAck(void)
{
  SCL_L();//SHT20_SCL = 0;
  SDA_OUTPUT();//SDA_OUT();
  SDA_H();//SHT20_SDA = 1;
  delay_us(20);
  SCL_H();//SHT20_SCL = 1;
  delay_us(20);
  SCL_L();//SHT20_SCL = 0;
}

void IIC_Send_Byte(uint8_t txd)
{
  uint8_t t;
  SDA_OUT();
  SCL_L();//SHT20_SCL = 0; 	
  for(t = 0; t < 8; t++)
  {
    if((txd & 0x80) >> 7)
      SDA_H();//SHT20_SDA = 1;
    else
      SDA_L();//SHT20_SDA = 0;
    txd <<= 1;
    delay_us(20);
    SCL_H();//SHT20_SCL = 1;
    delay_us(20);
    SCL_L();//SHT20_SCL = 0;
    delay_us(20);
  }
  delay_us(20);

}

uint16_t IIC_Read_Byte(uint8_t ack)
{
  uint8_t i;
  uint16_t receive = 0;
  SDA_IN();
  for(i = 0; i < 8; i++ )
  {
    SCL_L();//SHT20_SCL = 0;
    delay_us(20);
    SCL_H();//SHT20_SCL = 1;
    receive <<= 1;
    if(SHT20_SDA_READ())
      receive++;
    delay_us(20);
  }
  if (!ack)
    IIC_NAck();
  else
    IIC_Ack();
  return receive;
}



//void I2c_StartCondition ()
//{
//	SDA_OUTPUT();
//	SCL_OUTPUT();
//	SDA_H();
//	SCL_H();
//	delay_us(3);
////	SDA_INPUT();
////	if (gpio_input_bit_get(GPIOB,SDA_PIN)== 0)
////	{
////		printf("IIC start error L\n");
////		return;
////	}
////	SDA_OUTPUT();
////	//
////	SDA_L();
////  delay_us(10);
////	SDA_INPUT();
////	delay_us(10);
////	if (gpio_input_bit_get(GPIOB,SDA_PIN)== 1)
////	{
////		printf("IIC start error H\n");
////		return;
////	}
////	SDA_OUTPUT();
//	SDA_L();
//	delay_us(2);
//	SCL_L();
//	delay_us(10);
//	//printf("IIC start OK\n");
//	return;
//}


////==============================================================================
//void I2c_StopCondition ()
////==============================================================================
//{
//	SDA_OUTPUT();
//	SCL_OUTPUT();
//	SDA_L();
//	SCL_L();
//	delay_us(1);
//	SCL_H();

//	delay_us(10);
//	SDA_H();
//	delay_us(10);
////  SDA=LOW;
////  SCL=LOW;
////  SCL=HIGH;
////  DelayMicroSeconds(10);  // set-up time stop condition (t_SU;STO)
////  SDA=HIGH;
////  DelayMicroSeconds(10);
//}


//uint8_t I2C_Check_ACK(void) {
//    SDA_INPUT();           // ??SDA?????:ml-citation{ref="8" data="citationList"}
//    delay_us(5);
//    SCL_H();         // ??SCL,????????:ml-citation{ref="2" data="citationList"}
//    delay_us(2);
//    
//    if (gpio_input_bit_get(GPIOB,SDA_PIN)== 0) { // SDA  ACK
//        SCL_L();
//        return 0;
//    } else {                // SDA NACK
//        SCL_L();
//        return 1;
//    }
//}

//unsigned char I2C_waitAck(void)
//{
//  unsigned char t = 200;
//  SDA_OUTPUT();
//  SCL_L(); 
//  delay_us(2);
//  SDA_H();
//  delay_us(2);
//  //////////////////////////?????,IO????????????,??????
//  SDA_IN();
//  //////////////////////////
//  SCL_H();
//  delay_us(2);    
//  
//  while( SDA_IN() )
//  {
//    t --;
//    delay_us(2);

//    if(t==0)
//    {
//       SCL_L();
//        return 1;
//    }
//    delay_us(2);
//  }
//  delay_us(2);
//  SCL_L();
//  delay_us(2);
//  return 0;
//}

//void I2C_Send_ACK(uint8_t ack) {
//    SDA_OUTPUT();  
//		SCL_L();	
//		delay_us(5);
//    if (ack) {
//       SDA_H();   // SDA=1               // NACK
//    } else {
//       SDA_L();   // SDA=0;              // ACK
//    }
//    SCL_H(); // SCL=1 
//    delay_us(5);
//    SCL_L();   // SCL=0
//		delay_us(2);
//		SDA_H();
//		delay_us(2);
//}

////==============================================================================
//uint8_t I2c_WriteByte (uint8_t txByte)
////==============================================================================
//{
//  uint8_t mask,error=0;
//	SDA_OUTPUT();
//	SCL_OUTPUT();
//	SCL_L();    // SCL=0
//  for (mask=0x80; mask>0; mask>>=1)   //shift bit for masking (8 times)
//  { 
//		
//		if ((mask & txByte) == 0) 
//			SDA_L();   // SDA=0;//masking txByte, write bit to SDA-Line
//    else 
//			SDA_H();  // SDA=1
//    delay_us(1*5);             //data set-up time (t_SU;DAT)
//    SCL_H(); // SCL=1                     //generate clock pulse on SCL
//    delay_us(5*5);             //SCL high time (t_HIGH)
//		SCL_L();    // SCL=0
//		delay_us(2);             //data hold time(t_HD;DAT)	

//  }
//delay_us(20); 
//  return error;                       //return error code
//}

////==============================================================================
//uint8_t I2c_ReadByte (etI2cAck ack)
////==============================================================================
//{
//  uint8_t mask,rxByte=0;

//	SDA_INPUT();
//  for (mask=0x80; mask>0; mask>>=1)   //shift bit for masking (8 times)
//  { 
//		SCL_L();  // SCL=1                         //start clock on SCL-line
//    delay_us(2);             //data set-up time (t_SU;DAT)
//		SCL_H();   // SCL=0
//    delay_us(3);             //SCL high time (t_HIGH)
//		
//    if (gpio_input_bit_get(GPIOB,SDA_PIN)==1) rxByte=(rxByte | mask); //read bit
//    
//    delay_us(2);             //data hold time(t_HD;DAT)
//  }
//	//SDA_OUTPUT();
//	//SDA_L();
//	I2C_Send_ACK(!ack);//send acknowledge if necessary
////  delay_us(2);               //data set-up time (t_SU;DAT)
////  SCL_H();  // SCL=1                         //clk #9 for ack
////  delay_us(5);               //SCL high time (t_HIGH)
//  SCL_L();   // SCL=0
//	delay_us(2); 
//  
//  return rxByte;                      //return error code
//}




