#include "Beep.h"
#define BEEP_RCU   RCU_GPIOB
#define BEEP_PORT  GPIOB //BEEP_PB13
#define BEEP_PIN   GPIO_PIN_13
void beep_init()
{
	/* enable the key clock */
	rcu_periph_clock_enable(BEEP_RCU);
	/* configure beep GPIO port */ 
	gpio_init(BEEP_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,BEEP_PIN);
	gpio_bit_reset(BEEP_PORT, BEEP_PIN);

	return ;
}

void beep_on()
{
	gpio_bit_set(BEEP_PORT, BEEP_PIN);
	return;
}

void beep_off()
{
	gpio_bit_reset(BEEP_PORT, BEEP_PIN);
	return ;
}
