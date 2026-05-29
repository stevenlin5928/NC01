#include "Board.h"
#define BARCODE_POWER_PORT  GPIOA//PA0_BAR_EN
#define BARCODE_POWER_PIN  GPIO_PIN_0//PA0_BAR_EN

#define BARCODE_TRIG_PORT  GPIOA//PA1_BAR_TRIG
#define BARCODE_TRIG_PIN  GPIO_PIN_1//PA1_BAR_TRIG

sio_byte_buf_type rs232_rx_barcode;

void barcode_init()
{
	rcu_periph_clock_enable(RCU_GPIOA);
	/* configure BARCODE POWER GPIO port */ 
	gpio_init(BARCODE_POWER_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,BARCODE_POWER_PIN);
	gpio_bit_set(BARCODE_POWER_PORT, BARCODE_POWER_PIN);
	gpio_init(BARCODE_TRIG_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,BARCODE_TRIG_PIN);
	gpio_bit_set(BARCODE_TRIG_PORT, BARCODE_TRIG_PIN);
	return;
}

void barcode_power_onoff(uint8_t onoff)
{
	bit_status status= gpio_input_bit_get(BARCODE_POWER_PORT, BARCODE_POWER_PIN);
//	if(onoff==status)
//		return ;
	if(onoff==1)
		gpio_bit_set(BARCODE_POWER_PORT, BARCODE_POWER_PIN);
	else
		gpio_bit_reset(BARCODE_POWER_PORT, BARCODE_POWER_PIN);
	return ;
}

void barcode_trigger(){
	//add code for trigger here
	gpio_bit_set(BARCODE_TRIG_PORT, BARCODE_TRIG_PIN);
	delay_1ms(50);
	gpio_bit_reset(BARCODE_TRIG_PORT, BARCODE_TRIG_PIN);
	delay_1ms(300);
}

void barcode_trigger_pressed(){
	//add code for trigger here
//	gpio_bit_set(BARCODE_TRIG_PORT, BARCODE_TRIG_PIN);
//	delay_1ms(100);
	gpio_bit_reset(BARCODE_TRIG_PORT, BARCODE_TRIG_PIN);

}

void barcode_trigger_release(){
	//add code for trigger here
	gpio_bit_set(BARCODE_TRIG_PORT, BARCODE_TRIG_PIN);

}

void barcode_cmd_trigger()
{
	//57 00 00 03 03 00 00 00 00 00 00 68 60 50 41
	uint8_t cmd_trigger[]={0x57,0x00,0x00,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x68,0x60,0x50,0x41};
	myputstr(cmd_trigger,sizeof(cmd_trigger));
	return;
}

//debug
void USART0_IRQHandler(void)
{
  uint32_t inbyte;
	//printf("USART0");
  /* RXNE handler */
	if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE))
  {
    usart_interrupt_flag_clear(USART0,USART_INT_FLAG_RBNE);
		inbyte =usart_data_receive(USART0);//USART_ReceiveData(USART0);
		//_Debug2(0,"%02X",inbyte);
//		if(ConfigMsg.data2RS232!=ROUTE_RS232)
//			return;
    if ( rs232_rx_barcode.len  >= (SIO_BYTE_BUF_MAX - 1) ) 
    {
      rs232_rx_barcode.len = 0;
			
			return;
    }
		if((rs232_rx_barcode.len==0)&&(inbyte==0))
		{
			printf("Invalid 00\n");
			return;
		}
		sio_byte_buf_enqueue(&rs232_rx_barcode, inbyte);
    if((inbyte=='\r')||(rs232_rx_barcode.len>64))//\r\n
	  {
				//_Debug2(0,"RX:%x",inbyte);
				barcode_post_received_data(&rs232_rx_barcode.data[0],rs232_rx_barcode.len-1);
				rs232_rx_barcode.len=0;
    }
	
	}
	else if(usart_interrupt_flag_get(USART0,USART_INT_FLAG_RBNE_ORERR) == SET){ 
		//USART_ClearITPendingBit(USART2,USART_FLAG_ORE);
		usart_interrupt_flag_clear(USART0,USART_INT_FLAG_RBNE_ORERR);
		//printf("ORE");
  }
	else
	{
		static int print_counter=0;
//		if((print_counter%100)==0)
//				printf(".");
		print_counter++;
		//USART_ClearITPendingBit(USART0,USART_IT_RXNE);
		usart_interrupt_flag_clear(USART0,USART_INT_FLAG_RBNE_ORERR);
	}
	
}
