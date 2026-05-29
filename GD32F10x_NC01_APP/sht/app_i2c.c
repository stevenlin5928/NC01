
#include "app_i2c.h"
#define I2C0_OWN_ADDRESS7      0x72
#define I2C0_SLAVE_ADDRESS7    (0x80)

//PA11_SHT21_EN
#define SHT21_POWER_PORT GPIOA
#define SHT21_POWER_PIN  GPIO_PIN_11

#define SHT_I2C_BUS  I2C0

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

void I2C0_config (void)
{
	/* enable GPIOB clock */
	rcu_periph_clock_enable(RCU_GPIOB);
  gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6|GPIO_PIN_7);	
	GPIO_BC(GPIOB) |= GPIO_PIN_6 | GPIO_PIN_7;

    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    GPIO_BOP(GPIOB) |= GPIO_PIN_6;
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    GPIO_BOP(GPIOB) |= GPIO_PIN_7;

//	gpio_init(SHT21_POWER_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,SHT21_POWER_PIN);
//	gpio_bit_reset(SHT21_POWER_PORT,SHT21_POWER_PIN);	
//	delay_ms(10);	
//	gpio_bit_set(SHT21_POWER_PORT,SHT21_POWER_PIN);	
//	delay_ms(30);	
	/* enable I2C0 clock */
	rcu_periph_clock_enable(RCU_I2C0);

	/* I2C0 and I2C1 GPIO ports */
	/* connect PB6 to I2C0_SCL */
	/* connect PB7 to I2C0_SDA */
	gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_6 | GPIO_PIN_7);

	/* I2C clock configure */
	i2c_clock_config(SHT_I2C_BUS, 50000, I2C_DTCY_2);
	/* I2C address configure, I2C not smbus*/
	i2c_mode_addr_config(SHT_I2C_BUS, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2C0_SLAVE_ADDRESS7);
	/* enable I2C0 */
	i2c_enable(SHT_I2C_BUS);
	/* enable acknowledge */
	i2c_ack_config(SHT_I2C_BUS, I2C_ACK_ENABLE);
	
	return;
}

//void i2c_bus_reset() {
//	i2c_disable(SHT_I2C_BUS);
//	rcu_periph_clock_disable(RCU_I2C0);
//	gpio_bit_reset(SHT21_POWER_PORT,SHT21_POWER_PIN);
//	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6 | GPIO_PIN_7);
//    
//	// 2. ????SCL?SDA??STOP??
//	GPIO_BOP(GPIOB) = GPIO_PIN_6;  // SCL=1
//	delay_ms(1);
//	GPIO_BOP(GPIOB) = GPIO_PIN_7;  // SDA=1
//	delay_ms(1);
//	gpio_bit_set(SHT21_POWER_PORT,SHT21_POWER_PIN);
//		/* enable GPIOB clock */
//	rcu_periph_clock_enable(RCU_GPIOB);
//	/* enable I2C0 clock */
//	rcu_periph_clock_enable(RCU_I2C0);
//	// 3. ??I²C????
//	gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ,  GPIO_PIN_6 | GPIO_PIN_7);	/* enable I2C0 clock */

//	
//	/* I2C0 and I2C1 GPIO ports */
//	/* connect PB6 to I2C0_SCL */
//	/* connect PB7 to I2C0_SDA */
//	gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_6 | GPIO_PIN_7);

//	/* I2C clock configure */
//	i2c_clock_config(SHT_I2C_BUS, 100000, I2C_DTCY_2);
//	/* I2C address configure, I2C not smbus*/
//	i2c_mode_addr_config(SHT_I2C_BUS, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2C0_SLAVE_ADDRESS7);
//	/* enable I2C0 */
//	i2c_enable(SHT_I2C_BUS);
//		/* enable acknowledge */
//	i2c_ack_config(SHT_I2C_BUS, I2C_ACK_ENABLE);
//	return ;
//}
void i2c_bus_reset() {
    uint8_t i = 0;
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6); /* SCL output clock signal */
    for(i = 0; i < 10; i++){
        gpio_bit_reset(GPIOB, GPIO_PIN_6);
        delay_us(2);
        gpio_bit_set(GPIOB, GPIO_PIN_6);
        delay_us(2);
    }
    /* reset I2C */
    i2c_software_reset_config(I2C0, I2C_SRESET_RESET);
    i2c_software_reset_config(I2C0, I2C_SRESET_SET);
    gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_6 | GPIO_PIN_7);
}
 
void check_bus_status(void) {
	int time_out=4;
	while(i2c_flag_get(I2C0,I2C_FLAG_I2CBSY)) {
		if(--time_out == 0){
			i2c_bus_reset();
		}
	}
}


int8_t  I2C0_Register_Read(uint8_t* B_buffer, uint8_t read_address,uint16_t number_of_byte)
{
	int ticks = I2C_TIMEOUT;	
	/* wait until I2C bus is idle */
	while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY)&& ticks)
	{
		ticks--;
	}
	if (ticks == 0) goto _err;
	ticks = I2C_TIMEOUT;
	if(2 == number_of_byte){
		i2c_ackpos_config(I2C0,I2C_ACKPOS_NEXT);
	}
	//printf("%s ---1111---\n",__func__);
	/* send a start condition to I2C bus */
	i2c_start_on_bus(I2C0);
    
	/* wait until SBSEND bit is set */
	while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND)&& ticks)
	{
		ticks--;
	}
	if (ticks == 0) goto _err;
	ticks = I2C_TIMEOUT;
	//printf("%s ---2222---\n",__func__);   
	/* send slave address to I2C bus */
	i2c_master_addressing(I2C0, SHT21_ADDR, I2C_TRANSMITTER);
    
	/* wait until ADDSEND bit is set */
	while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND)&& ticks)
	{
		ticks--;
	}
	if (ticks == 0) goto _err;;
	ticks = I2C_TIMEOUT;
	//printf("%s ---3333---\n",__func__);
	/* clear the ADDSEND bit */
	i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
    
	/* wait until the transmit data buffer is empty */
	while(SET != i2c_flag_get( I2C0 , I2C_FLAG_TBE)&& ticks)
	{
		ticks--;
	}
	if (ticks == 0) goto _err;
	ticks = I2C_TIMEOUT;
	//printf("%s ---4444---\n",__func__);
	/* enable I2C0*/
	i2c_enable(I2C0);
    
	/* send the EEPROM's internal address to write to */
	i2c_data_transmit(I2C0, read_address);  
    
	/* wait until BTC bit is set */
	while(!i2c_flag_get(I2C0, I2C_FLAG_BTC)&& ticks)
	{
		ticks--;
	}
	if (ticks == 0) goto _err;
	ticks = I2C_TIMEOUT;
	//printf("%s ---5555---\n",__func__);
	delay_ms(20);
    
	/* send a start condition to I2C bus */
	i2c_start_on_bus(I2C0);
    
	/* wait until SBSEND bit is set */
	while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND)&& ticks)
	{
		ticks--;
	}
	if (ticks == 0) goto _err;
	ticks = I2C_TIMEOUT;
	//printf("%s ---6666---\n",__func__);
	/* send slave address to I2C bus */
	i2c_master_addressing(I2C0, SHT21_ADDR, I2C_RECEIVER);

	if(number_of_byte < 3){
		/* disable acknowledge */
		i2c_ack_config(I2C0,I2C_ACK_DISABLE);
	}
    
	/* wait until ADDSEND bit is set */
	while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND)&& ticks)
	{
		ticks--;
	}
	if (ticks == 0) goto _err;
	ticks = I2C_TIMEOUT;
	//printf("%s ---7777--- %d\n",__func__,number_of_byte);
	/* clear the ADDSEND bit */
	i2c_flag_clear(I2C0,I2C_FLAG_ADDSEND);
    
	if(1 == number_of_byte){
		/* send a stop condition to I2C bus */
		i2c_stop_on_bus(I2C0);
	}
    
	/* while there is data to be read */
	while(number_of_byte){
		if(3 == number_of_byte){
			/* wait until BTC bit is set */
			ticks = I2C_TIMEOUT;
			while(!i2c_flag_get(I2C0, I2C_FLAG_BTC)&& ticks)
			{
				ticks--;
			}
			if (ticks == 0) goto _err;
			ticks = I2C_TIMEOUT;
	printf("%s ---8888---\n",__func__);
			/* disable acknowledge */
			i2c_ack_config(I2C0,I2C_ACK_DISABLE);
		}
		if(2 == number_of_byte){
			ticks = I2C_TIMEOUT*2;
			/* wait until BTC bit is set */
			while(!i2c_flag_get(I2C0, I2C_FLAG_BTC)&& ticks)
			{
				ticks--;
			}
			if (ticks == 0) goto _err;
			ticks = I2C_TIMEOUT;
	//printf("%s ---9999---\n",__func__);
			/* send a stop condition to I2C bus */
			i2c_stop_on_bus(I2C0);
		}
					
		delay_1ms(1);
		/* wait until the RBNE bit is set and clear it */
		if(i2c_flag_get(I2C0, I2C_FLAG_RBNE)){
			/* read a byte from the EEPROM */
			*B_buffer = i2c_data_receive(I2C0);
							
			/* point to the next location where the byte read will be saved */
			B_buffer++; 
							
			/* decrement the read bytes counter */
			number_of_byte--;
		} 
	}
    
	/* wait until the stop condition is finished */
	while((I2C_CTL0(I2C0)&0x0200)&& ticks)
	{
		ticks--;
	}
	if (ticks == 0) goto _err;
	ticks = I2C_TIMEOUT;
	//printf("%s ---AAAA---\n",__func__);
	/* enable acknowledge */
	i2c_ack_config(I2C0, I2C_ACK_ENABLE);

	i2c_ackpos_config(I2C0, I2C_ACKPOS_CURRENT);
	return 0;
_err:
	if (i2c_flag_get(I2C0, I2C_FLAG_I2CBSY)) 
	{
   i2c_flag_clear(I2C0, I2C_FLAG_BERR); // 
                // 
		printf("%s reset \n",__func__);
		delay_1ms(100);
		//i2c_bus_reset();  
		check_bus_status();
	}
	return -1;
}

void I2C0_Byte_Write(uint8_t* P_buffer, uint8_t write_address)
{
    uint8_t i;
    /* wait until I2C bus is idle */
    while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));

    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C0);

    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));

    /* send slave address to I2C bus */
    i2c_master_addressing(I2C0, SHT21_ADDR, I2C_TRANSMITTER);

    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));

    /* clear the ADDSEND bit */
    i2c_flag_clear(I2C0,I2C_FLAG_ADDSEND);

    /* wait until the transmit data buffer is empty */
    while(SET != i2c_flag_get(I2C0, I2C_FLAG_TBE));

    /* send the EEPROM's internal address to write to : only one byte address */
    i2c_data_transmit(I2C0, write_address);

    /* wait until BTC bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_BTC));
    
    for(i=0;i<2;i++)
    {

        /* send the byte to be written */
        i2c_data_transmit(I2C0, *(P_buffer+i)); 

        /* wait until BTC bit is set */
        while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
    
    }

    /* send a stop condition to I2C bus */
    i2c_stop_on_bus(I2C0);

    /* wait until the stop condition is finished */
    while(I2C_CTL0(I2C0)&0x0200);
}

