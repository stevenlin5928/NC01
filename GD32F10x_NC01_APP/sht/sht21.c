#include "gd32f10x.h"
#include "sht21.h"
#include "app_i2c.h"
//https://zhuanlan.zhihu.com/p/498874352
void SHT21_Init (void)
{
	I2C0_config();
	return;
}


int32_t SHT21_ReadTemp (void)
{
	uint8_t tmp8[3]={0,0,0};
	float temperatureC;
	if(I2C0_Register_Read(tmp8, SHT21_CMD_TEMPERATURE,2)==0)
	{
		uint16_t u16sT =(uint16_t) ((tmp8[0]<<8) | (tmp8[1] & 0xFC));
	
		temperatureC = -46.85 + 175.72*(float)u16sT/65536;
		//printf("%s u16sT:%02x 0x%02X,%.01f\n",__func__,u16sT,tmp8[2],temperatureC);
		temperatureC *=100;
		return temperatureC;	
	}
	return 0.0f;
}

int32_t SHT21_ReadHumidity (void)
{
	uint8_t tmp8[3]={0,0,0};
	float humidityRH=0.0;
	if(I2C0_Register_Read(tmp8, SHT21_CMD_HUMIDITY,2)==0)
	{
		//printf("%s 0x%x%02x   %02X,%f\n",__func__,tmp8[0],tmp8[1],tmp8[2]);		
		uint16_t u16sT =(uint16_t) ((tmp8[0]<<8) | (tmp8[1] & 0xFC));
		
		humidityRH = -6.0 + 125*(float)u16sT/65536;
		//printf("%s u16sT:%02x 0x%02X,%.01f\n",__func__,u16sT,tmp8[2],humidityRH);
		return humidityRH*100;		
	}

	return 0.0;	
}
