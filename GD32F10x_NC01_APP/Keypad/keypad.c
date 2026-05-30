#include "keypad.h"
#include "w5500_conf.h"
#include "max7219.h"
#include "utility.h"

/*FNx_par fnx_par[OP_MAX]={
	{OP00,200,200,1},
	{OP01,240,240,1},
	{OP02,230,230,1},
	{OP03,240,240,1},
	{OP04,190,190,1},
	{OP05,250,250,1},
	{OP06,200,200,1},
	{OP07,270,270,1},
	{OP08,0,0,1},
	{OP09,0,0,1},
	{OP10,30,30,1}
};*/

FNx_par fnx_par[OP_MAX]={
	{OP00,200,200,1},
	{OP01,240,240,1},
	{OP02,750,750,1},
	{OP03,820,820,1},
	{OP04,280,280,1},
	{OP05,300,300,1},
	{OP06,800,800,1},
	{OP07,850,850,1},
	{OP08,0,0,1},
	{OP09,0,0,1},
	{OP10,30,30,1}
};

static keypad_OP  keypad_op;
//1:setting;0:
static uint8_t normal_op=0;
static  uint8_t bFlash=0;
void load_nc01_parameters(void)
{
	if((ConfigMsg.tmp_ctrl_floor==-1)||(ConfigMsg.tmp_ctrl_ceil==-1)||
		(ConfigMsg.humidity_ctrl_floor==-1)||(ConfigMsg.humidity_ctrl_ceil==-1)||
		(ConfigMsg.tmp_warn_floor==-1)||(ConfigMsg.tmp_warn_ceil==-1)||
		(ConfigMsg.humidity_warn_floor==-1)||(ConfigMsg.humidity_warn_ceil==-1)
	)
    {
		ConfigMsg.tmp_ctrl_floor=fnx_par[OP00].init_value;
		ConfigMsg.tmp_ctrl_ceil=fnx_par[OP01].init_value;
		ConfigMsg.humidity_ctrl_floor=fnx_par[OP02].init_value;
		ConfigMsg.humidity_ctrl_ceil=fnx_par[OP03].init_value;
		
		ConfigMsg.tmp_warn_floor=fnx_par[OP04].init_value;
		ConfigMsg.tmp_warn_ceil=fnx_par[OP05].init_value;
		ConfigMsg.humidity_warn_floor=fnx_par[OP06].init_value;
		ConfigMsg.humidity_warn_ceil=fnx_par[OP07].init_value;
		
		ConfigMsg.tmp_compensation=fnx_par[OP08].init_value;
		ConfigMsg.humidity_compensation=fnx_par[OP09].init_value;
		ConfigMsg.open_beep_delay_time=fnx_par[OP10].init_value;
		return;
	}
	fnx_par[OP00].modified_val=ConfigMsg.tmp_ctrl_floor;
	fnx_par[OP00].init_value=fnx_par[OP00].modified_val;
	fnx_par[OP01].modified_val=ConfigMsg.tmp_ctrl_ceil;//temperature control ceil
	fnx_par[OP01].init_value=fnx_par[OP01].modified_val;
	fnx_par[OP02].modified_val=ConfigMsg.humidity_ctrl_floor;//humidity control floor
	fnx_par[OP02].init_value=fnx_par[OP02].modified_val;
	fnx_par[OP03].modified_val=ConfigMsg.humidity_ctrl_ceil;//humidity control ceil
	fnx_par[OP03].init_value=fnx_par[OP03].modified_val;
	//warn floor and ceil
	fnx_par[OP04].modified_val=ConfigMsg.tmp_warn_floor;//temperature warn floor
	fnx_par[OP04].init_value=fnx_par[OP04].modified_val;
	fnx_par[OP05].modified_val=ConfigMsg.tmp_warn_ceil;//temperature warn ceil
	fnx_par[OP05].init_value=fnx_par[OP05].modified_val;
	fnx_par[OP06].modified_val=ConfigMsg.humidity_warn_floor;//humidity warn floor
	fnx_par[OP06].init_value=fnx_par[OP06].modified_val;
	fnx_par[OP07].modified_val=ConfigMsg.humidity_warn_ceil;//humidity warn ceil
	fnx_par[OP07].init_value=fnx_par[OP07].modified_val;
	//compensate
	fnx_par[OP08].modified_val=ConfigMsg.tmp_compensation;//temperature compensation
	fnx_par[OP08].init_value=fnx_par[OP08].modified_val;
	fnx_par[OP09].modified_val=ConfigMsg.humidity_compensation;//humidity compensation
	fnx_par[OP09].init_value=fnx_par[OP09].modified_val;
	//
	fnx_par[OP10].modified_val=ConfigMsg.open_beep_delay_time;//entrance and beep delay time
	fnx_par[OP10].init_value=fnx_par[OP10].modified_val;
	return;
}

void keypad_init()
{
	keypad_op.cur_mode=OP00;
	keypad_op.pFnxpar=&fnx_par[keypad_op.cur_mode];
//			Write_Max7219(8,0);
//			Write_Max7219(7,0xE);//p		
//			Write_Max7219(6,0);	
//			Write_Max7219(5,0);
	normal_op=1;
	return;
}
//1: normal mode; 0:setting mode
int8_t is_normal_mode(){
	if(normal_op>0)
		return 1;
	else
		return 0;
}
/**
*|FN|Add|Dec|bit position|Save|
* Description: handle keypad event
* key_id:key id
* pressed:
*		0-pressed;
*		1:released
* return:
*		1:update 8-LED
*		0:need not update 8-LED
*   -1:invalid key id
*/
uint8_t keypad_process(uint8_t key_id,uint8_t pressed)
{
	int8_t status;
	printf("%s normal_op:%d key_id:%d \n",__func__,normal_op,key_id);
	switch(key_id)
	{
		case 0:
		{			
			if(normal_op==1)
			{
				normal_op=0;
				keypad_op.cur_mode=OP00;
			}
			else
			{
				//keypad_op.cur_mode=(++keypad_op.cur_mode)%OP_MAX;  // 2026-5-30
				int next_mode = (keypad_op.cur_mode + 1) % OP_MAX;	// 2026-5-30
				keypad_op.cur_mode = (MODE_FN)next_mode;						// 2026-5-30 by steven

			}
				
			
			keypad_op.pFnxpar=&fnx_par[keypad_op.cur_mode%OP_MAX];
			status=1;
			//static uint32_t num=0;
			uint32_t v=keypad_op.cur_mode%10;

			Write_Max7219(8,0);
			Write_Max7219(7,0xE);//p
			v=(keypad_op.cur_mode/10)%10;				
			Write_Max7219(6,v);
			v=keypad_op.cur_mode%10;		
			Write_Max7219(5,v);

			uint32_t val=keypad_op.pFnxpar->init_value&0xfff;
			v=val%10;
			Write_Max7219(1,v);
			v=(val/10)%10;
			if(keypad_op.cur_mode!=OP10)
				v=v|0x80;
			Write_Max7219(2,v);//Dot
			v=(val/100)%10;
			Write_Max7219(3,v);	
			v=(val/1000)%10;
			if(v==0)
				Write_Max7219(4,0xf);
			else
				Write_Max7219(4,v);
			bFlash=0;
		}
		break;
		
		case 1:
		{
			if(normal_op==0){
				uint8_t one=keypad_op.pFnxpar->modified_val%10;
				uint8_t ten=(keypad_op.pFnxpar->modified_val/10)%10;
				uint8_t hundred=(keypad_op.pFnxpar->modified_val/100)%10;
				uint8_t thound=(keypad_op.pFnxpar->modified_val/1000)%10;
				//keypad_op.pFnxpar->modified_val=(keypad_op.pFnxpar->modified_val+keypad_op.pFnxpar->bitweight)%1000;
				if(keypad_op.pFnxpar->bitweight==1)
				{
					++one;
					one%=10;
				}
				else if(keypad_op.pFnxpar->bitweight==10)
				{
					++ten;
					ten%=10;
				}
				else if(keypad_op.pFnxpar->bitweight==100)
				{
					++hundred;
					hundred%=10;
				}
				else if(keypad_op.pFnxpar->bitweight==1000)
				{
					++thound;
					thound%=10;
				}	
				keypad_op.pFnxpar->modified_val=thound*1000+hundred*100+ten*10+one;
				status=1;
				printf("%d modified_val:%d \n",__LINE__,keypad_op.pFnxpar->modified_val);
			}
		}
			break;
		case 2:
		{
			if(normal_op==0){
				//keypad_op.pFnxpar->modified_val=(keypad_op.pFnxpar->modified_val-keypad_op.pFnxpar->bitweight)%1000;
				uint8_t one=keypad_op.pFnxpar->modified_val%10;
				uint8_t ten=(keypad_op.pFnxpar->modified_val/10)%10;
				uint8_t hundred=(keypad_op.pFnxpar->modified_val/100)%10;
				uint8_t thound=(keypad_op.pFnxpar->modified_val/1000)%10;
				if(keypad_op.pFnxpar->bitweight==1)
				{
					(one==0)?one=9:one--;
				}
				else if(keypad_op.pFnxpar->bitweight==10)
				{
					(ten==0)?ten=9:ten--;
				}
				else if(keypad_op.pFnxpar->bitweight==100)
				{
					(hundred==0)?hundred=9:hundred--;

				}
				else if(keypad_op.pFnxpar->bitweight==1000)
				{
					(thound==0)?thound=9:thound--;
				}	
				keypad_op.pFnxpar->modified_val=thound*1000+hundred*100+ten*10+one;
				status=1;
				printf("%d modified_val:%d \n",__LINE__,keypad_op.pFnxpar->modified_val);
			}
		}
			break;
		case 3: 
		{
			if(normal_op==0){
				if(bFlash==0)
				{
					bFlash=1;
					break;
				}
				keypad_op.pFnxpar->bitweight=keypad_op.pFnxpar->bitweight*10;
				if(keypad_op.pFnxpar->bitweight==1000)
					keypad_op.pFnxpar->bitweight=1;
				status=1;
				printf("keypad_op.pFnxpar->bitweight:%d \n",keypad_op.pFnxpar->bitweight);
			}
		}
			break;
		case 4:
		{
			if(normal_op==0){
				extern CONFIG_MSG  ConfigMsg,RecvMsg;	
				uint8_t need_update=0;
				for(uint8_t op=0;op<OP_MAX;op++)
				{
					if(fnx_par[op].modified_val!=fnx_par[op].init_value)
					{
						need_update=1;
						break;
					}
				}
				printf("%s need_update=%d\n",__func__,need_update);
				if(need_update==1)
				{
					extern uint32_t __IO magnetic_gate_beep_delay_time;
					//save data
					ConfigMsg.tmp_ctrl_floor=fnx_par[OP00].modified_val;
					fnx_par[OP00].init_value=fnx_par[OP00].modified_val;
					ConfigMsg.tmp_ctrl_ceil=fnx_par[OP01].modified_val;//temperature control ceil
					fnx_par[OP01].init_value=fnx_par[OP01].modified_val;
					ConfigMsg.humidity_ctrl_floor=fnx_par[OP02].modified_val;//humidity control floor
					fnx_par[OP02].init_value=fnx_par[OP02].modified_val;
					ConfigMsg.humidity_ctrl_ceil=fnx_par[OP03].modified_val;//humidity control ceil
					fnx_par[OP03].init_value=fnx_par[OP03].modified_val;
					//warn floor and ceil
					ConfigMsg.tmp_warn_floor=fnx_par[OP04].modified_val;//temperature warn floor
					fnx_par[OP04].init_value=fnx_par[OP04].modified_val;
					ConfigMsg.tmp_warn_ceil=fnx_par[OP05].modified_val;//temperature warn ceil
					fnx_par[OP05].init_value=fnx_par[OP05].modified_val;
					ConfigMsg.humidity_warn_floor=fnx_par[OP06].modified_val;//humidity warn floor
					fnx_par[OP06].init_value=fnx_par[OP06].modified_val;
					ConfigMsg.humidity_warn_ceil=fnx_par[OP07].modified_val;//humidity warn ceil
					fnx_par[OP07].init_value=fnx_par[OP07].modified_val;
					//compensate
					ConfigMsg.tmp_compensation=fnx_par[OP08].modified_val;//temperature compensation
					fnx_par[OP08].init_value=fnx_par[OP08].modified_val;
					ConfigMsg.humidity_compensation=fnx_par[OP09].modified_val;//humidity compensation
					fnx_par[OP09].init_value=fnx_par[OP09].modified_val;
					//
					ConfigMsg.open_beep_delay_time=fnx_par[OP10].modified_val;//entrance and beep delay time
					magnetic_gate_beep_delay_time=		ConfigMsg.open_beep_delay_time*1000;
					fnx_par[OP10].init_value=fnx_par[OP10].modified_val;
					write_config_to_flash();//将配置信息写入到Flash
					delay_ms(80);
					status=0;			
					normal_op=1;
					bFlash=0;
				}
				else
				{
					status=0;			
					normal_op=1;
					bFlash=0;
				}
			}
		}
			break;

		default:
		{
			
			printf("%s invalid key,key_id:%d, %d\n",__func__,key_id,pressed);
			status=-1;
		}
			break;
		
	}
	return status;
}

void flash_one_8_seg(void )
{
	static uint8_t toggle=0;
	if(bFlash==1)
	{
			keypad_op.pFnxpar=&fnx_par[keypad_op.cur_mode%OP_MAX];
			switch(keypad_op.pFnxpar->bitweight)
			{
				case 1:
				{
					uint32_t val=keypad_op.pFnxpar->modified_val&0xfff;
					uint8_t v=val%10;
					if(toggle==1)
						Write_Max7219(1,v);
					else
						Write_Max7219(1,0xf);
					v=(val/10)%10;
					if(keypad_op.cur_mode!=OP10)
						v=v|0x80;
					Write_Max7219(2,v);//Dot
					v=(val/100)%10;
					Write_Max7219(3,v);

					if(toggle==1)
						toggle=0;
					else
						toggle=1;
					//printf("%s toggle=%d \n",__func__,toggle);
				}
					break;
				case 10:
				{
					uint32_t val=keypad_op.pFnxpar->modified_val&0xfff;
					uint8_t v=val%10;
					Write_Max7219(1,v);
					v=(val/10)%10;
					if(toggle==1){
						if(keypad_op.cur_mode!=OP10)
							v=v|0x80;
						Write_Max7219(2,v);
					}
					else
						Write_Max7219(2,0xf);
					v=(val/100)%10;
					Write_Max7219(3,v);
					if(toggle==1)
						toggle=0;
					else
						toggle=1;
				}
					break;
				case 100:
				{
					uint32_t val=keypad_op.pFnxpar->modified_val&0xfff;
					uint8_t v=val%10;
					Write_Max7219(1,v);
					v=(val/10)%10;
					if(keypad_op.cur_mode!=OP10)
						v=v|0x80;
					Write_Max7219(2,v);
					
					v=(val/100)%10;
					if(toggle==1)
						Write_Max7219(3,v);
					else
						Write_Max7219(3,0xf);
					if(toggle==1)
						toggle=0;
					else
						toggle=1;
				}
					break;
				default:
				{
					printf("Ivalid bieweight :%s\n",__func__);
				}
					break;
			}
		
	}
	return;
}
