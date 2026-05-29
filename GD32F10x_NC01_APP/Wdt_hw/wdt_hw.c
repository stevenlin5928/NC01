#include "wdt_hw.h"
#define WDT_HW_RCU   RCU_GPIOB

/*

*/
void wdt_hw_int(void)
{
	/* enable the key clock */
	rcu_periph_clock_enable(WDT_HW_RCU);
	/* configure beep GPIO port */ 
	gpio_init(WDT_HW_ENABLE_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,WDT_HW_ENABLE_PIN);
	gpio_init(WDT_HW_WDI_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,WDT_HW_WDI_PIN);
	gpio_bit_reset(WDT_HW_ENABLE_PORT, WDT_HW_ENABLE_PIN);
	gpio_bit_reset(WDT_HW_WDI_PORT, WDT_HW_WDI_PIN);
	return;
}

void wdt_hw_enable(void)
{
	gpio_bit_reset(WDT_HW_ENABLE_PORT, WDT_HW_ENABLE_PIN);
	return;
}

void wdt_hw_disable(void)
{
	gpio_bit_set(WDT_HW_ENABLE_PORT, WDT_HW_ENABLE_PIN);
	return;
}

void wdt_hw_set_wdi(int val)
{
	if(val>0)
		gpio_bit_set(WDT_HW_WDI_PORT, WDT_HW_WDI_PIN);
	else
		gpio_bit_reset(WDT_HW_WDI_PORT, WDT_HW_WDI_PIN);
}

void wdt_hw_toggle_wdi()
{
	gpio_bit_write(WDT_HW_WDI_PORT, WDT_HW_WDI_PIN, 
        (bit_status)(1-gpio_input_bit_get(WDT_HW_WDI_PORT, WDT_HW_WDI_PIN)));
}
