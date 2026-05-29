#ifndef _SENSOR_H
#define _SENSOR_H
#include "gd32f10x.h"

/*position Sensor detect */
#define SUPPORT_SW_INTERRUPT 1
//POSTION sw1
#define SW1_PORT GPIOC//PC6_POSI_SW1
#define SW1_PIN GPIO_PIN_6//PC6_POSI_SW1

//POSTION sw2
#define SW2_PORT GPIOC//PC7_POSI_SW2
#define SW2_PIN GPIO_PIN_7//PC7_POSI_SW2

//POSTION sw3
#define SW3_PORT GPIOC//PC8_POSI_SW3
#define SW3_PIN GPIO_PIN_8//PC8_POSI_SW3

#define SW1_EXTI_LINE             EXTI_6
#define SW1_EXTI_PORT_SOURCE      GPIO_PORT_SOURCE_GPIOC
#define SW1_EXTI_PIN_SOURCE       GPIO_PIN_SOURCE_6
#define SW1_EXTI_IRQn             EXTI5_9_IRQn

#define SW2_EXTI_LINE             EXTI_7
#define SW2_EXTI_PORT_SOURCE      GPIO_PORT_SOURCE_GPIOC
#define SW2_EXTI_PIN_SOURCE       GPIO_PIN_SOURCE_7
#define SW2_EXTI_IRQn             EXTI5_9_IRQn

#define SW3_EXTI_LINE             EXTI_8
#define SW3_EXTI_PORT_SOURCE      GPIO_PORT_SOURCE_GPIOC
#define SW3_EXTI_PIN_SOURCE       GPIO_PIN_SOURCE_8
#define SW3_EXTI_IRQn             EXTI5_9_IRQn

void position_sensor_int(void);
uint8_t get_position_sensor_sw1_status(void);
uint8_t get_position_sensor_sw2_status(void);
uint8_t get_position_sensor_sw3_status(void);

#endif/**_SENSOR_H**/
