/*!
    \file    gd32f10x_eval.h
    \brief   definitions for GD32F10x_EVAL's leds, keys and COM ports hardware resources

    \version 2024-01-05, V2.3.0, firmware for GD32F10x
*/

/*
    Copyright (c) 2024, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#ifndef _KEYPAD_H_
#define _KEYPAD_H_

#ifdef cplusplus
 extern "C" {
#endif

#include "gd32f10x.h"
/*
0P00 温度控制下限 20
0P01 温度控制上限 24
0P02 瀑度度控制下限 23
0P03 瀑度控制上限 24
0P04 温度警報下限 19
0PO5 温度警報上限 25

0P06 海度警拔下限 20
0P07 漏度警报上限 27
0P08 温度佴差襦正值 校正
0P09 洱度偏差裤正伯 校正
0P10 开門&警蜂鳴器延理時聞
*/
#define OP_MAX 11
typedef enum {
	OP00=0,
	OP01=1,
	OP02=2,
	OP03=3,
	OP04=4,
	OP05=5,
	OP06=6,
	OP07=7,
	OP08=8,
	OP09=9,
	OP10=10,
}MODE_FN;

typedef  struct{
	MODE_FN fnx;
	uint16_t init_value;//init value,read from flash
	uint16_t modified_val;//modified value
	uint16_t  bitweight;//x1,x10,x100
}FNx_par;

typedef struct{
	MODE_FN cur_mode;
	FNx_par  *pFnxpar;
}keypad_OP;
void load_nc01_parameters(void);
void keypad_init(void);
int8_t is_normal_mode(void);
void flash_one_8_seg(void );
#ifdef cplusplus
}
#endif

#endif /* GD32F10X_EVAL_H */
