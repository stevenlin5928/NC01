//---------- Includes ----------------------------------------------------------
#include "SHT2x.h"
#include "utility.h" 



int32_t ReadSht20(unsigned char whatdo)
{
	uint16_t temp;
  unsigned char MSB,LSB;
  float Humidity,Temperature;

	IIC_Start();
	IIC_Send_Byte(0x80);
	if(IIC_Wait_Ack()==0)
	{
		IIC_Send_Byte(whatdo);
		if(IIC_Wait_Ack()==0)
		{
			do
			{
				delay_us(8);
				IIC_Start();	
				IIC_Send_Byte(0x81);
			}while(IIC_Wait_Ack()==1);

			MSB = IIC_Read_Byte(1);
			LSB = IIC_Read_Byte(1);
			IIC_Read_Byte(0); 
			IIC_Stop();

			LSB &= 0xfc;
    	temp = (MSB<<8)|(LSB);
			
			if (whatdo==((unsigned char)0xf5))
    		{        
        		Humidity =((float)temp*125)/65536-6.0;
						Humidity *=100;
						
        		return Humidity;
    		}                                                                                                  
    		else      
    		{        
        		Temperature = ((float)temp*175.72)/65536-46.85;
						Temperature *=100;
        		return Temperature; 
    		}
		}
	}
	return 0;
}

int32_t SHT21_ReadTemp(void)
{
	int32_t Temperature = ReadSht20(TRIG_T_MEASUREMENT_POLL);	   //????  
	return Temperature;
}

int32_t SHT21_ReadHumidity(void)
{
	int32_t Humidity = ReadSht20(TRIG_RH_MEASUREMENT_POLL);	       //????
	
	return Humidity;
}

