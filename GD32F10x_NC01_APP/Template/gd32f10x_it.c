/*!
    \file    gd32f10x_it.c
    \brief   interrupt service routines

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
#include "gd32f10x_it.h"
#include "main.h"
#include "systick.h"
#include "gp_io.h"
extern __IO uint32_t timedisplay;
extern int8_t m_reset_event;
extern __IO uint32_t ms_counter;

extern _GPI_FILTER gpi_filter[4];

/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
    /* if NMI exception occurs, go to infinite loop */
    while(1){
    }
}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while(1){
    }
}

/*!
    \brief      this function handles MemManage exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MemManage_Handler(void)
{
    /* if Memory Manage exception occurs, go to infinite loop */
    while(1){
    }
}

/*!
    \brief      this function handles BusFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void BusFault_Handler(void)
{
    /* if Bus Fault exception occurs, go to infinite loop */
    while(1){
    }
}

/*!
    \brief      this function handles UsageFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void UsageFault_Handler(void)
{
    /* if Usage Fault exception occurs, go to infinite loop */
    while(1){
    }
}

/*!
    \brief      this function handles SVC exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SVC_Handler(void)
{
    /* if SVC exception occurs, go to infinite loop */
    while(1){
    }
}

/*!
    \brief      this function handles DebugMon exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DebugMon_Handler(void)
{
    /* if DebugMon exception occurs, go to infinite loop */
    while(1){
    }
}

/*!
    \brief      this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PendSV_Handler(void)
{
    /* if PendSV exception occurs, go to infinite loop */
    while(1){
    }
}

/*!
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler(void)
{
    led_spark();
    delay_decrement();
}

void TIMER4_IRQHandler(void)
{
	extern void timer4_isr(void);
	if(timer_interrupt_flag_get(TIMER4, TIMER_INT_FLAG_UP))
	{	
		timer4_isr();
		timer_interrupt_flag_clear(TIMER4,TIMER_INT_FLAG_UP);		
	}		 	
}


/*!
    \brief      this function handles RTC global interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void RTC_IRQHandler(void)
{
    if (rtc_flag_get(RTC_FLAG_SECOND) != RESET){
        /* clear the RTC second interrupt flag*/
        rtc_flag_clear(RTC_FLAG_SECOND);

        /* enable time update */
        timedisplay = 1;
				printf("+");
        /* wait until last write operation on RTC registers has finished */
        rtc_lwoff_wait();
        /* reset RTC counter when time is 23:59:59 */
        if (rtc_counter_get() == 0x00015180){
            rtc_counter_set(0x0);
            /* wait until last write operation on RTC registers has finished */
            rtc_lwoff_wait();
        }
    }
}

/*!
    \brief      this function handles external lines 10 to 15 interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI0_IRQHandler(void)
{
    if(RESET != exti_interrupt_flag_get(EXTI_0)) {
				if(gpi_filter[0].debounce_flag==0)
					gpi_filter[0].debounce_flag=1;		
				exti_interrupt_flag_clear(EXTI_0);
				//printf("GPI_IN_1\n");	
    }
}

/*!
    \brief      this function handles external lines 10 to 15 interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI1_IRQHandler(void)
{
    if(RESET != exti_interrupt_flag_get(EXTI_1)) {
				exti_interrupt_flag_clear(EXTI_1);
				//printf("GPI_IN_2\n");

				if(gpi_filter[1].debounce_flag==0)
					gpi_filter[1].debounce_flag=1;
    }
}

/*!
    \brief      this function handles external lines 10 to 15 interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI2_IRQHandler(void)
{
    if(RESET != exti_interrupt_flag_get(EXTI_2)) {
				exti_interrupt_flag_clear(EXTI_2);
			//printf("GPI_IN_3\n");	

				if(gpi_filter[2].debounce_flag==0)
					gpi_filter[2].debounce_flag=1;
    }
}

/*
	int8_t type;//1:out;0:input
	int8_t retport_type;//0:passive;1:Inpassive
	int8_t pin;
	int8_t level;
*/
/*!
    \brief      this function handles external lines 10 to 15 interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI3_IRQHandler(void)
{
    if(RESET != exti_interrupt_flag_get(EXTI_3)) {
				exti_interrupt_flag_clear(EXTI_3);
				//printf("GPI_IN_4\n");	

				if(gpi_filter[3].debounce_flag==0)
					gpi_filter[3].debounce_flag=1;
    }
}

void EXTI4_IRQHandler(void)
{
    if(RESET != exti_interrupt_flag_get(EXTI_4)) {
				exti_interrupt_flag_clear(EXTI_4);
				//printf("GPI_IN_5\n");	

				if(gpi_filter[4].debounce_flag==0)
					gpi_filter[4].debounce_flag=1;
    }
}



//EXTI5_9_IRQn
/*!
    \brief      this function handles external lines 5 to 9 interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI5_9_IRQHandler(void)
{
    if(RESET != exti_interrupt_flag_get(EXTI_5)) {
			exti_interrupt_flag_clear(EXTI_5);
							//printf("GPI_IN_6\n");
			if(gpi_filter[5].debounce_flag==0)
				gpi_filter[5].debounce_flag=1;
    }		
		else if(RESET != exti_interrupt_flag_get(EXTI_6)) {
        exti_interrupt_flag_clear(EXTI_6);
				//printf("GPI_IN_7\n");
			if(gpi_filter[6].debounce_flag==0)
				gpi_filter[6].debounce_flag=1;
    }
		else if(RESET != exti_interrupt_flag_get(EXTI_7)) {
			exti_interrupt_flag_clear(EXTI_7);
			//printf("GPI_IN_8\n");
			if(gpi_filter[7].debounce_flag==0)
				gpi_filter[7].debounce_flag=1;
    }

		else
			printf("exti int\n");

}
