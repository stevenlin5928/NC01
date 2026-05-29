#include <stdarg.h>
#include "gd32f10x.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "board.h"
#include "secs.h"
int myputstr(uint8_t *buf,int len);
float NetworkToFloat(uint8_t *net_buf);
extern void rs232_post_received_data(uint8_t* data, uint16_t len);
extern void Get_UUID(uint8_t *mac,int len);
extern unsigned short generate_crc16(unsigned  char *buf,unsigned char len);
uint8_t IsDeviceValid(void);

#define MAX_TICK_LINK_TEST 60000 //60seconds for secs link_test
extern __IO uint32_t timedisplay;
extern sio_byte_buf_type 	sio_rx_buf,rs232_rx_buf_console;
extern sio_byte_buf_type 	secs_rx_buf;
__IO uint8_t hw_wdt_test=0;
const uint16_t FWSWver=((0x01<<8)|0x09);//1.2
__IO uint32 link_test_tick = MAX_TICK_LINK_TEST;  //link_test_tick vs MAX_TICK_LINK_TEST, check secs link test
__IO uint32 link_ok = 0;//1-link ok;0-timeout disconnect
__IO uint32_t ms_counter = 0U;

static __IO uint32_t cc_times = 0;

// SECS
unsigned char JY_DAM_A2_RESP[64];
uint8					JY_DAM_A2_RESP_LENGTH = 37;	

//hb
uint32_t MAX_TICK=8000;
uint32_t hb_sta=0;
uint8_t hb_rsp=0;
uint8_t hb_enable=0;

//led flash mode
uint8_t ledmode=0;//0:off; 1:on;  2:0.5HZ; 3:1Hz ;4:2Hz
uint16_t maxcount=500;
uint8_t redledmode=0; //0:off; 1:on; 3:1Hz
uint8_t greenledmode=0; //0:off; 1:on; 3:1Hz
uint8_t blueledmode=0; //0:off; 1:on; 3:1Hz
//
uint32_t greenDelay=0;
uint32_t redDelay=0;
uint32_t blueDelay=0;
static __IO uint32_t	ledms=0;	
//
__IO int8_t m_reset_event=-1;
__IO int32_t skip_counter=0;
__IO uint32_t p_run_counter=0xffff0000;//0;


extern int8_t SECS_S18F9_ENABLE;
extern int32_t SECS_Code;

__IO uint8_t bIwdg_reset=0;
__IO uint8_t ValidDevice_ok=0;

#define MAX_LF_LEN  32
#define MAX_WAIT_S18F72 9000
__IO uint8 targetid=3,ceid=5;
__IO uint8_t debug_flag=0;

uint8_t bSetDefaultRtc=0;

typedef struct {
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint8_t weekdays;
}_RTC;

//int8_t read_rtc(_RTC *prtc);
//int8_t write_rtc(_RTC *prtc);

_RTC initrtc={
		.year=2025,
		.month=3,
		.day=3,
		.hour=17,
		.minute=0,
		.second=0,
		.weekdays=1
};
//open electric magnetic gate and beeep delay time
uint8_t 	__IO magnetic_gate_beep_delay_enable=0;//enable reset
uint32_t 	__IO magnetic_gate_beep_delay_time=30000;//reset time
uint32_t 	__IO magnetic_gate_beep_delay_count=0;//reset count time
uint8_t 	__IO magnetic_gate_beep_delay_Done=0;/*reset Done*/
	
//Uart receive test
uint8_t 	uart_test_port=0;	//COM1 COM2
uint8_t 	uart_test_flag=1;	//1:start test;0:stop test

//max7219 led mantrix
extern uint8_t Max7219_code_disp[36][8];

_GPI_FILTER gpi_filter[GPI_MAX];

void reset_gpi_filter(uint8_t index)
{
	int i=index%GPI_MAX;
	gpi_filter[i].counter=0;
	gpi_filter[i].keysample=0x5AA5;
	gpi_filter[i].debounce_flag=0;
	gpi_filter[i].pin=-1;
	return;
}

void process_gpi_filter(void)
{
	for(uint8_t i=0;i<GPI_MAX;i++)
	{
		if(gpi_filter[i].debounce_flag==1)
		{
			uint8_t level=0;;
			gp_get_in(i,&level);
			gpi_filter[i].keysample=(gpi_filter[i].keysample<<1)|level;
			if(gpi_filter[i].counter>8)
			{
				if(gpi_filter[i].keysample==0x00)
				{
					//falling edge
					gpi_filter[i].debounce_flag=0;
					printf("gpi_%d falling\n",i);
				}
				else if(gpi_filter[i].keysample==(int16_t)0xffff)
				{
					//rising edge
					gpi_filter[i].debounce_flag=0;
					printf("gpi_%d rising\n",i);
				}
			}
			gpi_filter[i].counter++;
		}
	}

}

void magnetic_gate_beep_start(){
	//add code for trigger here
//	gpio_bit_set(BARCODE_TRIG_PORT, BARCODE_TRIG_PIN);
//	delay_1ms(100);
	//gpio_bit_reset(BARCODE_TRIG_PORT, BARCODE_TRIG_PIN);
	printf("start magnetic gate and beep\n");
}

void magnetic_gate_beep_on(){
	//on beep GPO#5
	gp_set_out(4,1);
	printf("magnetic gate beep on\n");
}
void magnetic_gate_beep_off(){
	//off beep GPO#5
    
	gp_set_out(4,0);
	//printf("magnetic gate beep off\n");
}

//use timer to reset  magnetic gate  and beep
void reset_magnetic_gate_beep(void)
{
	magnetic_gate_beep_delay_enable=1;
	magnetic_gate_beep_delay_count=0;
	magnetic_gate_beep_delay_Done=0;
}

/*!
    \brief      toggle the led every 500ms
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_spark(void)
{
	static __IO uint32_t timingdelaylocal = 0U;

	if(timingdelaylocal){
        if(timingdelaylocal < 500U){
            gpio_bit_set(GPIOA, GPIO_PIN_0);
        }else{
            gpio_bit_reset(GPIOA, GPIO_PIN_0);
        }

		timingdelaylocal--;
	}else{
		timingdelaylocal = 1000U;
	}
	ms_counter++;

	process_gpi_filter();
		
	
	if(magnetic_gate_beep_delay_enable==1)
	{
		if(magnetic_gate_beep_delay_count==0)
		{
					//
			//magnetic_gate_beep_start();			
		}
		else if(magnetic_gate_beep_delay_count==magnetic_gate_beep_delay_time)
		{
			printf("On magnetic_gate_beep_delay_time \n");
			magnetic_gate_beep_on();
			magnetic_gate_beep_delay_enable=0;
			magnetic_gate_beep_delay_Done=1;
					
		}
        //printf("magnetic_gate_beep_delay_count \n");
		magnetic_gate_beep_delay_count++;
	}
	if(link_test_tick>MAX_TICK_LINK_TEST)
		link_ok=0;//timeout ,disconnect
	else
		link_test_tick++;
	return;
}


/*
 * TIM_Period / Auto Reload Register(ARR) = 1000   TIM_Prescaler--71 
 * �ж�����Ϊ = 1/(72MHZ /72) * 1000 = 1ms
 *
 * TIMxCLK/CK_PSC --> TIMxCNT --> TIM_Period(ARR) --> �ж� ��TIMxCNT����Ϊ0���¼��� 
 */
void TIM4_Configuration(void)
{
	timer_parameter_struct timer_initpara;

	/* ����TIM2CLK Ϊ 72MHZ */
	rcu_periph_clock_enable(RCU_TIMER4);
	timer_deinit(TIMER0);
	timer_init(TIMER0,&timer_initpara);
	timer_initpara.prescaler=119;//TIMER1CLK = SystemCoreClock/120 = 1
	timer_initpara.alignedmode=TIMER_COUNTER_EDGE; 
	timer_initpara.counterdirection= TIMER_COUNTER_UP ;
	timer_initpara.period=1000;
	timer_initpara.clockdivision=TIMER_CKDIV_DIV1;
	timer_initpara.repetitioncounter = 0;
	timer_init(TIMER4,&timer_initpara);
//	/* auto-reload preload enable */
//	timer_auto_reload_shadow_enable(TIMER4);
	nvic_irq_enable(TIMER4_IRQn,1,1);
	timer_interrupt_enable(TIMER4,TIMER_INT_UP);
	/* TIMER0 counter enable */
	timer_enable(TIMER4);   
}

void timer4_isr(void)
{

	ledms++;	
  if(ledms>=maxcount)
  {  
    ledms=0;
		//if(ledmode>1)
		//	LED1_TOGGLE
		if(redledmode>1)
			led_toggle(LED_RED);
		if(greenledmode>1)
			led_toggle(LED_GREEN);
		if(blueledmode>1)
			led_toggle(LED_BLUE);
		if(redledmode==1)
		{
			
			redDelay--;
			if(redDelay==0)
			{
				printf("R");
				redledmode=0;
				led_off(LED_RED);
			}
		}
		if(greenledmode==1)
		{
			greenDelay--;

			if(greenDelay==0)
			{
				printf("G");				
				greenledmode=0;
				led_off(LED_GREEN);
			}
		}
		
		if(blueledmode==1)
		{
			blueDelay--;

			if(blueDelay==0)
			{
				printf("B");				
				blueledmode=0;
				led_off(LED_BLUE);
			}
		}
  }
	
}

void stop_TIM4()
{
	timer_disable(TIMER4);   
}

void start_TIM4()
{
	timer_enable(TIMER4);   
}

void switchLEDmode(u8 mode)
{
	switch(mode)
	{
		case 5:  //red off 
				stop_TIM4();
				ledms=0;
				maxcount=1000;
				redledmode=0;//0:off; 1:on;  2:0.5HZ; 3:1Hz ;4:2Hz
				led_off(LED_RED);
			break;
		case 6:  //red on 
				stop_TIM4();
				ledms=0;
				maxcount=1000;
				redledmode=1;//0:off; 1:on;  2:0.5HZ; 3:1Hz ;4:2Hz
		    led_on(LED_RED);
				//start_TIM4();
			break;
		case 7:  //red flash 
				stop_TIM4();
				ledms=0;
				maxcount=1000;
				redledmode=3;//0:off; 1:on;  2:0.5HZ; 3:1Hz ;4:2Hz
				start_TIM4();
			break;
		case 8:  //green off
				stop_TIM4();
				ledms=0;
				maxcount=1000;
				greenledmode=0;//0:off; 1:on;  2:0.5HZ; 3:1Hz ;4:2Hz
				led_off(LED_GREEN);
				
			break;
		case 9:  //green on 
				stop_TIM4();
				ledms=0;
				maxcount=1000;
				greenledmode=1;//0:off; 1:on;  2:0.5HZ; 3:1Hz ;4:2Hz
				led_on(LED_GREEN);
			break;
		case 10: //green flash 
				stop_TIM4();
				ledms=0;
				maxcount=1000;
				greenledmode=3;//0:off; 1:on;  2:0.5HZ; 3:1Hz ;4:2Hz
				start_TIM4();
			break;
		case 11:  //blue off
				stop_TIM4();
				ledms=0;
				maxcount=1000;
				blueledmode=0;//0:off; 1:on;  2:0.5HZ; 3:1Hz ;4:2Hz
				led_off(LED_BLUE);
			break;
		case 12:  //blue on 
				stop_TIM4();
				ledms=0;
				maxcount=1000;
				blueledmode=1;//0:off; 1:on;  2:0.5HZ; 3:1Hz ;4:2Hz
				led_on(LED_BLUE);
			break;
		case 13: //blue flash 
				stop_TIM4();
				ledms=0;
				maxcount=1000;
				blueledmode=3;//0:off; 1:on;  2:0.5HZ; 3:1Hz ;4:2Hz
				start_TIM4();
			break;
		default:

			break;
	}
}


int before_filter(uint8_t *data,int len)
{
	_Debug2(5,"Len:%d \n",len);
	return 0;
}

void process_rcv_cmd(USERCMD *rxcmd)
{
	//process AT command
	uint8_t *data=rxcmd->data;
	uint16_t len=(data[5]<<8)|(data[6])+9;
//	printf("process_rcv_cmd:\n ");
//	for(i=0;i<len;i++)
//	{
//		printf("%02X ",data[i]);
//	}

	if(before_filter(data,len)>0)
	{
		return;
	}
	//_Debug2(0,"process_rcv_cmd:%d",len);
	//write to USART
	
	return ;
}



int checkSecsLinkOk()
{
	if(link_ok==1)
	{
		printf("%d \n",(int)link_test_tick);
		return 0;
	}
	else
		return -1;
}


float NetworkToFloat(uint8_t *net_buf) {
    uint32_t raw_int;
    
    // 1. 先用安全的方式將 4 個位元組組合（或用 memcpy 複製到 uint32_t）
    // 假設目前讀到的是 0x4035FA77
    raw_int = ((uint32_t)net_buf[0] << 24) | 
              ((uint32_t)net_buf[1] << 16) | 
              ((uint32_t)net_buf[2] << 8)  | 
              ((uint32_t)net_buf[3]);
              
    // 2. 使用 ARM 硬體指令，一瞬間把 0x4035FA77 反轉成 0x77FA3540
    //uint32_t reversed_int = __REV(raw_int);
    
    // 3. 強制轉型輸出（對齊記憶體）
    return *(float *)&raw_int;
}

int before_secs_filter(uint8_t *data,int len)
{
	uint8_t hsmsbuf[64]={0};
	//HSMS mHsms;

	int hemslen=0;
//	printf("before_secs_filter:\n ");
//	for(int i=0;i<len;i++)
//	{
//		printf("%02X ",data[i]);
//	}
	link_test_tick =0; //reset
	link_ok=1;
	if (SECS_S18F9_ENABLE == 0)
	{
		// select req
		if (len == 14 && data[4] == 0xff && data[5] == 0xff && data[9] == 0x01)
		{
			data[9]=0x02;
			if(ConfigMsg.debug>0)
				_Debug2(0, "Select request-->Send to EAP, L%d", __LINE__);
			send_secs_socket((uint8_t *)&data[0],14);
		}
		else if (len == 14 && data[4] == 0xff && data[5] == 0xff && data[9] == 0x05) // link test
		{
		
			memcpy(hsmsbuf, data, 14);
			data[9]=0x06;
			send_secs_socket((uint8_t *)&data[0],14);
		
		}
		else if ((len == 16 || len == 14) && data[6] == 0x81 && data[7] == 0x01) // S1F1
		{
			gp_set_out(0,1);
			
			_Debug2(0, "S1F1-->Send to EAP, --%d\n", __LINE__);
			
			
			
			extern uint8_t SECS_S1F2_HEADER_RESP[];
			extern unsigned char SECS_S1F1_DATA_RESP[45];
			SECS_Code = 120;

			memcpy(hsmsbuf, SECS_S1F2_HEADER_RESP, 14); 
		
			// copy session 4 bytes
			memcpy(hsmsbuf + 10, data + 10, 4);
			//FWSWver
			SECS_S1F1_DATA_RESP[24] ='A';
			SECS_S1F1_DATA_RESP[25] ='1';
			SECS_S1F1_DATA_RESP[26] ='.' ;
			SECS_S1F1_DATA_RESP[28] ='1' ;
			SECS_S1F1_DATA_RESP[29] =0x20 ;
			SECS_S1F1_DATA_RESP[30] =0x20 ;
			//B9100

			{
				SECS_S1F1_DATA_RESP[31] = 0x20;
				
				SECS_S1F1_DATA_RESP[32] = 0x20;
				SECS_S1F1_DATA_RESP[33] = 0x20;
				SECS_S1F1_DATA_RESP[34] = 0x20;
				SECS_S1F1_DATA_RESP[35] = 0x20;
				SECS_S1F1_DATA_RESP[36] = 0x20;
			}
			
			//2024-05-17
			SECS_S1F1_DATA_RESP[37]= 0x20;
			SECS_S1F1_DATA_RESP[38]= 0x20;
			SECS_S1F1_DATA_RESP[39]= 0x20;
			//
			SECS_S1F1_DATA_RESP[41]= 0x20;
			SECS_S1F1_DATA_RESP[42]= 0x20;
			SECS_S1F1_DATA_RESP[43]= 0x20;
			SECS_S1F1_DATA_RESP[44]= 0x20;
			
			memcpy(hsmsbuf + 14, SECS_S1F1_DATA_RESP, 45);   //2024-05-17
			hemslen=59;			
			
			send_secs_socket(hsmsbuf,hemslen);
		}		
		else if ((len > 14) && data[6] == 0x81 && data[7] == 0x03) // S1F3
		{
			unsigned char SECS_S1F4_DATA_RESP[32] = {0x00,0x00,0x00,0x13,0x00,0x00,0x01,0x04,0x00,0x00,0x00,0x00,0x00,0x0D,0x01,0x01,0x41,0x05,0x00,0x00,0x00,0x00,0x00};
			_Debug2(0, "S1F3-->Send to EAP, --%d", __LINE__);
			// clean buffer
			for(int i = 0; i < JY_DAM_A2_RESP_LENGTH; i++)
			{
				JY_DAM_A2_RESP[i] = 0x00;
			}
			
			// copy session 4 bytes
			memcpy(SECS_S1F4_DATA_RESP + 10, data + 10, 4);
			
			gp_set_out(1,1);
			uint8_t cmd_buf[] = {0x01,0x04,0x00,0x32,0x00,0x10,0x50,0x09};
			myputstr(cmd_buf,8);
			delay_ms(400);
			char v_str[4];
			if(JY_DAM_A2_RESP[0] == 0x01 && JY_DAM_A2_RESP[1] == 0x04)
			{
				uint8_t buf[4];
				buf[0] = JY_DAM_A2_RESP[7];
				buf[1] = JY_DAM_A2_RESP[8];
				buf[2] = JY_DAM_A2_RESP[9];
				buf[3] = JY_DAM_A2_RESP[10];
				float f = NetworkToFloat(buf);
				printf("JY-DAM Output: %2X %2X %2X %2X , %.3f V\n", buf[0],buf[1],buf[2],buf[3],f); 
				
				sprintf(v_str, "%.3fV", f); 
				memcpy(&SECS_S1F4_DATA_RESP[18], v_str, strlen(v_str));
			}
			else{
				printf("No response\n"); 
				send_secs_socket(SECS_S1F4_DATA_RESP, 23);
			}
			// DATA Length
			SECS_S1F4_DATA_RESP[17] = strlen(v_str);
			// Message Length
			SECS_S1F4_DATA_RESP[3] = 14 + strlen(v_str);
			uint16 _total_len_ = 4 + SECS_S1F4_DATA_RESP[3];
			
			// Send to server
			send_secs_socket(SECS_S1F4_DATA_RESP, _total_len_);
		
			gp_set_out(1,0);
		}
		else if ((len > 14) && data[6] == 0x92 && data[7] == 0x0f) // S18F15
		{
			extern unsigned char SECS_S18F16_HEADER_RESP[14];
			extern unsigned char SECS_S18F16_DATA_RESP[5];
			SECS_Code = 120;

			memcpy(hsmsbuf, SECS_S18F16_HEADER_RESP, 14); 

		
			// copy session 4 bytes
			memcpy(hsmsbuf + 10, data + 10, 4);

			
			memcpy(hsmsbuf + 14, SECS_S18F16_DATA_RESP, sizeof(SECS_S18F16_DATA_RESP));   //2024-10-21
			hemslen=14+sizeof(SECS_S18F16_DATA_RESP);	
			hsmsbuf[2]=((hemslen-4)>>8)&0xff;		
			hsmsbuf[3]=(hemslen-4)&0xff;
			
			printf("S18F15 Data\r\n");
			for(int  i=0;i<len;i++)
			{
				printf(" %02x",data[i]);
			}
			printf("\r\n");
			//1:Green 2:Red 3:Blue 4:buzzer 5:LCD
			//0=>off 1=>on 2=>flash
			
			uint8_t greenmode=data[62]-0x30;
			uint8_t redmode=data[63]-0x30;
			uint8_t bluemode=data[64]-0x30;
			uint8_t buzzer=data[65]-0x30;
			uint8_t lcdNum=data[66]-0x30;
			switch(greenmode)
			{
				case 0://Off
					switchLEDmode(8);
					break;
				case 1://On
					switchLEDmode(9);
					break;
				case 2://flash
					switchLEDmode(10);
					break;
			}
			
			switch(redmode)
			{
				case 0://Off
					switchLEDmode(5);
					break;
				case 1://On
					switchLEDmode(6);
					break;
				case 2://flash
					switchLEDmode(7);
					break;
			}
			
			switch(bluemode)
			{
				case 0://Off
					switchLEDmode(11);
					break;
				case 1://On
					switchLEDmode(12);
					break;
				case 2://flash
					switchLEDmode(13);
					break;
			}
			//
			greenDelay=(((data[69]-0x30)<<8)+(data[70]-0x30));
			redDelay=(((data[71]-0x30)<<8)+(data[72]-0x30));
			blueDelay=(((data[73]-0x30)<<8)+(data[74]-0x30));
			
			_Debug2(0, "S18F16-->Send to EAP, --%d,greenDelay:%02X,redDelay:%02X,blueDelay:%02X", __LINE__,greenDelay,redDelay,blueDelay);
			if(ConfigMsg.debug>0)
				_Debug2(0, "S18F16-->Send to EAP, --%d,hemslen:%d ,%d:%d:%d:%d:%d", __LINE__,hemslen,greenmode,redmode,bluemode,buzzer,lcdNum);
			
			send_secs_socket(hsmsbuf,hemslen);
		}
		else if (len == 17 &&data[6] == 0x82 && data[7] == 0x15) // S2F21->S18F22
		{
			// copy session 4 bytes
			uint32_t session=(data[10]<<24)|(data[11]<<16)
													|(data[12]<<8)|data[13];
			uint8_t trackMode=data[16];
			report_S2F22(session);
			if(ConfigMsg.debug>0)
				_Debug2(0, "L_%d trackMode:%02X\n",__LINE__,trackMode);

		}
		else if (data[6] == 0x82 && data[7] == 0x11) // S2F17->S2F18
		{
			// copy session 4 bytes
			uint32_t session=(data[10]<<24)|(data[11]<<16)
													|(data[12]<<8)|data[13];
			uint8_t trackMode=data[16];
			report_S2F18(session,(uint8_t*)"20241023142219",strlen("20241023142219"));
			if(ConfigMsg.debug>0)
				_Debug2(0, "L_%d trackMode:%02X\n",__LINE__,trackMode);

		}
		else if (data[6] == 0x92 && data[7] == 0x09) // S18F9->S18F10-one session
		{
			extern unsigned char SECS_S18F10_HEADER_RESP[14];
			// copy session 4 bytes
			memcpy(SECS_S18F10_HEADER_RESP + 10, data + 10, 4);

			uint16 ant=data[3] & 0x0f; // get ant no
			//
			ant=ant%GPI_MAX;

			if(ConfigMsg.debug>0)
				_Debug2(0, "L_%d RFID_ANTENNA:%d\n",__LINE__,0);

		}
		else if (data[6] == 0x92 && (data[7] == 0x48||data[7] == 0x49)) // S18F72
		{

			if(ConfigMsg.debug>0)
				_Debug2(0, "L_%d S18F72\n",__LINE__);

		}
//		else if (len == 2 && hsmsbuf[0] == 0xa0 && hsmsbuf[1] == 0x0a) // fun:S18F71, time up, from gpi trigger
//		{
//			printf("timeout %d",__LINE__);
//		}

	}
//  else if(SECS_S18F9_ENABLE == 1)
//	{
//		if (len == 4) // S18F9-->Data
//		{
//			SECS_S18F9_ENABLE = 2;
//			//tcp_source_mode|=RFID_SECS_TCP;
//			SECS_Code = 140;
//			bUseUhf=1;
//			memset(USER_BANK_BUFFER, 0x00, sizeof(USER_BANK_BUFFER));

//			memcpy(USER_BANK_BUFFER, SECS_S18F10_HEADER_RESP, sizeof(SECS_S18F10_HEADER_RESP));
//			// copy data
//			memcpy(USER_BANK_BUFFER + sizeof(SECS_S18F10_HEADER_RESP), SECS_S18F10_DATA_RESP, sizeof(SECS_S18F10_DATA_RESP));

//			RFID_ANTENNA = data[3] & 0x0f; // get ant no
//			// 
//			USER_BANK_BUFFER[18+1] = data[3];
//			if(ConfigMsg.debug>0)
//				_Debug2(0,"S18F9 RFID_ANTENNA:%x,SECS_S18F9_ENABLE == 1\n",RFID_ANTENNA);
//		}
//	}
	

	return 0;
}


uint16_t amkor_nc01_crc(uint8_t *data,int len)
{
	uint16_t crc_sum=0;
	for(uint8_t i=0;i<len-2;i++)
	{
		crc_sum+=data[i];
	}
	data[len-2]=(uint8_t)((crc_sum>>8)&0xff);
	data[len-1]=(uint8_t)(crc_sum&0xff);
	return crc_sum;
	
}

int before_amkor_nc01_filter(uint8_t *data,int len)
{
	link_test_tick =0; //reset
	link_ok=1;

	// 5A 04 00 00 00 00 5E
	if (len == 7&& data[0] == 0x5A && data[1] == 0x00 && data[6] == 0x5A)
	{
		//5A 00 AA AA AA 02 58
		uint8_t cmd_link[]={0x5A,0x00,0xAA,0xAA,0xAA,0x02,0x58};
		if(ConfigMsg.debug>0)
			_Debug2(0, "Link test, L%d", __LINE__);
		send_secs_socket((uint8_t *)&cmd_link[0],sizeof(cmd_link));
	}
	else if(len == 7&& data[0] == 0x5A && data[1] == 0x04 && data[6] == 0x5E) // 
	{
		//resp:5A 04 00 00 00 D7 00 E4 02 19
		uint8_t cmd_link[]={0x5A,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
		uint8_t cmd_len=sizeof(cmd_link);

		uint16_t temper_10=(uint16_t)SHT21_ReadTemp()/10;
		uint16_t humidity_10=(uint16_t)SHT21_ReadHumidity()/10;
		//printf("temper_10:%d humidity_10:%d\n ",temper_10,humidity_10);
		cmd_link[4]=(uint8_t)(temper_10>>8);
		cmd_link[5]=(uint8_t)(temper_10&0xff);

		cmd_link[6]=(uint8_t)(humidity_10>>8);;
		cmd_link[7]=(uint8_t)(humidity_10&0xff);		
		uint16_t crc=amkor_nc01_crc(cmd_link,sizeof(cmd_link));		
		cmd_link[cmd_len-2]=(uint8_t)((crc>>8)&0xff);
		cmd_link[cmd_len-1]=(uint8_t)(crc&0xff);
		
		if(ConfigMsg.debug>0)
			_Debug2(0, "Get Humidity & temp, L%d", __LINE__);
		send_secs_socket((uint8_t *)&cmd_link[0],sizeof(cmd_link));
	}
	return 0;
}


int before_amkor_nc01_rs232_filter(uint8_t *data,int len)
{
	link_test_tick =0; //reset
	link_ok=1;

	// 5A 04 00 00 00 00 5E
	if (len == 7&& data[0] == 0x5A && data[1] == 0x00 && data[6] == 0x5A)
	{
		//5A 00 AA AA AA 02 58
		uint8_t cmd_link[]={0x5A,0x00,0xAA,0xAA,0xAA,0x02,0x58};
		if(ConfigMsg.debug>0)
			_Debug2(0, "rs232 Link test, L%d", __LINE__);
		
		//put_str_uart1((uint8_t *)&cmd_link[0],sizeof(cmd_link));
		myputstr((uint8_t *)&cmd_link[0],sizeof(cmd_link));
	}
	else if(len == 7&& data[0] == 0x5A && data[1] == 0x04 && data[6] == 0x5E) // 
	{
		//resp:5A 04 00 00 00 D7 00 E4 02 19
		uint8_t cmd_link[]={0x5A,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
		uint8_t cmd_len=sizeof(cmd_link);

		uint16_t temper_10=(uint16_t)SHT21_ReadTemp()/10;
		uint16_t humidity_10=(uint16_t)SHT21_ReadHumidity()/10;
		//printf("temper_10:%d humidity_10:%d\n ",temper_10,humidity_10);
		cmd_link[4]=(uint8_t)(temper_10>>8);
		cmd_link[5]=(uint8_t)(temper_10&0xff);

		cmd_link[6]=(uint8_t)(humidity_10>>8);;
		cmd_link[7]=(uint8_t)(humidity_10&0xff);		
		uint16_t crc=amkor_nc01_crc(cmd_link,sizeof(cmd_link));		
		cmd_link[cmd_len-2]=(uint8_t)((crc>>8)&0xff);
		cmd_link[cmd_len-1]=(uint8_t)(crc&0xff);
		
		if(ConfigMsg.debug>0)
			_Debug2(0, "rs232 Get Humidity & temp, L%d", __LINE__);
		//send_secs_socket((uint8_t *)&cmd_link[0],sizeof(cmd_link));
		myputstr((uint8_t *)&cmd_link[0],sizeof(cmd_link));
	}
	return 0;
}

void process_com1_com2_test(USERCMD *rxcmd,uint8_t type)
{
	uint8_t *data=rxcmd->data;
	uint16_t dlen=rxcmd->len;
	uint8_t info[64]={
		0xff,0x00,0x03,
		0x62,
		0x00,0x00,
		0x00,
		0x00,0x00,
		0x00,0x00
	};
	if(dlen>54)
	{
		dlen=54;
	}
	uint8_t act_len=dlen+3;
	info[1]=(uint8_t)(act_len>>8);
	info[2]=(uint8_t)(act_len&0xff);
	uint8_t tx_len=6+act_len;
	
	//FF	0003	61	Status	Uart num	Text	CRC
	switch(type)
	{
		case TYPE_COM1:
		{

			info[6]=1;
			memcpy(&info[7],data,dlen);
			uint16_t crc = Cal_Crc16_xmodem(info, tx_len - 2);
			info[tx_len-2]=(crc>>8)&0xff;
			info[tx_len-1]=(crc)&0xff;
			//tx
			send_tcp_socket(info,tx_len);
		}
		break;
		case TYPE_COM2:
		{
			info[6]=2;
			memcpy(&info[7],data,dlen);
			uint16_t crc = Cal_Crc16_xmodem(info, tx_len - 2);
			info[tx_len-2]=(crc>>8)&0xff;
			info[tx_len-1]=(crc)&0xff;
			//tx
			send_tcp_socket(info,tx_len);
		}
		break;
		default:
			printf("Uknow uart test :%02X\n",type);
		break;
	}
	return;
}


void process_rcv_secs_cmd(USERCMD *rxcmd)
{
	//process AT command
	uint8_t *data=rxcmd->data;
	uint8_t buf[128]={0};
	uint16_t len=(data[0]<<24)+(data[1]<<16)+
							(data[2]<<8)+data[3]+4;
	memcpy(buf,data,len);
//	printf("process_rcv_cmd:\n ");
//	for(int i=0;i<len;i++)
//	{
//		printf("%02X ",buf[i]);
//	}

	if(before_secs_filter(buf,len)>0)
	{
		return;
	}
	//_Debug2(0,"process_rcv_cmd:%d",len);
	//write to USART,uhf invenotry;
	//put_str_uart1((char *)data,len);
	return ;
}


void process_rcv_amkor_nc01_cmd(USERCMD *rxcmd)
{
	//process AT command
	uint8_t *data=rxcmd->data;
	uint8_t buf[128]={0};
	uint16_t len=rxcmd->len;
	memcpy(buf,data,len);
	printf("process_rcv_cmd:\n ");
	for(int i=0;i<len;i++)
	{
		printf("%02X ",buf[i]);
	}

	if(before_amkor_nc01_filter(buf,len)>0)
	{
		return;
	}

	return ;
}

void process_rcv_amkor_nc01_rs232_cmd(USERCMD *rxcmd)
{
	//process AT command
	uint8_t *data=rxcmd->data;
	uint8_t buf[128]={0};
	uint16_t len=rxcmd->len;
	memcpy(buf,data,len);
	printf("rs232 process_rcv_cmd: %s/%s/%d\n ",__FILE__, __FUNCTION__, __LINE__);
	for(int i=0;i<len;i++)
	{
		printf("%02X ",buf[i]);
	}

	if(before_amkor_nc01_rs232_filter(buf,len)>0)
	{
		return;
	}

	return ;
}

int elapsed_time(uint32_t start)
{

	uint32_t cur_time=ms_counter;
	if(cur_time>start)
	{
		return cur_time-start;
	}
	else
	{
		return 0xffffffff-start+cur_time;
	}
}

uint8_t IsDeviceValid(void)
{
  uint8_t array[11]={0};
	uint16_t crc16=0;
  Get_UUID(array,6);
  array[6]=ConfigMsg.batch;
  array[7]=(uint8_t)(ConfigMsg.sn>>8)&0xff;
  array[8]=(uint8_t)(ConfigMsg.sn)&0xff;
  array[9]=0xE8;	
  array[10]=0xCF;							
  crc16=generate_crc16(array,sizeof(array));
  if(ConfigMsg.key==crc16)
		 return 1;
	else 
		return 0;	
}



_lf_event gpi_trigger_event[GPI_MAX]={
	{-1,-1,0},
	{-1,-1,0},
	{-1,-1,0},
	{-1,-1,0}
};


void sent_gpi_status_out(uint8_t retport_type,uint8_t pin,uint8_t level)
{
	uint8_t info[11]={
		0xff,0x00,0x05,
		0x42,0x00,0x00,
		0,0,0,
		0x00,0x00
	};
	uint8_t len=sizeof(info);		
	info[6]=	pin;
	info[7]=level;
	info[8]=retport_type;
	uint16_t crc = Cal_Crc16_xmodem(info, len - 2);
	info[len-2]=(crc>>8)&0xff;
	info[len-1]=(crc)&0xff;
	//tx
	send_tcp_socket(info,len);
	return;
}


/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/

int main(void)
{
extern USERCMD_PACKETS_LIST usercmd_packets_list;
extern USERCMD_PACKETS_LIST eth_rx_packets_list;
extern uint8 update_flag ; 

	uint16_t swver=0;
	int time_watchdog=500;
	int watchdog_flag=0;
	uint32_t hb_rcv_elapse=0;//hb ????
	uint32_t hb_cur=0;

	USERCMD *pusercmd=NULL;
	u8 pkg_index=0;
	_gpio_event *p_gpio_event=NULL;

	SECS_S18F9_ENABLE=0;
	/* configure systick */
	systick_config();
	//vector offset
	nvic_configuration();
	if(RESET != rcu_flag_get(RCU_FLAG_WWDGTRST))
  {
		/* clear the WWDGTRST flag */
    rcu_all_reset_flag_clear();
		bIwdg_reset=1;
  }
  else
  {
			//?????????(???????????????)
		bIwdg_reset=0;
  }
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_AF);	// 
	gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
	//sht2x_power_init();
	Max7219_power_init();
	//set_rs485_direction(0);

	/* initilize the LEDs, USART and key */
	/*Led init*/
	led_init();
	gd_eval_com_init(EVAL_COM1);
	gd_eval_com_init(EVAL_COM0);
	init_rs485();
	confit_rs485(8,1,115200);
//	//uart2_init(115200);
//	//gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);	
//	set_rs485_direction(1);
	//
	//sht2x_power_on_off(1);


	sio_byte_buf_flush(&sio_rx_buf);
	sio_byte_buf_flush(&secs_rx_buf);
	for(int i=0;i<MAX_RX_USERCMD_PACKETS_LEN;i++)
	{
		usercmd_packets_list.Done[i]=0;
		eth_rx_packets_list.Done[i]=0;
	}

		/***/
	/* USART interrupt configuration */
	//usart_interrupt_enable(USART1, USART_INT_RBNE);
	usart_interrupt_enable(USART0, USART_INT_RBNE);
	usart_interrupt_enable(USART2, USART_INT_RBNE);
	nvic_irq_enable(USART0_IRQn, 0, 0);
	nvic_irq_enable(USART2_IRQn, 0, 2);
	/* print out the clock frequency of system, AHB, APB1 and APB2 */
	printf("\r\nCK_SYS is %d\r\n", rcu_clock_freq_get(CK_SYS));
//	printf("\r\nCK_AHB is %d", rcu_clock_freq_get(CK_AHB));
//	printf("\r\nCK_APB1 is %d", rcu_clock_freq_get(CK_APB1));
//	printf("\r\nCK_APB2 is %d", rcu_clock_freq_get(CK_APB2));
	//SHT21
	//SHT21_Init();

#ifdef SUPPORT_ETHERNET
	gpio_for_w5500_config();
	/***** ??W5500 *****/
	ConfigMsg.debug=1;

	reset_w5500();

#endif/*SUPPORT_ETHERNET*/
	
	/* USART interrupt configuration */
	/***** *****/
	get_config();
	printf("Parameters:%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d \n",	ConfigMsg.tmp_ctrl_floor,	ConfigMsg.tmp_ctrl_ceil,ConfigMsg.humidity_ctrl_floor,ConfigMsg.humidity_ctrl_ceil,
		ConfigMsg.tmp_warn_floor,ConfigMsg.tmp_warn_ceil,	ConfigMsg.humidity_warn_floor,ConfigMsg.humidity_warn_ceil,
		ConfigMsg.tmp_compensation,ConfigMsg.humidity_compensation,	ConfigMsg.open_beep_delay_time);
	magnetic_gate_beep_delay_time=ConfigMsg.open_beep_delay_time*1000;
   
	gp_io_init();

#ifdef SUPPORT_ETHERNET	
	
	swver=(ConfigMsg.sw_ver[0]<<8)|ConfigMsg.sw_ver[1];
	if(ConfigMsg.debug==1)
		printf("Version:%x\r\n",swver);
	if(swver!=FWSWver)
	{
		//save sw version
		ConfigMsg.sw_ver[0]=(FWSWver>>8);
		ConfigMsg.sw_ver[1]=(FWSWver&0xff);
		
		//set_w5500_ip();
		if(ConfigMsg.lip[0]==(uint8)0xff)
		{
			set_default();
		}
		else
			write_ver_to_flash();
//		write_ver_to_flash();//????????Flash
//		write_config_to_flash();
	}
	//
	load_nc01_parameters();
	if(ConfigMsg.lip[0]==(uint8)0xff)
	{
		set_default();
		printf("Set default network\n");

	}
	if(IsDeviceValid()==1)
		ValidDevice_ok=1;
	else
		ValidDevice_ok=0;
	/**W5500 IP*/
	set_network();

#endif/*SUPPORT_ETHERNET*/

	//Init Hardware watchdog
	wdt_hw_int();

	/***** W5500?IP????? *****/
	if(bIwdg_reset==1)
	{
		printf("iwdg reset\n");
	}
	//intf("%s: debug:%x\n", __func__,ConfigMsg.debug);
	printf("\r\nBuild at %s  %s\r\n",__DATE__,__TIME__);	
	reset_gpi_filter(0);
	reset_gpi_filter(1);
	reset_gpi_filter(2);
	reset_gpi_filter(3);
	reset_gpi_filter(4);
	reset_gpi_filter(5);
	reset_gpi_filter(6);
	reset_gpi_filter(7);
	wdt_hw_enable();
	TIM4_Configuration();
	keypad_init();
	cc_times=0;	

	
	Init_MAX7219();		
	SHT21_Init();
	//
	while(1){

		//normal use tcp
		if( update_flag==0) 
			do_attcp_cmd();

		//secs tcp//amkor nc 01
		do_secstcp_cmd();
		
		if((p_run_counter%2)==0)
			do_udp_config();
		if((p_run_counter%6)==0)
		{
			do_fw_update();
			if( update_flag==1) 
				wdt_hw_disable();

		}
		
		p_run_counter++;

		pusercmd= usercmd_deque(&pkg_index );
		if(pusercmd!=NULL)
		{
			//uint8_t *data=pusercmd->data;
			//uint16_t len=(data[5]<<8)|(data[6])+9;
			switch(pusercmd->type)
			{
				case TYPE_SECS://secs
					process_rcv_secs_cmd(pusercmd);
					break;
				case TYPE_AMKOR_NC01:
					process_rcv_amkor_nc01_cmd(pusercmd);
					break;
			  case TYPE_AMKOR_NC01_RS232:
					process_rcv_amkor_nc01_rs232_cmd(pusercmd);
					break;
				case TYPE_COM1://for test
				case TYPE_COM2:
					process_com1_com2_test(pusercmd,pusercmd->type);
					break;
				default:
					printf("None define type\n");
					break;
			}

			usercmd_packets_list.Done[pkg_index]=0;
		}
		
		//gpio event
		if((p_run_counter%10)==1)
		{
			p_gpio_event= gio_event_deque(&pkg_index );
			if(p_gpio_event!=NULL)
			{
				extern _gpio_event_list gpio_event_list;
				if((p_gpio_event->type>-1)&&(p_gpio_event->retport_type>-1))
				{
					static uint32_t trigger_sta=0;

					uint8_t info[11]={
						0xff,0x00,0x05,
						0x42,0x00,0x00,
						0,0,0,
						0x00,0x00
					};
					uint8_t len=sizeof(info);		
					info[6]=	p_gpio_event->pin;
					info[7]=p_gpio_event->level;
					info[8]=p_gpio_event->retport_type;
					uint16_t crc = Cal_Crc16_xmodem(info, len - 2);
					info[len-2]=(crc>>8)&0xff;
					info[len-1]=(crc)&0xff;
					//tx
					send_tcp_socket(info,len);
					if(trigger_sta==0){
						trigger_sta=p_gpio_event->msta;
					}
				}
				gpio_event_list.Done[pkg_index]=0;
			}			
			
		}

		//process error add here
		//printf("*");
		// watch dog 
		if(time_watchdog == 0)
		{
			time_watchdog = 500; // loop 500, change watch dog flag
			
			if(hw_wdt_test==0)
			{
				//printf("-");
				if(watchdog_flag == 1) 
				{	
					watchdog_flag = 2;
					wdt_hw_set_wdi(0);
				}
				else
				{
					watchdog_flag = 1;
					wdt_hw_set_wdi(1);
				}
			}
		}

		time_watchdog--;	
		//
		hb_cur=ms_counter;
		if(hb_sta<=hb_cur)
			hb_rcv_elapse=hb_cur-hb_sta;
		else
			hb_rcv_elapse=MAX_TICK+hb_cur-hb_sta;
		if((hb_rcv_elapse>17000)&&(hb_enable==1))
		{
        //close socket
			  close_atsocket();
				hb_enable=0;
				hb_sta=ms_counter;
				printf("time out hb_enable:%d,%d %d \n",hb_enable,hb_cur,hb_sta);
				
		}
		else if(hb_rcv_elapse>5000)//check wheather cale is Unplugged
		{
			//checkNetCable();
		}

	}
}


/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM1, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM1, USART_FLAG_TBE));

    return ch;
}

int myputc(int ch)
{
    usart_data_transmit(USART0, (uint8_t)ch);
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));

    return ch;
}

int myputstr(uint8_t *buf,int len)
{
	int i=0;
	while(i<len)
	{
		myputc(buf[i]);
		i++;
	}
	return 0;
}

void _Debug2(int type, char *mess, ...)
{
	int		len = 0;
	char	buf[256] = {0};
	char	buf2[256] = {0};
	va_list	ptr;
	
	if(type != 9 && debug_flag != 0) return;

	va_start(ptr, mess);
	vsprintf(buf, mess, ptr);
	va_end(ptr);
	if(type == 0)
		len = sprintf(buf2, "[DEBUG]--> %s", buf);
	else if(type == 1)
		len = sprintf(buf2, "[ERR]--> %s", buf);
	else if(type == 9)
	{
		len = sprintf(buf2, "[DEBUG]--> %s", buf);
	}
	else
	{
		len = sprintf(buf2, "%s", buf);
	}
	
	// print debuf log to UART2
	//_UART_SendByteArray(USART2, (unsigned char*)buf2, len);
	printf("%s",buf2);
}

void put_test_data_queue(uint8_t *data,int len,uint8_t type)
{
	USERCMD ucmd;
	ucmd.type=type;
	ucmd.len=len;
	memcpy((char *)ucmd.data,data,len);
  usercmd_enque(&ucmd,len);
	return;
}

//COM1: DEBUG
void USART1_IRQHandler(void)
{
  uint32_t inbyte;
	//printf("USART1");
  /* RXNE handler */
	if(RESET != usart_interrupt_flag_get(USART1, USART_INT_FLAG_RBNE))
  {
    usart_interrupt_flag_clear(USART1,USART_INT_FLAG_RBNE);
		inbyte =usart_data_receive(USART1);//USART_ReceiveData(USART0);
		//_Debug2(0,"%02X",inbyte);

		//for uart test
		if(uart_test_flag==1)
		{
			put_test_data_queue((uint8_t*)&inbyte,1,TYPE_COM1);
		}
	
	}
	else if(usart_interrupt_flag_get(USART1,USART_INT_FLAG_RBNE_ORERR) == SET){ 
		usart_interrupt_flag_clear(USART1,USART_INT_FLAG_RBNE_ORERR);
		//printf("ORE");
  }
	else
	{
		static int print_counter=0;
		print_counter++;
		//USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		usart_interrupt_flag_clear(USART1,USART_INT_FLAG_RBNE_ORERR);
	}
	
}

//
//RS232 -- JY-DAM
// BR:9600
//
void USART0_IRQHandler(void)
{
  uint32_t inbyte;
	//printf("USART0 -%d\n", __LINE__);
	
  /* RXNE handler */
	if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE))
  {
    usart_interrupt_flag_clear(USART0,USART_INT_FLAG_RBNE);
		inbyte =usart_data_receive(USART0);//USART_ReceiveData(USART0);
		//_Debug2(0,"%02X ,SIO_BYTE_BUF_MAX:%d\n",inbyte, SIO_BYTE_BUF_MAX);

		//for uart test
//		if(uart_test_flag==1)
//		{
//			put_test_data_queue((uint8_t*)&inbyte,1,TYPE_COM1);
//		}
		if ( rs232_rx_buf_console.len  >= (SIO_BYTE_BUF_MAX - 1) ) 
    {
      rs232_rx_buf_console.len = 0;
			
			return;
    }
		
//    if ( (rs232_rx_buf_console.len == 0 ) && (inbyte!=0x5A) ) 
//		{
//			//_Debug2(0,"Invalid:%02X",inbyte);
//		  return;
//		}
		
		sio_byte_buf_enqueue(&rs232_rx_buf_console, inbyte);

		//
		//uint8_t* 	JY_DAM_A2_RESP[64];
		//uint8			JY_DAM_A2_RESP_LENGTH = 37;	
		//
    if(rs232_rx_buf_console.len == JY_DAM_A2_RESP_LENGTH)//\r\n
	  {
			//uint8_t pl_len=37;
	
			//if(rs232_rx_buf_console.len==pl_len)
			//{
				
				//rs232_post_received_data(&rs232_rx_buf_console.data[0],rs232_rx_buf_console.len);
				//rs232_rx_buf_console.len=0;
			
				printf("JY_DAM_A2_RESP: \n");
				memcpy(JY_DAM_A2_RESP, &rs232_rx_buf_console.data[0], JY_DAM_A2_RESP_LENGTH);
				for(int i=0;i<JY_DAM_A2_RESP_LENGTH;i++)
				{
					printf("%02X ",JY_DAM_A2_RESP[i]);
				}
				rs232_rx_buf_console.len=0;
				//pl_len=0;
				
			//}
    }
	}
	else if(usart_interrupt_flag_get(USART0,USART_INT_FLAG_RBNE_ORERR) == SET){ 
		usart_interrupt_flag_clear(USART0,USART_INT_FLAG_RBNE_ORERR);
		//printf("ORE");
  }
	else
	{
		static int print_counter=0;
		print_counter++;
		//USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		usart_interrupt_flag_clear(USART0,USART_INT_FLAG_RBNE_ORERR);
	}
	
}

