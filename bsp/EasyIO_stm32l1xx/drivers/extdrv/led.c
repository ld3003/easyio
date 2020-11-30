#include "led.h"
#include <stm32l1xx.h>
#include <rtthread.h>
#include "app_timer.h"
#include "my_stdc_func/debugl.h"
#include "gps_tracker.h"
#include "stabdby.h"

static struct rt_timer flicker_led_tmr;
//static struct rt_timer key_tmr;
static unsigned char atom_sem = 0;

//PB234
void init_led_hw(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	memset(&GPIO_InitStructure,0x0,sizeof(GPIO_InitStructure));
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD,ENABLE);
	

	//LED1 红
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	//GPIO_ResetBits(GPIOD,GPIO_Pin_2);
	
	//LED2 蓝
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//LED3 绿
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//AMP-EN
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//GPIO_SetBits(GPIOB,GPIO_Pin_1);
	
	//PWR KEY
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_400KHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//FUNC KEY
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	
	//PWR KEY 5vDET
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	

	
	//PWR EN
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
	//GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	#if 0
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC,GPIO_Pin_5);
	#endif
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	
	led_off(0);
	led_off(1);
	led_off(2);

}

uint8_t __is_charge(void)
{
	uint8_t ret = 0;
	
	static short ms100 = 0;
	
	GPIO_InitTypeDef GPIO_InitStructure;
	memset(&GPIO_InitStructure,0x0,sizeof(GPIO_InitStructure));
	
	if (ret <= 0)
	{
			//PWR KEY 5vDET
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}else{
		ret --;
	}
	
	ret = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_12);
	
	//如果检测为正在充电，且delay时间为0，那么冲洗设置delay时间
	if (ret > 0)
	{
		if (ms100 == 0)
			ms100 = 50;
				//PWR KEY 5vDET
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIO_ResetBits(GPIOA,GPIO_Pin_12);
	}
	
	return ret;
}

void __disable_charge(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	memset(&GPIO_InitStructure,0x0,sizeof(GPIO_InitStructure));
	
		//PWR KEY 5vDET
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_12);
	
//		//PWR KEY 5vDET
//	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
//	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	
//	//return GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_12);
	
	//
}


#define LED_FAST_FLICKER(EX) EX.mode=0;if(EX.mode>1){EX.mode=0;};EX.busy_ms_max=200;EX.idle_ms_max=200;
#define LED_SLOW_FLICKER(EX) EX.mode=0;if(EX.mode>1){EX.mode=0;};EX.busy_ms_max=200;EX.idle_ms_max=1000;

unsigned char disable_RGB_led = 0;


struct LED_FLICKER led_flicker[3];

void update_led_mod(void)
{
	

	switch(ledstatus[LED_R])
	{
		case RGB_LED_OFF:
			led_flicker[LED_R].mode = 4;
			break;
		case LED_STATUS_ON:
		case RED_STATUS_ON:
			led_flicker[LED_R].mode = 3;
			break;
		case LED_STATUS_FAST:
		case RED_STATUS_CHARGE:
			LED_FAST_FLICKER(led_flicker[LED_R]);
			break;
		default:
			break;
		//
	}
	
	switch(ledstatus[LED_G])
	{
		case LED_STATUS_ON:
			led_flicker[LED_G].mode = 3;
			break;
		case RGB_LED_OFF:
			led_flicker[LED_G].mode = 4;
			break;
		case GREEN_STATUS_GPSOFF:
			led_flicker[LED_G].mode = 4;
			break;
		case GREEN_STATUS_GPSV:
			LED_SLOW_FLICKER(led_flicker[LED_G]);
			break;
		case LED_STATUS_FAST:
		case GREEN_STATUS_GPSA:
			//LED_FAST_FLICKER(led_flicker[LED_G]);
			led_flicker[LED_G].mode = 3;
			break;
		default:
			break;
		//
		//
	}
	
	switch(ledstatus[LED_B])
	{
		
		case RGB_LED_OFF:
			led_flicker[LED_B].mode = 4;
			break;
		
		case BLUE_STATUS_NOSIMCARD:
			led_flicker[LED_B].mode = 4;
			break;
		case BLUE_STATUS_REGISTER:
			LED_SLOW_FLICKER(led_flicker[LED_B]);
			break;
		case LED_STATUS_FAST:
		case BLUE_STATUS_LOGIN:
			LED_FAST_FLICKER(led_flicker[LED_B]);
			break;
		case LED_STATUS_ON:
		case BLUE_STATUS_SUCCESS:
			led_flicker[LED_B].mode = 3;
			break;
		
		
		default:
			break;
		//
		//
	}
	//
}

static unsigned int func_key_timer = 0;

unsigned int pwr_press_time = 0;
unsigned int func_press_time = 0;
unsigned int charge_disable_tim = 0;

enum {
	FUN_KEY_STATUS_RELEASE,
	FUN_KEY_STATUS_PRESS,
	FUN_KEY_STATUS_LONGPRESS,
	FUN_KEY_STATUS_LONGRELEASE,
};
unsigned char FUN_KEY_STATUS = FUN_KEY_STATUS_RELEASE;

static void flick_led_tmr_timeout(void * arg)
{
	int i=0;
	
	static unsigned char flag_power_key_press_release = 0;

	
	//下面是新的LED控制规则
	for(i=0;i<3;i++)
	{
		switch(led_flicker[i].mode)
		{
			case 0:
				
				led_off(i);
				if (led_flicker[i].idle_ms_max == 0)
					break;
				led_flicker[i].idle_ms += 50;
				if (led_flicker[i].idle_ms >= led_flicker[i].idle_ms_max)
				{
					led_flicker[i].idle_ms = 0;
					led_flicker[i].mode = 1;
				}
				break;
			case 1:
				led_on(i);
				if (led_flicker[i].busy_ms_max == 0)
					break;
				led_flicker[i].busy_ms += 50;
				if (led_flicker[i].busy_ms >= led_flicker[i].busy_ms_max)
				{
					led_flicker[i].busy_ms = 0;
					led_flicker[i].mode = 0;
				}
				break;
				
			case 3:
				led_on(i);
				break;
			case 4:
				led_off(i);
				break;
			default:
				break;
		}
	}

	START_PROCESS_KEY:

	#define LONE_PRESS_SECOND 5
	

	switch(FUN_KEY_STATUS)
	{
		case FUN_KEY_STATUS_RELEASE:
			if (FUNC_KEY)
			{
				func_press_time = app_timer_data.app_timer_second;
				FUN_KEY_STATUS = FUN_KEY_STATUS_PRESS;
				func_key_press(0);
			}
			break;
		case FUN_KEY_STATUS_PRESS:
			if (FUNC_KEY)
			{
				if ((app_timer_data.app_timer_second - func_press_time) > LONE_PRESS_SECOND)
				{
					FUN_KEY_STATUS = FUN_KEY_STATUS_LONGPRESS;
					//
				}
			}else{
				FUN_KEY_STATUS = FUN_KEY_STATUS_RELEASE;
				func_key_release(0);
			}
			break;
			
		case FUN_KEY_STATUS_LONGPRESS:
			if (FUNC_KEY)
			{}
			else
			{
				FUN_KEY_STATUS = FUN_KEY_STATUS_LONGRELEASE;
				func_key_long_release(0);
			}
			break;
		case FUN_KEY_STATUS_LONGRELEASE:
			FUN_KEY_STATUS = FUN_KEY_STATUS_RELEASE;
			break;
		default:
			break;
	}
	
	
	if (POWER_KEY)
	{
		
		
		
		if (flag_power_key_press_release == 0)
		{
			pwr_press_time = app_timer_data.app_timer_second;
			flag_power_key_press_release = 1;
			pwr_key_press(0);
		}
		
		
		if ((app_timer_data.app_timer_second - pwr_press_time) >= LONE_PRESS_SECOND)
		{
			pwr_key_long_press(0);
		}
		
	}else
	{
		
		if (flag_power_key_press_release == 1)
		{
			flag_power_key_press_release = 0;
			
			if ((app_timer_data.app_timer_second - pwr_press_time) <= 1)
			{
				pwr_key_release(0);
			}
			else if ((app_timer_data.app_timer_second - pwr_press_time) >= LONE_PRESS_SECOND)
			{
				pwr_key_long_release(0);
				//
			}
		}
		
	}
	
	return;
	
	//
}

//START*********************************************************************************
#include "sosapp.h"
void func_key_press(void *p){
	DEBUGL->debug("PWR KEY PRESS !!!\r\n");
}
void func_key_release(void *p){
	DEBUGL->debug("PWR KEY RELEASE !!!\r\n");
}
void func_key_long_press(void *p){
	DEBUGL->debug("PWR KEY L_PRESS !!!\r\n");
	//开始进入SOS 状态
	if (sosrouting_status == SOSROUTING_STATUS_INIT)
	{
		sosrouting_status = SOSROUTING_STATUS_START;
	}
}
void func_key_long_release(void *p){
	DEBUGL->debug("PWR KEY L_RELEASE !!!\r\n");
}

void pwr_key_press(void *p)
{
	DEBUGL->debug("PWR KEY PRESS !!!\r\n");
	//
}
void pwr_key_release(void *p)
{
	DEBUGL->debug("PWR KEY RELEASE !!!\r\n");
	//
}

void pwr_key_long_press(void *p)
{
	DEBUGL->debug("PWR KEY LONG PRESS !!!\r\n");
	#ifndef BOOTLOADER
	
	if (disable_RGB_led == 1)
		return ;
	
	//不同模式下对长安建的处理
	switch(tracker_private_data.SYSTEM_STATUS)
	{
		case SYSTEM_POWER_ON:
			break;
		case SYSTEM_POWER_OFF:
			break;
		case SYSTEM_CONFIG_MOD:
			break;
		default:
			break;
	}
	
	if (FUNC_KEY)
	{
		tracker_private_data.test_flag = 1;
		//
	}
	
	if (tracker_private_data.pre_poweroff_on != 1)
	{
		tracker_private_data.pre_poweroff_on = 1;
		tracker_private_data.pre_poweroff_on_time = app_timer_data.app_timer_second;
		logout_xmpp();
	}
	#endif
	
	//
}
void pwr_key_long_release(void *p)
{
	DEBUGL->debug("PWR KEY LONG RELEASE !!!\r\n");
	
	if (disable_RGB_led == 1)
		return ;
	
	#ifndef BOOTLOADER
	tracker_private_data.pre_poweroff_on = 2;
	#endif
	//
}

//END********************************************************************************************************
  
void init_flicker_led_tmr(void)
{
	memset(&led_flicker,0x0,sizeof(led_flicker));
	//100ms
	
	rt_timer_init(&flicker_led_tmr,"fl_tmr",flick_led_tmr_timeout,0,RT_TICK_PER_SECOND/20,RT_TIMER_FLAG_PERIODIC);
  rt_timer_start(&flicker_led_tmr);
	//
}

void mot_on(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	memset(&GPIO_InitStructure,0x0,sizeof(GPIO_InitStructure));
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC,GPIO_Pin_9);
}

void mot_off(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	memset(&GPIO_InitStructure,0x0,sizeof(GPIO_InitStructure));
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOC,GPIO_Pin_9);
}

void led_on(unsigned char index)
{
	#ifdef DISABLE_LED
	return;
	//关闭LED
	#endif
	
	if (disable_RGB_led == 1)
	{
		
				led_off(0);
				led_off(1);
				led_off(2);
		
				return ;
		
	}
	
	
	
	switch(index)
	{
		case 0:
		{
			GPIO_InitTypeDef GPIO_InitStructure;
			memset(&GPIO_InitStructure,0x0,sizeof(GPIO_InitStructure));
			GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
			GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
			GPIO_Init(GPIOD, &GPIO_InitStructure);
			GPIO_ResetBits(GPIOD,GPIO_Pin_2);
			//
			break;
		}
		case 1:
		{
			GPIO_InitTypeDef GPIO_InitStructure;
			memset(&GPIO_InitStructure,0x0,sizeof(GPIO_InitStructure));
			GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
			GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
			GPIO_Init(GPIOB, &GPIO_InitStructure);
			GPIO_ResetBits(GPIOB,GPIO_Pin_4);
			//
			break;
		}
		case 2:
		{
			GPIO_InitTypeDef GPIO_InitStructure;
			memset(&GPIO_InitStructure,0x0,sizeof(GPIO_InitStructure));
			GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
			GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
			GPIO_Init(GPIOB, &GPIO_InitStructure);
			GPIO_ResetBits(GPIOB,GPIO_Pin_3);
			//
			break;
		}
	}
}

void led_off(unsigned char index)
{
	
	switch(index)
	{
		case 0:
		{
			GPIO_InitTypeDef GPIO_InitStructure;
			memset(&GPIO_InitStructure,0x0,sizeof(GPIO_InitStructure));
			GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
			GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
			GPIO_Init(GPIOD, &GPIO_InitStructure);
			//GPIO_ResetBits(GPIOD,GPIO_Pin_2);
			//
			break;
		}
		case 1:
		{
			GPIO_InitTypeDef GPIO_InitStructure;
			memset(&GPIO_InitStructure,0x0,sizeof(GPIO_InitStructure));
			GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
			GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
			GPIO_Init(GPIOB, &GPIO_InitStructure);
			//GPIO_ResetBits(GPIOB,GPIO_Pin_4);
			//
			break;
		}
		case 2:
		{
			GPIO_InitTypeDef GPIO_InitStructure;
			memset(&GPIO_InitStructure,0x0,sizeof(GPIO_InitStructure));
			GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
			GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
			GPIO_Init(GPIOB, &GPIO_InitStructure);
			//GPIO_ResetBits(GPIOB,GPIO_Pin_3);
			//
			break;
		}
	}
	
}

void led_off_all(void)
{
	
	led_off(0);
	led_off(1);
	led_off(2);
	//
}

void __enable_AMP_EN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_1);
	//
}
void __disable_AMP_EN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);
	//
}
void ___flicker_led(unsigned int index , unsigned int ms)
{	
}


char ledstatus[3] = {RGB_LED_OFF,RGB_LED_OFF,RGB_LED_OFF};


/**
LED 定义
红 蓝 绿

红 GPS状态
蓝 XMPP
绿 拨号

**/