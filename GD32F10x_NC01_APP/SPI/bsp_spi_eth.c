#include "bsp_spi_eth.h"
#include "w5500_conf.h"

#define Dummy_Byte                    0xFF
/*******************************************************************************
* Function Name  : SPI_Eth_Init
* Description    : Initializes the peripherals used by the SPI FLASH driver.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_Eth_Init(void)
{
	spi_parameter_struct spi_init_struct;
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_SPI0);
	//rcu_periph_clock_enable(RCU_SPI1);
//	rcu_periph_clock_enable(RCU_AF);
//	gpio_pin_remap_config(GPIO_SPI0_REMAP, ENABLE);
  /* SPI0 GPIO config:SCK/PA5, MISO/PA6, MOSI/PA7 */
  gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7);
  gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_6);//GPIO_MODE_IN_FLOATING
  /* PB8 as NSS */
  gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
/* SPI0 parameter config */
  spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
  spi_init_struct.device_mode          = SPI_MASTER;
  spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
  spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
  spi_init_struct.nss                  = SPI_NSS_SOFT;
  spi_init_struct.prescale             = SPI_PSC_2;
  spi_init_struct.endian               = SPI_ENDIAN_MSB;
	spi_init(SPI0, &spi_init_struct);
	/* set crc polyomial */
	spi_crc_polynomial_set(SPI0,7);
	spi_enable(SPI0);
}

u8 SPI_Eth_ReadByte(void)
{
  return (SPI_Eth_SendByte(Dummy_Byte));
}

/*******************************************************************************
* Function Name  : SPI_Eth_SendByte
* Description    : Sends a byte through the SPI interface and return the byte
*                  received from the SPI bus.
* Input          : byte : byte to send.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
u8 SPI_Eth_SendByte(u8 byte)
{
	int timeout=4096;
//	  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
//		SPI_I2S_SendData(SPI1, byte);
//		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
//		return SPI_I2S_ReceiveData(SPI1);
		while(RESET == spi_i2s_flag_get(SPI0, I2S_FLAG_TBE))
    {
        if(timeout-- == 0)
            return 0xFF;        
    }
    spi_i2s_data_transmit(SPI0, byte);
		timeout=4096;
    while(RESET == spi_i2s_flag_get(SPI0, I2S_FLAG_RBNE))
    {
        if(timeout-- == 0)
            return 0xFF;        
    }
    return spi_i2s_data_receive(SPI0);	
}

