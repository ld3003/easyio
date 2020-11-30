#include "lowpower.h"
#include "app_timer.h"

#include "gnss.h"
#include "global_env.h"
#include "MMA845X.h"
#include "ublox_gps.h"
#include "gps_tracker.h"

#include "my_stdc_func/debugl.h"
#include "w25q16.h"

int GPS_LOWPOWER_STATUS = GPS_INIT;

#define PRESENCE_TIME PRESENCE_UPDATE_PREIOD

#define GPS_OPEN_TIME1 60
#define GPS_OPEN_TIME2 120
#define GPS_OPEN_TIME3 60*5
#define GPS_CLOSE_TIME 60


#define ENABLE_GPS_TIM1 GPS_LOWPOWER_STATUS = GPS_ON;gps_tim  = app_timer_data.app_timer_second;sleep_tim = GPS_OPEN_TIME1;enable_gps();DEBUGL->debug("ENABLE_GPS_TIME1!!!!!!!!!!!!!!!\r\n");
#define ENABLE_GPS_TIM2 GPS_LOWPOWER_STATUS = GPS_ON;gps_tim  = app_timer_data.app_timer_second;sleep_tim = GPS_OPEN_TIME2;enable_gps();DEBUGL->debug("ENABLE_GPS_TIME2!!!!!!!!!!!!!!!\r\n");
#define ENABLE_GPS_TIM3 GPS_LOWPOWER_STATUS = GPS_ON;gps_tim  = app_timer_data.app_timer_second;sleep_tim = GPS_OPEN_TIME3;enable_gps();DEBUGL->debug("ENABLE_GPS_TIME3!!!!!!!!!!!!!!!\r\n");

//#define ENABLE_GPS_TIM3 GPS_LOWPOWER_STATUS = GPS_ON;gps_tim  = app_timer_data.app_timer_second;sleep_tim = GPS_OPEN_TIME1;enable_gps();DEBUGL->debug("ENABLE_GPS_TIME1!!!!!!!!!!!!!!!\r\n");

#define DISABLE_GPS_TIM GPS_LOWPOWER_STATUS = GPS_OFF;gps_tim  = app_timer_data.app_timer_second;sleep_tim = GPS_CLOSE_TIME;disable_gps();DEBUGL->debug("DISABLE!!!!!!!!!!!!!!!\r\n");

#define ROUT_SLEEP_TIM(X) gps_tim=app_timer_data.app_timer_second;sleep_tim = X;
#define WAIT_ROUT_TIMEOUT ((app_timer_data.app_timer_second - gps_tim) >= sleep_tim)

static unsigned char DEVICE_STAY_TIME_REC = 0; //��¼��һ�μ�¼��ֹ����Сʱ��״̬

static unsigned char zero_cnt = 0;
static unsigned char less_than_3 = 0;
static unsigned char 	great_than_or_equal_3 = 0;
static void lowpower_sta_counter(void)
{
	if (gps_info.available_sat_count == 0)
	{
		zero_cnt ++;
		great_than_or_equal_3 = 0;
	}
	if (gps_info.available_sat_count < 3)
	{
		less_than_3 ++;
		great_than_or_equal_3 = 0;
	}
	
	if (gps_info.available_sat_count >= 3)
	{
		zero_cnt =0;
		less_than_3 =0;
		great_than_or_equal_3 ++;
	}
	//
}

static void reset_lowpower_sta_counter(void)
{
	zero_cnt =0;
	less_than_3 =0;
	great_than_or_equal_3 =0;
}

static unsigned int gps_tim = 0;		//gps ����ʱ��
static unsigned int sleep_tim = 0;	//gps ����ʱ��

void lowpower_routing(void *p)
{

	
	//���ݲ�ͬ��GPSģʽ����ѡ��lowpower ����
	switch(CONF_GPS_WORK_TYPE)
	{
		case 0:
			disable_gps();
			GPS_LOWPOWER_STATUS = GPS_OFF;
			return ;
			break;
		case 1:
			enable_gps();
			GPS_LOWPOWER_STATUS = GPS_ON;
			return ;
			break;
		case 2:
			break;
	}
	
	switch(GPS_LOWPOWER_STATUS)
	{
		case GPS_INIT:
		{
			reset_lowpower_sta_counter();
			
				if (tracker_private_data.xmpp_pma->xmpp_status != XMPP_BINDSUCC)
				{
					DISABLE_GPS_TIM;
				} else {
					ENABLE_GPS_TIM1;
				}
			break;
		}
		
		case GPS_NOPOS:
			DEBUGL->debug("gps_nopos \r\n");
			if (WAIT_ROUT_TIMEOUT)
			{
				lowpower_sta_counter();
				
				//�������ε���0 ������4�ε��� 3 ����...
				if ((zero_cnt == 2) || (less_than_3 == 4) || (great_than_or_equal_3 == 20))
				{
					reset_lowpower_sta_counter();
					DISABLE_GPS_TIM;
					//
				}else
				
				//����3������GPS15��
				if (great_than_or_equal_3 > 0)
				{
					ENABLE_GPS_TIM1;
				}else
				{
				
					ROUT_SLEEP_TIM(3);
				}
				
				//
			}
			
			break;
		
		case GPS_ON:
		{
			if ((app_timer_data.app_timer_second - gps_tim) >= sleep_tim)
			{
				//��ʱ��
				if (gps_info.gprmc.status == 'A') //�Ѷ�λ
				{
					if (PRESENCE_TIME < 60)//�ر����С�� 60��
					{
						if(MMA845X_IS_MOVING) //��������ƶ�
						{
							ENABLE_GPS_TIM1;
							//
						}else //���û���ƶ�
						{
							if (gps_info.available_sat_count < 3) //���û��һ���ҿ���������С��3��
							{
								DISABLE_GPS_TIM;
								//
							}else
							{
								ENABLE_GPS_TIM1;
								//
							}
							//
						}
						//
					}else//�ر���� ���� 60 ��
					{
						
						DISABLE_GPS_TIM;
					}
					//
				}else //δ��λ
				{
					GPS_LOWPOWER_STATUS = GPS_NOPOS;
					ROUT_SLEEP_TIM(3);
					//
				}
				
			}else
			{
				DEBUGL->debug("LP Delay GPS_ON [%d] !!!!\r\n",(sleep_tim - (app_timer_data.app_timer_second - gps_tim)));

			}
			break;
		}
		case GPS_OFF:
		{
			
			if ((app_timer_data.app_timer_second - gps_tim) >= sleep_tim)
			{
				
				if (MMA845X_IS_MOVING > 0) //���Gsensor ���ƶ�
				{
					if (tracker_private_data.xmpp_pma->xmpp_status != XMPP_BINDSUCC)
					{
						DISABLE_GPS_TIM;
					} else {
						ENABLE_GPS_TIM1;
					}
					//
				}else 
				{
					
					#if 0 //�ر�ͣ������Сʱ���Ϳ���GPS 2���ӵĹ��ܣ���Ϊ������ܴ���BUG
					if (DEVICE_STAY_TIME > 3600 * 3) //���ͣ��ʱ�䳬�� 3 Сʱ
					{
						ENABLE_GPS_TIM2;
					}else
					{
						DISABLE_GPS_TIM;
						//
					}
					#else
					DISABLE_GPS_TIM;
					#endif
					//
				}
				//
			}else
			{
				DEBUGL->debug("LP Delay GPS_OFF [%d] !!!!\r\n",(sleep_tim - (app_timer_data.app_timer_second - gps_tim)));
			}
			
			break;
		}
		
	}
	
	//
}

void open_gps_2min(void)
{
	ENABLE_GPS_TIM2;
}

void open_gps_5min(void)
{
	ENABLE_GPS_TIM3;
}


//��Ƶ��غ�����
extern unsigned int SystemCoreClock;
extern void rt_hw_board_init(void);
extern unsigned int SystemCoreClockConfigure_1M(void);
extern unsigned int SystemCoreClockConfigure_2M(void);
extern unsigned int SystemCoreClockConfigure_4M(void);
extern unsigned int SystemCoreClockConfigure_32M(void);
extern unsigned int SystemCoreClockConfigure_12M(void);
extern unsigned int SystemCoreClockConfigure_65K(void);
///SystemCoreClockConfigure_65K
extern void rt_hw_board_init2(void);
extern void rt_hw_usart_init(void);
extern void reinit_system_serial(void);

extern char UART3_USE_DMA_FLAG;
extern char UART2_USE_DMA_FLAG;

#if 0
void rt_hw_board_init(void)
{
	
    /* NVIC Configuration */
    NVIC_Configuration();

    /* Configure the SysTick */
    SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND);

#if STM32_EXT_SRAM
    EXT_SRAM_Configuration();
#endif

    rt_hw_usart_init();
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);

#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif
}
#endif

extern void reinit_board(void);

#ifdef FRQL
static unsigned char fqtype = 1;
#endif

#ifdef FRQH
static unsigned char fqtype = 2;
#endif

void config_fq_0(void)
{

	if (fqtype == 0)
		return ;
	fqtype = 0;
	UART3_USE_DMA_FLAG = 1;
	SystemCoreClock = SystemCoreClockConfigure_1M();
	USART_DeInit(USART2);
	reinit_board();
	reinit_system_serial();
}

void config_fq_1(void)
{

	unsigned short spi_flash_id = 0x0;
	
	//��ֻ֤��
	if (fqtype == 1)
		return ;
	fqtype = 1;

	UART3_USE_DMA_FLAG = 1;
	UART2_USE_DMA_FLAG = 1;
	SystemInit();
	SystemCoreClock = SystemCoreClockConfigure_2M();
	reinit_board();
	reinit_system_serial();
	
		//��ʼ��SPIFlash
	SPI_Flash_Init();
	spi_flash_id = SPI_Flash_ReadID();
	DEBUGL->debug("SPI Flash ID %X \r\n",spi_flash_id);
}



void config_fq_2(void)
{
	unsigned short spi_flash_id = 0x0;
	
	if (fqtype == 2)
		return ;
	fqtype = 2;

	UART3_USE_DMA_FLAG = 0;
	UART2_USE_DMA_FLAG = 0;
	SystemCoreClock = SystemCoreClockConfigure_32M();
	reinit_board();
	reinit_system_serial();
	
	//��ʼ��SPIFlash
	SPI_Flash_Init();
	spi_flash_id = SPI_Flash_ReadID();
	DEBUGL->debug("SPI Flash ID %X \r\n",spi_flash_id);
	
	
}

#include "led.h"
#include "gps_tracker.h"

void sys_power_off(void)
{
	unsigned int powerkey_cnt = 0;
	
	config_fq_1();		//�ָ���2M
	
	if (tracker_private_data.SYSTEM_STATUS != SYSTEM_POWER_OFF)
	{
		tracker_private_data.SYSTEM_STATUS = SYSTEM_POWER_OFF;
		m_define_trans_threshold_set(0);
		//userapp_writeconfig();
	}
	
	__disable_dtr_nosleep();
	
	//�ر�gpsģ��
	disable_gps();
	//�ر�GPRS ģ��
	__reset_rst();
	__reset_power();
	
	//�ر���Ƶ�Ŵ���,amp DIABLE
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);
	
							
	//
}