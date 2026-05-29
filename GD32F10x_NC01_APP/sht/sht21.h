#ifndef _SHT21_H
#define _SHT21_H
#include "gd32f10x.h"


#define SHT21_ADDR							(0x80)

#define SHT21_CMD_TEMPERATURE		0xE3				
#define SHT21_CMD_HUMIDITY			0xE5
#define SHT21_USER_REG					0xE7
#define SHT21_SOFT_RESET				0xFE

#define I2C_TIMEOUT 						100000
void I2C1_config(void);
void SHT21_Init(void);
//
int32_t SHT21_ReadTemp (void);
int32_t SHT21_ReadHumidity (void);

#endif

