#ifndef I2C_HAL_H
#define I2C_HAL_H
//==============================================================================
//    S E N S I R I O N   AG,  Laubisruetistr. 50, CH-8712 Staefa, Switzerland
//==============================================================================
// Project   :  SHT2x Sample Code (V1.2)
// File      :  I2C_HAL.h
// Author    :  MST
// Controller:  NEC V850/SG3 (uPD70F3740)
// Compiler  :  IAR compiler for V850 (3.50A)
// Brief     :  I2C Hardware abstraction layer
//==============================================================================

//---------- Includes ----------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>

#include "gd32f10x.h"
#include "gd32f10x_dma.h"
#include "gd32f10x_gpio.h"
#include "gd32f10x_rcu.h"
#include "gd32f10x_timer.h"

//PA11_SHT21_EN
#define SHT21_POWER_PORT GPIOA
#define SHT21_POWER_PIN  GPIO_PIN_11



#define SCL_PIN  GPIO_PIN_6//SCL
#define SDA_PIN  GPIO_PIN_7// I2C0_SDA
//---------- Defines -----------------------------------------------------------
//I2C ports
//The communication on SDA and SCL is done by switching pad direction
//For a low level on SCL or SDA, direction is set to output. For a high level on
//SCL or SDA, direction is set to input. (pull up resistor active)
#define SCL_OUTPUT()  gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SCL_PIN)
#define SCL_INPUT()   gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, SCL_PIN)

#define SDA_OUTPUT()  gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SDA_PIN)
#define SDA_INPUT()   gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, SDA_PIN)

#define SDA_H()  gpio_bit_set(GPIOB, SDA_PIN)    // ?????
#define SDA_L()  gpio_bit_reset(GPIOB, SDA_PIN)
#define SCL_H()  gpio_bit_set(GPIOB, SCL_PIN)
#define SCL_L()  gpio_bit_reset(GPIOB, SCL_PIN)
#define SHT20_SDA_READ()  gpio_input_bit_get(GPIOB,SDA_PIN)
//---------- Enumerations ------------------------------------------------------
//  I2C level
typedef enum{
  LOW                      = 0,
  HIGH                     = 1,
}etI2cLevel;

// I2C acknowledge
typedef enum{
  ACK                      = 0,
  NO_ACK                   = 1,
}etI2cAck;
//---------- Enumerations ------------------------------------------------------
// Error codes
typedef enum{
  ACK_ERROR                = 0x01,
  TIME_OUT_ERROR           = 0x02,
  CHECKSUM_ERROR           = 0x04,
  UNIT_ERROR               = 0x08
}etError;
//==============================================================================
void I2c_Init (void);
//==============================================================================
//Initializes the ports for I2C interface

void sht2x_power_init(void);

void sht2x_power_on_off(int on);
//==============================================================================
void SHT21_Init(void);

//iic start
void IIC_Start(void);
//iic stop
void IIC_Stop(void);
uint8_t IIC_Wait_Ack(void);
//iic Ack
void IIC_Ack(void);
//iic NACK
void IIC_NAck(void);
//iic write byte
void IIC_Send_Byte(uint8_t txd);
//iic read byte
uint16_t IIC_Read_Byte(uint8_t ack);

#endif
