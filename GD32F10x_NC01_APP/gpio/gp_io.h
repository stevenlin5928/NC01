#ifndef _GP_IO_H
#define _GP_IO_H
#include "gd32f10x.h"
//Input 
#define GPI_IN_PORT GPIOC      //
#define GPI_IN_1  GPIO_PIN_0//PC0_INPUT1
#define GPI_IN_2  GPIO_PIN_1//PC1_INPUT2
#define GPI_IN_3  GPIO_PIN_2//PC2_INPUT3 
#define GPI_IN_4  GPIO_PIN_3//PC3_INPUT4
#define GPI_IN_5  GPIO_PIN_4//PC4_INPUT5
#define GPI_IN_6  GPIO_PIN_5//PC5_INPUT6
#define GPI_IN_7  GPIO_PIN_6//PC6_INPUT7
#define GPI_IN_8  GPIO_PIN_7//PC7_INPUT8
//
#define GPO_OUT_1_PORT GPIOB  
#define GPO_OUT_1  GPIO_PIN_4 //PB4_OUTPUT1
#define GPO_OUT_2_PORT GPIOC 
#define GPO_OUT_2  GPIO_PIN_12//PC12_OUTPUT2
#define GPO_OUT_3_PORT GPIOC
#define GPO_OUT_3  GPIO_PIN_11//PC11_OUTPUT3
#define GPO_OUT_4_PORT GPIOC
#define GPO_OUT_4  GPIO_PIN_10//PC10_OUTPUT4
#define GPO_OUT_5_PORT GPIOB
#define GPO_OUT_5  GPIO_PIN_0//PB0_OUTPUT5
#define GPO_OUT_6_PORT GPIOB
#define GPO_OUT_6  GPIO_PIN_1//PB1_OUTPUT6
#define GPO_OUT_7_PORT GPIOC
#define GPO_OUT_7  GPIO_PIN_8//PC8_OUTPUT7
#define GPO_OUT_8_PORT GPIOC
#define GPO_OUT_8  GPIO_PIN_9//PC9_OUTPUT8
typedef struct 
{
	int8_t type;//1:out;0:input
	int8_t retport_type;//0:passive;1:Inpassive
	int8_t pin;
	int8_t level;
	uint32_t msta;
}_gpio_event;

#define GPI_MAX 8
#define MAX_LF_EVENT_NUM 32
typedef struct {
	int8_t pin;
	int8_t level;
	int8_t IsStable;//1:stable;0: unstable
	uint8_t reserved;
}_lf_event;


typedef  struct {
	uint8_t Done[MAX_LF_EVENT_NUM];
	_lf_event lf_event[MAX_LF_EVENT_NUM];
}_lf_event_list;

#define MAX_GPIO_EVENT_LEN 32
typedef  struct {
	uint8_t Done[MAX_GPIO_EVENT_LEN];
	_gpio_event gpio_event[MAX_GPIO_EVENT_LEN];
}_gpio_event_list;



/*

*/
typedef struct {
	int8_t pin;
	int8_t debounce_flag;//1-debounce; 0:debounce
	int16_t keysample;
	int32_t counter;
}_GPI_FILTER;

void gp_io_init(void);
int8_t gp_get_out(uint16_t out,uint8_t *value);
void gp_set_out(uint16_t out,uint8_t level);
int gp_get_in(uint8_t in,uint8_t *value);
//gio event
void gio_event_init(void);

int gio_event_enque(_gpio_event *event);

_gpio_event * gio_event_deque(uint8_t *pkg_index );


void lf_event_init(void);

int lf_event_enque(_lf_event *event);

_lf_event * lf_event_deque(uint8_t *pkg_index );
#endif
