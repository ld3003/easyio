#include "sim900.h"
#include "at_cmd.h"
#include "modem_serial.h"
#include "common.h"

#include <rtthread.h>
#include <stm32l1xx.h>

#include "app_timer.h"
#include "app_local_conf.h"
#include "my_stdc_func/debugl.h"
#include "gps_tracker.h"

#define SET_PWR __set_power();
#define RESET_PWR __reset_power();

#define SET_RST __set_rst();
#define RESET_RST __reset_rst();

static struct rt_semaphore dtr_sem;

static void setGPIO_AN(void)
{
//	
//	GPIO_InitTypeDef GPIO_InitStructure;
//	
//	memset(&GPIO_InitStructure,0x0,sizeof(GPIO_InitStructure));
//		//PWR
//	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AN;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
//	GPIO_Init(GPIOC, &GPIO_InitStructure);
//	
//	//RESET
//	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AN;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
//	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
}

void __reset_power(void)
{
		//PWR
	GPIO_InitTypeDef GPIO_InitStructure;
	memset(&GPIO_InitStructure,0x0,sizeof(GPIO_InitStructure));
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOC,GPIO_Pin_0);
}

void __set_power(void)
{
		//PWR
	GPIO_InitTypeDef GPIO_InitStructure;
	memset(&GPIO_InitStructure,0x0,sizeof(GPIO_InitStructure));
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC,GPIO_Pin_0);
}

void __reset_rst(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	memset(&GPIO_InitStructure,0x0,sizeof(GPIO_InitStructure));
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOC,GPIO_Pin_1);
}

void __set_rst(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	memset(&GPIO_InitStructure,0x0,sizeof(GPIO_InitStructure));
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC,GPIO_Pin_1);
}

static void init_sim900_ctrl_gpio(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//RCC_APB2PeriphClockCmd(RCC_APBPeriph_GPIOB,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC,ENABLE);
	

	//DTR
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);
	

	//DSR
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	//DCD
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	//RI
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
	RESET_PWR;
	RESET_RST;
	
	
	rt_sem_init(&dtr_sem, "dtr_sem", 1, RT_IPC_FLAG_FIFO);
	

}


static void power_sim900(void)
{
	
	RESET_PWR;
	rt_thread_sleep(RT_TICK_PER_SECOND*1);
	SET_PWR;
	
}

void power_off(void)
{
	RESET_PWR;
	rt_thread_sleep(RT_TICK_PER_SECOND*2);
}

static void reset_sim900(void)
{
	
	//return ;
	RESET_RST;
	RESET_PWR;
	rt_thread_sleep( RT_TICK_PER_SECOND * 1);
	SET_RST;
	SET_PWR
	//rt_thread_sleep( RT_TICK_PER_SECOND/10);
}


#define UART3_GPIO_TX		GPIO_Pin_10
#define UART3_GPIO_RX		GPIO_Pin_11
#define UART3_GPIO_CTS		GPIO_Pin_13
#define UART3_GPIO_RTS		GPIO_Pin_14
#define UART3_GPIO			GPIOB

static unsigned char always_enable_dtr_flag = 0;
void enable_dtr_always(char on)
{
	always_enable_dtr_flag = on;
	enable_dtr();
}

void enable_dtr(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//DEBUGL->debug("enable DTR  \r\n \r\n");
	
	//打开DTR
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);
	
	#ifdef ENABLE_CTSRTS
	//开启CTS	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = UART3_GPIO_CTS;
	GPIO_Init(UART3_GPIO, &GPIO_InitStructure);
	#endif
	
	
	//重新初始化TX管脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
	rt_thread_sleep(RT_TICK_PER_SECOND/50);
}

void disable_dtr(void)
{
	
	
	GPIO_InitTypeDef GPIO_InitStructure;
	memset(&GPIO_InitStructure,0x0,sizeof(GPIO_InitStructure));
	
	if (always_enable_dtr_flag == 1)
		return ;
	
	//这里也需要加一个delay，也不知道为什么
	rt_thread_sleep(RT_TICK_PER_SECOND/100);
	
	
	#ifdef ENABLE_CTSRTS
	//CTS 变为高阻
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = UART3_GPIO_CTS;
	GPIO_Init(UART3_GPIO, &GPIO_InitStructure);
	#endif
	
	
	//将TX变为高阻态
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
	//关闭DTR
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
	//
}


/*
	主要用于 关机的时候 会把所有的管脚的输出设置为 AN 以减少功耗
*/

void __disable_dtr_nosleep(void)
{
	
	
	GPIO_InitTypeDef GPIO_InitStructure;
	memset(&GPIO_InitStructure,0x0,sizeof(GPIO_InitStructure));
	
	#ifdef ENABLE_CTSRTS
	//CTS 变为高阻
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = UART3_GPIO_CTS;
	GPIO_Init(UART3_GPIO, &GPIO_InitStructure);
	#endif
	
	
	//将TX变为高阻态
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	////将TX变为高阻态
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
	//关闭DTR
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
	//
}

static void initModem2(void)
{
	unsigned char __UEXTDCONF_FLAG	= 0;
	unsigned char __USPM						= 0;
	unsigned char __USGC						= 0;
	unsigned char __USGC2						= 0;
	unsigned char __ATD							= 1;
	//AT+USGC?
	
	int i=0;
	int at_cmd_ret_code;
	unsigned char atcmd_cnt;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	repower:	
	DEBUGL->debug("reset_sim900....\r\n");
	reset_sim900();

	
	for(i=0;i<5;i++)
	{
		at_command("AT\r\n",AT_AT,200,&at_cmd_ret_code);
		if (at_cmd_ret_code == AT_RESP_OK)
			return ;
	}
	
	goto repower;
}


void init_modem_hw(void)
{
	init_sim900_ctrl_gpio();
	config_simdet();
}


static int recv_cmux_buf(const char *resp , int len)
{
	return 1;
	//
}

static unsigned char setting_modem_status = 0;
static int setting_modem(void)
{
	int at_cmd_ret_code;
	switch(setting_modem_status)
	{
		case 0:
		{
			at_command("AT+FLO=0\r\n",AT_AT,200,&at_cmd_ret_code);
			if (at_cmd_ret_code == AT_RESP_OK)
			{
				setting_modem_status ++;
			}
			break;
		}
		case 1:
		{
			at_command("AT&K0\r\n",AT_AT,200,&at_cmd_ret_code);
			if (at_cmd_ret_code == AT_RESP_OK)
			{
				setting_modem_status ++;
			}
			break;
		}		
		case 2:
		{
			at_command("AT+CGMR\r\n",AT_AT,200,&at_cmd_ret_code);
			if (at_cmd_ret_code == AT_RESP_OK)
			{
				setting_modem_status ++;
			}
			break;
		}		
			
	}
}
extern void reinit_uart3_baud(unsigned int baud);


static unsigned char check_3gmodem_active_flag = 0;
void check_3gmodem_active(void)
{
	static unsigned IDLE_CNT = 0;
	if (check_3gmodem_active_flag == 1)
	{
		
		//检查3g modem 是否正常
		if (!IS_SARAU2_ACTIVE)
		{
			IDLE_CNT ++;
			
			if (IDLE_CNT > 5)
			{
				DEBUGL->debug("SARA U2 Error !\r\n");
				
				//简单粗暴，发现Ublox3G模块 挂掉 直接复位系统
				rt_reset_system();
			}
			//
		} else {
			IDLE_CNT = 0;
		}
		//
	} else {
		IDLE_CNT = 0;
	}
	//
}



void init_sim900_modem(void)
{
	
	int i,at_cmd_ret_code;
	
	reinit:
	check_3gmodem_active_flag = 0;
	initModem2();
	
//	ENABLE_SMS_TEXTMOD;
//	

//	//AT+CMGR=
//	
//	at_command("AT+CMGR=1\r\n",AT_AT,200,&at_cmd_ret_code);
//	at_command("AT+CMGD=1\r\n",AT_AT,200,&at_cmd_ret_code);
	
	
	//at_command("AT+URING=0\r\n",AT_AT,200,&at_cmd_ret_code); //sms
	
	at_command("AT+UGPIOC=42,0\r\n",AT_AT,200,&at_cmd_ret_code);	//变为OUTPUT模式
	if (AT_RESP_OK != at_cmd_ret_code)
	{
		goto reinit;
	}
	
	at_command("AT+UGPIOW=42,1\r\n",AT_AT,200,&at_cmd_ret_code);	//变为 output 为 1
	if (AT_RESP_OK != at_cmd_ret_code)
	{
		goto reinit;
	}
	
	check_3gmodem_active_flag = 1;
	
	

	//开启
	
	#ifdef ENABLE_CTSRTS
	at_command("AT&K3\r\n",AT_AT,200,&at_cmd_ret_code);
	#endif
	
	at_command("AT+COPS=0,2\r\n",AT_AT,200,&at_cmd_ret_code);
	
	for(i=1;i<=10;i++)
	{
		at_command("AT+GSN\r\n",AT_GSN,200,&at_cmd_ret_code);
		
		if (at_cmd_ret_code == AT_RESP_OK)
			break;
		
		if (i == 10)
		{
			goto reinit;
		}
		
	}
	
	for(i=1;i<=10;i++)
	{
		at_command("AT+CCID\r\n",AT_CCID,200,&at_cmd_ret_code);
		
		if (at_cmd_ret_code == AT_RESP_OK)
			break;
		
		if (i == 10)
		{
			goto reinit;
		}
		
	}
	
	for(i=1;i<=60;i++)
	{
		//AT+CPIN?
		at_command("AT+GSN\r\n",AT_GSN,200,&at_cmd_ret_code);
		//at_command("AT+CCID\r\n",AT_CCID,200,&at_cmd_ret_code);
		at_command("AT+CPIN?\r\n",AT_COPS,200,&at_cmd_ret_code);
		at_command("AT+COPS?\r\n",AT_COPS,200,&at_cmd_ret_code);
		
		if (at_cmd_ret_code == AT_RESP_OK)
			break;
		
		if (i == 60)
		{
			goto reinit;
		}
		
	}
	
	
	//檢查CCID是否存在於 SPIFLASH中
	if(strstr(tracker_private_data.conf->APNINFO.CCID,simcard_ccid) <= 0)
	{
		int i=0;
		char apn_cnt = 0;
		char apn_cnt_index = 0;
		const APN_TABLE *atb;
		char atcmd_buf[64];
		apn_cnt = get_apn_cnt(network_code);
		
		
		for(i=0;i<apn_cnt;i++)
		{
			
			atb = get_apn(network_code,i);
			if (atb > 0)
			{
				DEBUGL->debug("GET APN NAME %d %s \r\n",network_code,atb->apn);
				snprintf(atcmd_buf,sizeof(atcmd_buf),"AT+CGDCONT=1,\"IP\",\"%s\"\r\n",atb->apn);
				at_command(atcmd_buf,AT_CGDCONT,200,&at_cmd_ret_code);
				
				at_command_wait("ATD*99***1#\r\n",AT_ATD,6000,&at_cmd_ret_code);
				if (at_cmd_ret_code == AT_RESP_OK)
				{
					sprintf(tracker_private_data.conf->APNINFO.CCID,"%s",simcard_ccid);
					sprintf(tracker_private_data.conf->APNINFO.APN,"%s",atb->apn);
					sprintf(tracker_private_data.conf->APNINFO.USERNAME,"%s",atb->username);
					sprintf(tracker_private_data.conf->APNINFO.PASSWORD,"%s",atb->password);
					userapp_writeconfig();
					rt_reset_system();
					break;
				}
				
			} else {
				break;
			}
			
		}
		
		goto reinit;

		
		//
	}
	
	
	//setGPIO_AN();
	#ifdef TRACKER_TST
	open_max9860();
	#else
	close_max9860();
	#endif
	//
	
}



void init_sim900_modem_chk(void)
{
	
	int i,at_cmd_ret_code;
	
	reinit:
	check_3gmodem_active_flag = 0;
	initModem2();
	
//	ENABLE_SMS_TEXTMOD;
//	

//	//AT+CMGR=
//	
//	at_command("AT+CMGR=1\r\n",AT_AT,200,&at_cmd_ret_code);
//	at_command("AT+CMGD=1\r\n",AT_AT,200,&at_cmd_ret_code);
	
	
	//at_command("AT+URING=0\r\n",AT_AT,200,&at_cmd_ret_code); //sms
	
	at_command("AT+UGPIOC=42,0\r\n",AT_AT,200,&at_cmd_ret_code);	//变为OUTPUT模式
	if (AT_RESP_OK != at_cmd_ret_code)
	{
		goto reinit;
	}
	
	at_command("AT+UGPIOW=42,1\r\n",AT_AT,200,&at_cmd_ret_code);	//变为 output 为 1
	if (AT_RESP_OK != at_cmd_ret_code)
	{
		goto reinit;
	}
	
	check_3gmodem_active_flag = 1;
	
	

	//开启
	
	#ifdef ENABLE_CTSRTS
	at_command("AT&K3\r\n",AT_AT,200,&at_cmd_ret_code);
	#endif
	
	at_command("AT+COPS=0,2\r\n",AT_AT,200,&at_cmd_ret_code);
	
	for(i=1;i<=10;i++)
	{
		at_command("AT+GSN\r\n",AT_GSN,200,&at_cmd_ret_code);
		
		if (at_cmd_ret_code == AT_RESP_OK)
			break;
		
		if (i == 10)
		{
			goto reinit;
		}
		
	}
	
//	for(i=1;i<=10;i++)
//	{
//		at_command("AT+CCID\r\n",AT_CCID,200,&at_cmd_ret_code);
//		
//		if (at_cmd_ret_code == AT_RESP_OK)
//			break;
//		
//		if (i == 10)
//		{
//			goto reinit;
//		}
//		
//	}
//	
//	for(i=1;i<=60;i++)
//	{
//		//AT+CPIN?
//		at_command("AT+GSN\r\n",AT_GSN,200,&at_cmd_ret_code);
//		//at_command("AT+CCID\r\n",AT_CCID,200,&at_cmd_ret_code);
//		at_command("AT+CPIN?\r\n",AT_COPS,200,&at_cmd_ret_code);
//		at_command("AT+COPS?\r\n",AT_COPS,200,&at_cmd_ret_code);
//		
//		if (at_cmd_ret_code == AT_RESP_OK)
//			break;
//		
//		if (i == 60)
//		{
//			goto reinit;
//		}
//		
//	}
//	
//	
//	//檢查CCID是否存在於 SPIFLASH中
//	if(strstr(tracker_private_data.conf->APNINFO.CCID,simcard_ccid) <= 0)
//	{
//		int i=0;
//		char apn_cnt = 0;
//		char apn_cnt_index = 0;
//		const APN_TABLE *atb;
//		char atcmd_buf[64];
//		apn_cnt = get_apn_cnt(network_code);
//		
//		
//		for(i=0;i<apn_cnt;i++)
//		{
//			
//			atb = get_apn(network_code,i);
//			if (atb > 0)
//			{
//				DEBUGL->debug("GET APN NAME %d %s \r\n",network_code,atb->apn);
//				snprintf(atcmd_buf,sizeof(atcmd_buf),"AT+CGDCONT=1,\"IP\",\"%s\"\r\n",atb->apn);
//				at_command(atcmd_buf,AT_CGDCONT,200,&at_cmd_ret_code);
//				
//				at_command_wait("ATD*99***1#\r\n",AT_ATD,6000,&at_cmd_ret_code);
//				if (at_cmd_ret_code == AT_RESP_OK)
//				{
//					sprintf(tracker_private_data.conf->APNINFO.CCID,"%s",simcard_ccid);
//					sprintf(tracker_private_data.conf->APNINFO.APN,"%s",atb->apn);
//					sprintf(tracker_private_data.conf->APNINFO.USERNAME,"%s",atb->username);
//					sprintf(tracker_private_data.conf->APNINFO.PASSWORD,"%s",atb->password);
//					userapp_writeconfig();
//					rt_reset_system();
//					break;
//				}
//				
//			} else {
//				break;
//			}
//			
//		}
//		
//		goto reinit;

//		
//		//
//	}
//	
	
	//setGPIO_AN();
	#ifdef TRACKER_TST
	open_max9860();
	#else
	close_max9860();
	#endif
	//
	
}



#include "stm32l1xx_syscfg.h"
#include "app_ev.h"



static void debug_ring_int(void *p)
{
	//config_fq_1();
	DEBUGL->debug("LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL\r\n");
}
static void debug_ring_int2(void *p)
{
	int at_cmd_ret_code;
	//config_fq_1();
	DEBUGL->debug("HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH\r\n");

	
}



void EXTI15_10_IRQHandler(void)
{

	EXTI_ClearFlag(EXTI_Line15);
	
	if (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15))
	{
		//config_fq_1();
		post_hw_int_event(debug_ring_int,0,0,0);
	}
	else
	{
		//config_fq_1();
		post_hw_int_event(debug_ring_int2,0,0,0);
	}
	
}



void config_ring_interrupt(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	
	/* Configure PD.03, PC.04, as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource15);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	//设置中断触发状态 
	EXTI_InitStructure.EXTI_Line = EXTI_Line15 ;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;//EXTI_Trigger_Rising;//EXTI_Trigger_Rising_Falling; //EXTI_Trigger_Rising
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable the EXTI12\3 Interrupt on PC13 */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}




static void debug_simdet_int(void *p)
{
	//config_fq_1();
	DEBUGL->debug("SIMDET LLLLLLLLLLLLLLLLLLLLLLLL\r\n");
}
static void debug_simdet_int2(void *p)
{
	//config_fq_1();
	DEBUGL->debug("SIMDET HHHHHHHHHHHHHHHHHHHHHHHH\r\n");
}

void EXTI9_5_IRQHandler_Line8(void)
{

	EXTI_ClearFlag(EXTI_Line18);
	if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8))
	{
		//config_fq_1();
		post_hw_int_event(debug_simdet_int,0,0,0);
	}
	else
	{
		//config_fq_1();
		post_hw_int_event(debug_simdet_int2,0,0,0);
	}
	//
}

void config_simdet(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	//GPIO_InitStructure.
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	//GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource8);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
//	
//	//设置中断触发状态 
//	EXTI_InitStructure.EXTI_Line = EXTI_Line8 ;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;//EXTI_Trigger_Rising;//EXTI_Trigger_Rising_Falling; //EXTI_Trigger_Rising
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);

//	/* Enable the EXTI12\3 Interrupt on PC13 */
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	
	
}

void close_max9860(void)
{
	int at_cmd_ret_code;
	at_command("AT+UI2CO=1,0,0,0x10,0\r\n",AT_AT,200,&at_cmd_ret_code);
	at_command("AT+UI2CW=\"1000\",2\r\n",AT_AT,200,&at_cmd_ret_code);
	at_command("AT+UI2CC\r\n",AT_AT,200,&at_cmd_ret_code);
	//
}

void open_max9860(void)
{
	int at_cmd_ret_code;
	at_command("AT+UI2CO=1,0,0,0x10,0\r\n",AT_AT,200,&at_cmd_ret_code);
	at_command("AT+UEXTDCONF=0,1\r\n",AT_AT,200,&at_cmd_ret_code);
	//at_command("AT+USPM=1,1,0,0\r\n",AT_AT,200,&at_cmd_ret_code);
	at_command("AT+USGC=1,6,6,16000,16000,22000\r\n",AT_AT,200,&at_cmd_ret_code);
}
