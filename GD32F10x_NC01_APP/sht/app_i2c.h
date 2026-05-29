#ifndef _APP_I2C_H
#define _APP_I2C_H
#include <stdio.h>
#include <stdlib.h>

#include "gd32f10x.h"
#include "gd32f10x_dma.h"
#include "gd32f10x_gpio.h"
#include "gd32f10x_rcu.h"
#include "gd32f10x_timer.h"

#include "app_i2c.h"
#include "sht21.h"


void I2C0_config (void);

int8_t  I2C0_Register_Read(uint8_t* B_buffer, uint8_t read_address,uint16_t number_of_byte);
#endif
