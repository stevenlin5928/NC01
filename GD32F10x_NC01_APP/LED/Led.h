#ifndef _LED_H
#define _LED_H
#include "gd32f10x.h"
/* exported types */
typedef enum 
{
    LED_RED = 0,
    LED_BLUE = 1,
    LED_GREEN = 2,
    LED_UNKONOW = 3
} led_color_enum;
void led_init(void);
void led_on(led_color_enum ledtype);
void led_off(led_color_enum ledtype);
void led_toggle(led_color_enum ledtype);

void led_run_onoff(uint8_t onoff);
void led_stop_onoff(uint8_t onoff);
#endif
