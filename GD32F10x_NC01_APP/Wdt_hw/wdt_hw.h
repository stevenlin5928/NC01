#ifndef _WDT_HW_H
#define _WDT_HW_H
#include "gd32f10x.h"
#define WDT_HW_ENABLE_PORT GPIOB      //PB15_WD_EN
#define WDT_HW_ENABLE_PIN  GPIO_PIN_15
#define WDT_HW_WDI_PORT    GPIOB      //PB14_WDI
#define WDT_HW_WDI_PIN     GPIO_PIN_14
void wdt_hw_int(void);
void wdt_hw_enable(void);
void wdt_hw_disable(void);
void wdt_hw_set_wdi(int wdi_pin);
#endif
