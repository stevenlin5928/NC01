#include "sensor.h"


/*
init position sensor port
*/
void position_sensor_int(void)
{
	/* enable the key clock */
	rcu_periph_clock_enable(RCU_AF);
	
	rcu_periph_clock_enable(RCU_GPIOC);
	/* configure beep GPIO port */ 
	gpio_init(SW1_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_2MHZ,SW1_PIN);
	gpio_init(SW2_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_2MHZ,SW2_PIN);
	gpio_init(SW3_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_2MHZ,SW3_PIN);
#if 0//def SUPPORT_POSITION

	//sw1
	/* connect key EXTI line to key GPIO pin */
	gpio_exti_source_select(SW1_EXTI_PORT_SOURCE, SW1_EXTI_PIN_SOURCE);

	/* configure key EXTI line */
	exti_init(SW1_EXTI_LINE, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
	exti_interrupt_flag_clear(SW1_EXTI_LINE);
	
	//SW2
	/* connect key EXTI line to key GPIO pin */
	gpio_exti_source_select(SW2_EXTI_PORT_SOURCE, SW2_EXTI_PIN_SOURCE);

	/* configure key EXTI line */
	exti_init(SW2_EXTI_LINE, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
	exti_interrupt_flag_clear(SW2_EXTI_LINE);
	
	//SW3
	/* connect key EXTI line to key GPIO pin */
	gpio_exti_source_select(SW3_EXTI_PORT_SOURCE, SW3_EXTI_PIN_SOURCE);

	/* configure key EXTI line */
	exti_init(SW3_EXTI_LINE, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
	exti_interrupt_flag_clear(SW3_EXTI_LINE);
		/* enable and set key EXTI interrupt to the lowest priority */
	nvic_irq_enable(SW3_EXTI_IRQn, 2U, 0U);
#endif/**/
printf("Position sensor init\n");
	return;
}

/*
Read position sensor  SW1
*/
uint8_t get_position_sensor_sw1_status(void)
{
	return gpio_input_bit_get(SW1_PORT, SW1_PIN);
}

/*
Read position sensor  SW2
*/
uint8_t get_position_sensor_sw2_status(void)
{
	return gpio_input_bit_get(SW2_PORT, SW2_PIN);
}
/*
Read position sensor  SW3
*/
uint8_t get_position_sensor_sw3_status(void)
{
	return gpio_input_bit_get(SW3_PORT, SW3_PIN);
}
