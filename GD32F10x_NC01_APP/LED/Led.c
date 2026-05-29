#include "Led.h"


#define LED_WORK_PORT  GPIOA//PA0_WORK_LED
#define LED_WORK_PIN  GPIO_PIN_0//PA0_WORK_LED

void led_init(void)
{
	/* enable the key clock */
	rcu_periph_clock_enable(RCU_GPIOA);
	
	gpio_init(LED_WORK_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,LED_WORK_PIN);
	gpio_bit_set(LED_WORK_PORT, LED_WORK_PIN);

	return ;
}
//turn on red/green/blue
void led_on(led_color_enum ledtype)
{
	switch((uint8_t)ledtype)
	{
		case LED_RED:
		;
		break;
		case LED_GREEN:
		gpio_bit_reset(LED_WORK_PORT, LED_WORK_PIN);
		break;
		case LED_BLUE:
		;
		break;
	}
	
}
//turn off red/green/blue
void led_off(led_color_enum ledtype)
{
	switch((uint8_t)ledtype)
	{
		case LED_RED:
		;
		break;
		case LED_GREEN:
		gpio_bit_set(LED_WORK_PORT, LED_WORK_PIN);
		break;
		case LED_BLUE:
		;
		break;
	}
}

void led_toggle(led_color_enum ledtype)
{
	switch((uint8_t)ledtype)
	{
		case LED_RED:
			;
		break;
		case LED_GREEN:
			gpio_bit_write(LED_WORK_PORT, LED_WORK_PIN, 
        (bit_status)(1-gpio_input_bit_get(LED_WORK_PORT, LED_WORK_PIN)));
		break;
		case LED_BLUE:
			;
		break;
	}
}

void led_run_onoff(uint8_t onoff)
{
	if(onoff==1)
		gpio_bit_reset(LED_WORK_PORT, LED_WORK_PIN);
	else
		gpio_bit_set(LED_WORK_PORT, LED_WORK_PIN);
}


