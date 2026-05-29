#include "Board.h"



void init_rs485()
{
	//  GPIOB 
	rcu_periph_clock_enable(RCU_GPIOB);
//  USART??( USART3)
	rcu_periph_clock_enable(RCU_USART2);

	// PB10 TX 
	gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
	// PB11 RX 
	gpio_init(GPIOB, GPIO_MODE_IPD, GPIO_OSPEED_50MHZ, GPIO_PIN_11);

	//PB12_485_DIR
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);
	
}


//direction:
//				1-out
//				0-input
void set_rs485_direction(uint8_t direction)
{
	if(direction>0)
		gpio_bit_set(GPIOB,GPIO_PIN_12);
	else
		gpio_bit_reset(GPIOB,GPIO_PIN_12);
	return;
}

int rs485_putc(int ch)
{
    usart_data_transmit(USART2, (uint8_t)ch);
    while(RESET == usart_flag_get(USART2, USART_FLAG_TBE));

    return ch;
}

int rs485_outstr(uint8_t *buf,int len)
{
	int i=0;
	set_rs485_direction(1);
	delay_ms(5);
	while(i<len)
	{
		rs485_putc(buf[i]);
		i++;
	}
	delay_ms(3);
	set_rs485_direction(0);
	
	return 0;
}

void confit_rs485(uint8_t databits,uint8_t stop,int baudrate)
{
	usart_deinit(USART2);  // USART
	usart_baudrate_set(USART2, baudrate);  // ???
	usart_word_length_set(USART2, USART_WL_8BIT);  // ?????
	usart_stop_bit_set(USART2, USART_STB_1BIT);    // ???
	usart_parity_config(USART2, USART_PM_NONE);    // ???
	usart_hardware_flow_rts_config(USART2, USART_RTS_DISABLE);  // ??????
	usart_hardware_flow_cts_config(USART2, USART_CTS_DISABLE);
	usart_receive_config(USART2, USART_RECEIVE_ENABLE);  // ????
	usart_transmit_config(USART2, USART_TRANSMIT_ENABLE);  // ????
	usart_enable(USART2);  // ??USART
	
}

//debug
void USART2_IRQHandler(void)
{
extern uint8_t uart_test_port;//COM1 COM2
extern uint8_t uart_test_flag;//1:start test;0:stop test
  uint32_t inbyte;
	//printf("USART2");
  /* RXNE handler */
	if(RESET != usart_interrupt_flag_get(USART2, USART_INT_FLAG_RBNE))
  {
    usart_interrupt_flag_clear(USART2,USART_INT_FLAG_RBNE);
		inbyte =usart_data_receive(USART2);//USART_ReceiveData(USART0);
//		_Debug2(0,"%02X",inbyte);
		//for uart test
		if(uart_test_flag==1)
		{
			//printf("UR2\n");
			put_test_data_queue((uint8_t*)&inbyte,1,TYPE_COM2);
		}
	
	}
	else if(usart_interrupt_flag_get(USART2,USART_INT_FLAG_RBNE_ORERR) == SET){ 
		//USART_ClearITPendingBit(USART2,USART_FLAG_ORE);
		usart_interrupt_flag_clear(USART2,USART_INT_FLAG_RBNE_ORERR);
		//printf("ORE");
  }
	else
	{
		static int print_counter=0;
//		if((print_counter%100)==0)
//				printf(".");
		print_counter++;
		//USART_ClearITPendingBit(USART0,USART_IT_RXNE);
		usart_interrupt_flag_clear(USART2,USART_INT_FLAG_RBNE_ORERR);
	}
	
}
