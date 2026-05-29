#include "stdio.h"
#include "gp_io.h"
#define WDT_HW_RCU   RCU_GPIOB

/*
#define GP_INPUT_PORT GPIOC      //
#define GPI_IN_1  GPIO_PIN_3//PC3_INPUT1
#define GPI_IN_2  GPIO_PIN_2//PC2_INPUT2
#define GPI_IN_3  GPIO_PIN_1//PC1_INPUT3
#define GPI_IN_4  GPIO_PIN_0//PC0_INPUT4
//
#define GPI_OUT_1_PORT GPIOB  
#define GPI_OUT_1  GPIO_PIN_4 //PB4_OUTPUT1
#define GPI_OUT_2_PORT GPIOC 
#define GPI_OUT_2  GPIO_PIN_12//PC12_OUTPUT2
#define GPI_OUT_3_PORT GPIOC
#define GPI_OUT_3  GPIO_PIN_11//PC11_OUTPUT3
#define GPI_OUT_4_PORT GPIOC
#define GPI_OUT_4  	//PC10_OUTPUT4
*/
void gp_io_init(void)
{
	/* enable the key clock */
	rcu_periph_clock_enable(RCU_GPIOC);
	rcu_periph_clock_enable(RCU_GPIOB);
	/* configure beep GPIO port */ 
	gpio_init(GPI_IN_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ,GPI_IN_1);
	gpio_init(GPI_IN_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ,GPI_IN_2);
	gpio_init(GPI_IN_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ,GPI_IN_3);
	gpio_init(GPI_IN_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ,GPI_IN_4);
	
	gpio_init(GPI_IN_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ,GPI_IN_5);
	gpio_init(GPI_IN_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ,GPI_IN_6);
	gpio_init(GPI_IN_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ,GPI_IN_7);
	gpio_init(GPI_IN_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ,GPI_IN_8);
	#if 1
	//rcu_periph_clock_enable(RCU_AF);
	//exit_0
	exti_interrupt_enable(EXTI_0);	
	/* enable and set key EXTI interrupt to the lowest priority */
	nvic_irq_enable(EXTI0_IRQn, 2U, 2U);
	/* connect key EXT0 line to key GPIO pin */
	gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOC, GPIO_PIN_SOURCE_0);
	/* configure key EXTI line */
	exti_init(EXTI_0, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
	exti_interrupt_flag_clear(EXTI_0);
	
	//exit_1
	exti_interrupt_enable(EXTI_1);	
	/* enable and set key EXTI interrupt to the lowest priority */
	nvic_irq_enable(EXTI1_IRQn, 2U, 2U);
	/* connect key EXT0 line to key GPIO pin */
	gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOC, GPIO_PIN_SOURCE_1);
	/* configure key EXTI line */
	exti_init(EXTI_1, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
	exti_interrupt_flag_clear(EXTI_1);
	
	//exit_2
	exti_interrupt_enable(EXTI_2);	
	/* enable and set key EXTI interrupt to the lowest priority */
	nvic_irq_enable(EXTI2_IRQn, 2U, 2U);
	/* connect key EXT0 line to key GPIO pin */
	gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOC, GPIO_PIN_SOURCE_2);
	/* configure key EXTI line */
	exti_init(EXTI_2, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
	exti_interrupt_flag_clear(EXTI_2);
	
	//exit_3
	exti_interrupt_enable(EXTI_3);	
	/* enable and set key EXTI interrupt to the lowest priority */
	nvic_irq_enable(EXTI3_IRQn, 2U, 2U);
	/* connect key EXT0 line to key GPIO pin */
	gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOC, GPIO_PIN_SOURCE_3);

	/* configure key EXTI line */
	exti_init(EXTI_3, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
	exti_interrupt_flag_clear(EXTI_3);
	
	//exit_4
	exti_interrupt_enable(EXTI_4);	
	/* enable and set key EXTI interrupt to the lowest priority */
	nvic_irq_enable(EXTI4_IRQn, 2U, 2U);
	/* connect key EXT4 line to key GPIO pin */
	gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOC, GPIO_PIN_SOURCE_4);
	/* configure key EXTI line */
	exti_init(EXTI_4, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
	exti_interrupt_flag_clear(EXTI_4);
	
	//exit_5
	exti_interrupt_enable(EXTI_5);	
	/* enable and set key EXTI interrupt to the lowest priority */
	nvic_irq_enable(EXTI5_9_IRQn, 2U, 2U);
	/* connect key EXT5 line to key GPIO pin */
	gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOC, GPIO_PIN_SOURCE_5);
	/* configure key EXTI line */
	exti_init(EXTI_5, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
	exti_interrupt_flag_clear(EXTI_5);
	
	//exit_6
	exti_interrupt_enable(EXTI_6);	
	/* enable and set key EXTI interrupt to the lowest priority */
	nvic_irq_enable(EXTI5_9_IRQn, 2U, 2U);
	/* connect key EXT5 line to key GPIO pin */
	gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOC, GPIO_PIN_SOURCE_6);
	/* configure key EXTI line */
	exti_init(EXTI_6, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
	exti_interrupt_flag_clear(EXTI_6);
	
	//exit_7
	exti_interrupt_enable(EXTI_7);	
	/* enable and set key EXTI interrupt to the lowest priority */
	nvic_irq_enable(EXTI5_9_IRQn, 2U, 2U);
	/* connect key EXT5 line to key GPIO pin */
	gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOC, GPIO_PIN_SOURCE_7);
	/* configure key EXTI line */
	exti_init(EXTI_7, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
	exti_interrupt_flag_clear(EXTI_7);
	
#endif/**/
	
	//put put init
	gpio_init(GPO_OUT_1_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,GPO_OUT_1);
	gpio_init(GPO_OUT_2_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,GPO_OUT_2);
	gpio_init(GPO_OUT_3_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,GPO_OUT_3);
	gpio_init(GPO_OUT_4_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,GPO_OUT_4);
	gpio_init(GPO_OUT_5_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,GPO_OUT_5);
	gpio_init(GPO_OUT_6_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,GPO_OUT_6);
	gpio_init(GPO_OUT_7_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,GPO_OUT_7);
	gpio_init(GPO_OUT_8_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,GPO_OUT_8);
	
	gpio_bit_reset(GPO_OUT_1_PORT, GPO_OUT_1);
	gpio_bit_reset(GPO_OUT_2_PORT, GPO_OUT_2);
	gpio_bit_reset(GPO_OUT_3_PORT, GPO_OUT_3);
	gpio_bit_reset(GPO_OUT_4_PORT, GPO_OUT_4);
	
	gpio_bit_reset(GPO_OUT_5_PORT, GPO_OUT_5);
	gpio_bit_reset(GPO_OUT_6_PORT, GPO_OUT_6);
	gpio_bit_reset(GPO_OUT_7_PORT, GPO_OUT_7);
	gpio_bit_reset(GPO_OUT_8_PORT, GPO_OUT_8);
	return;
}

void gp_set_out(uint16_t out,uint8_t level)
{
	switch(out)
	{
		case 0:
			if(level>0)
				gpio_bit_set(GPO_OUT_1_PORT, GPO_OUT_1);
			else
				gpio_bit_reset(GPO_OUT_1_PORT, GPO_OUT_1);
			break;
		case 1:
			if(level>0)
				gpio_bit_set(GPO_OUT_2_PORT, GPO_OUT_2);
			else
				gpio_bit_reset(GPO_OUT_2_PORT, GPO_OUT_2);
			break;
		case 2:
			if(level>0)
				gpio_bit_set(GPO_OUT_3_PORT, GPO_OUT_3);
			else
				gpio_bit_reset(GPO_OUT_3_PORT, GPO_OUT_3);
			break;
		case 3:
			if(level>0)
				gpio_bit_set(GPO_OUT_4_PORT, GPO_OUT_4);
			else
				gpio_bit_reset(GPO_OUT_4_PORT, GPO_OUT_4);
			break;
		case 4:
			if(level>0)
				gpio_bit_set(GPO_OUT_5_PORT, GPO_OUT_5);
			else
				gpio_bit_reset(GPO_OUT_5_PORT, GPO_OUT_5);
			break;
		case 5:
			if(level>0)
				gpio_bit_set(GPO_OUT_6_PORT, GPO_OUT_6);
			else
				gpio_bit_reset(GPO_OUT_6_PORT, GPO_OUT_6);
			break;
		case 6:
			if(level>0)
				gpio_bit_set(GPO_OUT_7_PORT, GPO_OUT_7);
			else
				gpio_bit_reset(GPO_OUT_7_PORT, GPO_OUT_7);
			break;
		case 7:
			if(level>0)
				gpio_bit_set(GPO_OUT_8_PORT, GPO_OUT_8);
			else
				gpio_bit_reset(GPO_OUT_8_PORT, GPO_OUT_8);
			break;			
			
			
		default:
			printf("%s out:%d\n",__func__,out);
			break;
	}
	return ;
}

int8_t gp_get_out(uint16_t out,uint8_t *value)
{
	FlagStatus status;
	switch(out)
	{
		case 0:
			status=gpio_output_bit_get(GPO_OUT_1_PORT,GPO_OUT_1);
			break;
		case 1:
			status=gpio_output_bit_get(GPO_OUT_2_PORT,GPO_OUT_2);

			break;
		case 2:
			status=gpio_output_bit_get(GPO_OUT_3_PORT,GPO_OUT_3);
			break;
		case 3:
			status=gpio_output_bit_get(GPO_OUT_4_PORT,GPO_OUT_4);
			break;
		
		case 4:
			status=gpio_output_bit_get(GPO_OUT_5_PORT,GPO_OUT_5);
			break;
		case 5:
			status=gpio_output_bit_get(GPO_OUT_6_PORT,GPO_OUT_6);

			break;
		case 6:
			status=gpio_output_bit_get(GPO_OUT_7_PORT,GPO_OUT_7);
			break;
		case 7:
			status=gpio_output_bit_get(GPO_OUT_8_PORT,GPO_OUT_8);
			break;
		default:
			printf("%s out:%d\n",__func__,out);

			break;
	}

	return status;
}

int gp_get_in(uint8_t in,uint8_t *value)
{
	FlagStatus status;
	switch(in)
	{
		case 0:
			status=gpio_input_bit_get(GPI_IN_PORT,GPI_IN_1);
			break;
		case 1:
			status=gpio_input_bit_get(GPI_IN_PORT,GPI_IN_2);
			break;
		case 2:
			status=gpio_input_bit_get(GPI_IN_PORT,GPI_IN_3);
			break;
		case 3:
			status=gpio_input_bit_get(GPI_IN_PORT,GPI_IN_4);
			break;
		case 4:
			status=gpio_input_bit_get(GPI_IN_PORT,GPI_IN_5);
			break;
		case 5:
			status=gpio_input_bit_get(GPI_IN_PORT,GPI_IN_6);
			break;
		case 6:
			status=gpio_input_bit_get(GPI_IN_PORT,GPI_IN_7);
			break;
		case 7:
			status=gpio_input_bit_get(GPI_IN_PORT,GPI_IN_8);
			break;		
		default:
			printf("%s pin:%d\n",__func__,in);
		  return -1;
			break;
	}
	if(status==RESET)
	{
		*value=0;
		return 0;
	}
	else{
		*value=1;
		return 1;
	}
}



_gpio_event_list gpio_event_list={
	{0},	
	{0,0,0,0},
};
void gio_event_init()
{
	for(int i=0;i<MAX_GPIO_EVENT_LEN;i++)
	{
		gpio_event_list.Done[i]=0;
		gpio_event_list.gpio_event[i].type=-1;
		gpio_event_list.gpio_event[i].retport_type=-1;
		gpio_event_list.gpio_event[i].pin=-1;
		gpio_event_list.gpio_event[i].level=-1;
		gpio_event_list.gpio_event[i].msta=0;
	}
	
	return;
}

int gio_event_enque(_gpio_event *event)
{
	extern __IO uint32_t ms_counter;
	int i=0;
	
	for(i=0;i<MAX_GPIO_EVENT_LEN;i++)
	{
		if(gpio_event_list.Done[i]==1)
		{
			continue;
		}
		else
		{

			gpio_event_list.gpio_event[i]=*event;
			gpio_event_list.Done[i]=1;
			break;
		}
	}
	if(i>=MAX_GPIO_EVENT_LEN)
		return -1;
	return 0;
}

_gpio_event * gio_event_deque(uint8_t *pkg_index )
{
	int i=0;
	for(i=0;i<MAX_GPIO_EVENT_LEN;i++)
	{
		if(gpio_event_list.Done[i]==1)
		{
			*pkg_index=i;
			return &gpio_event_list.gpio_event[i];
		}
		else
		{

		}
	}
	return NULL;
}


_lf_event_list lf_event_list={
	{0},	
	{0,0,0,0},
};

void lf_event_init(void)
{
	for(int i=0;i<MAX_LF_EVENT_NUM;i++)
	{
		lf_event_list.Done[i]=0;
		lf_event_list.lf_event[i].pin=-1;
		lf_event_list.lf_event[i].IsStable=-1;
	}
	
	return;
}

int lf_event_enque(_lf_event *event)
{
	int i=0;
	for(i=0;i<MAX_LF_EVENT_NUM;i++)
	{
		if(lf_event_list.Done[i]==1)
		{
			continue;
		}
		else
		{
			lf_event_list.lf_event[i].pin=event->pin;
			lf_event_list.lf_event[i].level=event->level;
			lf_event_list.lf_event[i].IsStable=event->IsStable;
			lf_event_list.Done[i]=1;
			printf("%s \n",__func__);
			break;
		}
	}
	if(i>=MAX_LF_EVENT_NUM)
		return -1;
	return 0;
}

_lf_event * lf_event_deque(uint8_t *pkg_index )
{
	int i=0;
	for(i=0;i<MAX_LF_EVENT_NUM;i++)
	{
		if(lf_event_list.Done[i]==1)
		{
			*pkg_index=i;
			printf("*pkg_index:%d \n",*pkg_index);
			return &lf_event_list.lf_event[i];
		}
		else
		{

		}
	}
	return NULL;
}


