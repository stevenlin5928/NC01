#ifndef __MAX7219_H
#define __MAX7219_H 

#include "gd32f10x.h"
void Max7219_power_init(void);
void Max7219_power_on_off(int on);

void Init_MAX7219(void);
void Write_Max7219(uint8_t address,uint8_t dat);
void MAX7219_Clear (void) ;

#endif
