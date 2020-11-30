#include <stm32l1xx.h>
#include <rtthread.h>

#include "IOI2C.h"
#include "MMA845X.h"
#include "app_ev.h"
#include "app_timer.h"

#include "my_stdc_func/debugl.h"
#include "gps_tracker.h"

#include <stm32l1xx_syscfg.h>


#define m_define_ic_id 0x2A
#define m_define_device_address_write 0x3A
#define m_define_device_address_read 0x3B

#define m_define_ctl_reg_1 0x2A
#define m_define_ctl_reg_2 0x2B
#define m_define_ctl_reg_3 0x2C
#define m_define_ctl_reg_4 0x2D
#define m_define_ctl_reg_5 0x2E

#define m_define_ctl_interrupt_status 0x0C
#define m_define_trans_config 0x1D
#define m_define_trans_source 0x1E
#define m_define_trans_threshold 0x1F
#define m_define_trans_count 0x20

#define m_define_xyz_date_config 0x0E
#define m_define_hp_filter_cut_off_config 0x0F

#define m_define_value_ctl_reg_1_standby 0x1C
#define m_define_value_ctl_reg_1_activity 0x1D

#define m_define_value_ctl_reg_2  0x01
#define m_define_value_ctl_reg_3_interrupt_high 0x02
#define m_define_value_ctl_reg_4_trans_mode 0x20
#define m_define_value_ctl_reg_5_pin2 0x00

#define m_define_value_how_much_byte_1 1
#define m_define_value_setting 0x24

#define m_define_value_trans_xyz_date_config 0x10
#define m_define_value_trans_hp_filter_cut_off_config 0x00

#define m_define_value_trans_config 0x0E
#define m_define_value_trans_threshold 0x01

#define m_define_value_trans_count  0x05

unsigned char mma845x_last_value = 0;
unsigned int device_moving_time = 0;
unsigned int mma8452_moving_time = 0;
unsigned int mma8452_stay_time = 0;

unsigned char mma845x_movingid = 0;

typedef unsigned char u8;


static void write_i2c_dev(u8 devid , u8 reg , u8 value , u8 num , u8 vs)
{
	IICwriteBytes(devid,reg,1,&value);
}

static void read_i2c_dev(u8 devid , u8 reg , u8 value , u8 num , u8 vs)
{
	IICreadBytes(devid,reg,1,&value);
}
//IICreadBytes
/**
	初始化MMA8452模块
*/

void m_define_trans_threshold_set(unsigned char value)
{
	extern unsigned short I2C_Erorr_Count;
	write_i2c_dev(m_define_device_address_write,
			m_define_trans_threshold,
			value,
			0x01,
			m_define_value_setting);
	
}

unsigned char  m_define_trans_threshold_get(unsigned char value)
{
	extern unsigned short I2C_Erorr_Count;
	unsigned char xxxvalue = 0x0; //0x1F
	IICreadBytes(m_define_device_address_write,m_define_trans_threshold,1,&xxxvalue);
	return xxxvalue;
	
}

char init_mma845x(void)
{

	unsigned char xxxvalue;
	extern unsigned short I2C_Erorr_Count;
	
	
	IOI2C_Init();
	
	#if 0
		write_i2c_dev(m_define_device_address_write,
			m_define_trans_threshold,
			m_define_value_trans_threshold,
			m_define_value_how_much_byte_1,
			m_define_value_setting);
	#endif
	
	xxxvalue = 0x0; //0x1F
	IICreadBytes(m_define_device_address_write,m_define_trans_threshold,1,&xxxvalue);
	DEBUGL->debug("XXXXXXXXXX %d \r\n",xxxvalue);
	
	tracker_private_data.mma8452_power_on_off = xxxvalue;
	
	
	reread:
	
	
	write_i2c_dev(m_define_device_address_write, 
			m_define_ctl_reg_1, 
			m_define_value_ctl_reg_1_standby, 
			m_define_value_how_much_byte_1, 
			m_define_value_setting);

	write_i2c_dev(m_define_device_address_write,
			m_define_ctl_reg_2,
			m_define_value_ctl_reg_2,
			m_define_value_how_much_byte_1,
			m_define_value_setting);

	write_i2c_dev(m_define_device_address_write,
			m_define_ctl_reg_3,
			m_define_value_ctl_reg_3_interrupt_high,
			m_define_value_how_much_byte_1,
			m_define_value_setting);

	write_i2c_dev(m_define_device_address_write,
			m_define_ctl_reg_4,
			m_define_value_ctl_reg_4_trans_mode,
			m_define_value_how_much_byte_1,
			m_define_value_setting);

	write_i2c_dev(m_define_device_address_write,
			m_define_ctl_reg_5,
			m_define_value_ctl_reg_5_pin2,
			m_define_value_how_much_byte_1,
			m_define_value_setting);

	write_i2c_dev(m_define_device_address_write,
			m_define_xyz_date_config,
			m_define_value_trans_xyz_date_config,
			m_define_value_how_much_byte_1,
			m_define_value_setting);

	write_i2c_dev(m_define_device_address_write,
			m_define_hp_filter_cut_off_config,
			m_define_value_trans_hp_filter_cut_off_config,
			m_define_value_how_much_byte_1,
			m_define_value_setting);

	write_i2c_dev(m_define_device_address_write,
			m_define_trans_config,
			m_define_value_trans_config,
			m_define_value_how_much_byte_1,
			m_define_value_setting);

	write_i2c_dev(m_define_device_address_write,
			m_define_trans_threshold,
			m_define_value_trans_threshold,
			m_define_value_how_much_byte_1,
			m_define_value_setting);


	write_i2c_dev(m_define_device_address_write,
			m_define_trans_count,
			m_define_value_trans_count,
			m_define_value_how_much_byte_1,
			m_define_value_setting);
			
			
			


	write_i2c_dev(m_define_device_address_write, 
			m_define_ctl_reg_1, 
			m_define_value_ctl_reg_1_standby, 
			m_define_value_how_much_byte_1, 
			m_define_value_setting);

	write_i2c_dev(m_define_device_address_write, 
			m_define_ctl_reg_1, 
			m_define_value_ctl_reg_1_activity, 
			m_define_value_how_much_byte_1, 
			m_define_value_setting);


	write_i2c_dev(m_define_device_address_write, 
			m_define_ctl_reg_1, 
			m_define_value_ctl_reg_1_activity, 
			m_define_value_how_much_byte_1, 
			m_define_value_setting);

			
			
	if (I2C_Erorr_Count == 0)
	{
		DEBUGL->debug("Init MMA8452Q SUCCESS ! \r\n");
		return 1;
	}
	else
		DEBUGL->debug("Init MMA8452Q Failed ! \r\n");
	
	
	return 0;

}




static void debug_mma845_int(void *p)
{
	mma845x_routing(0);
}

void EXTI9_5_IRQHandler(void)
{

	EXTI_ClearFlag(EXTI_Line5);
	post_hw_int_event(debug_mma845_int,0,0,0);
	//
}

void config_mma845x_interrupt(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	
	/* Configure PD.03, PC.04, as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource5);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	//设置中断触发状态 
	EXTI_InitStructure.EXTI_Line = EXTI_Line5 ;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;//EXTI_Trigger_Rising;//EXTI_Trigger_Rising_Falling; //EXTI_Trigger_Rising
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable the EXTI12\3 Interrupt on PC13 */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}



void mma845x_routing(void *p)
{
	
	//app_timer_data
	static unsigned int start_time = 0;
	static uint8_t status;
	/* 检查MMA8452 是否有移动*/
	status = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5);
	
	#ifdef DEVICE_MOTION
	status = 1;
	#endif
	
	
	if (status == 1)
	{
		//把ID加1
		mma845x_movingid ++;
		//global_gps_mov_status = DEV_MOVING;
		mma8452_stay_time = 0;
		if (mma8452_moving_time == 0)
		{
			mma8452_moving_time = app_timer_data.app_timer_second ;
		}
		//mma8452_moving_time = (app_timer_data.app_timer_second - start_time);
	}else if (status == 0)
	{
		mma8452_moving_time = 0;
		if (mma8452_stay_time == 0)
		{
			mma8452_stay_time = app_timer_data.app_timer_second ;
		}
		//mma8452_stay_time = (app_timer_data.app_timer_second - start_time);
	}
	
	DEBUGL->debug("mma8452 status MOV:%d STA:%d \r\n",DEVICE_MOV_TIME,DEVICE_STAY_TIME);
}



int __is_moving(void)
{
	static unsigned char DEVICE_STAY_TIME_SEC = 20;
	
//	if (DEVICE_STAY_TIME > DEVICE_STAY_TIME_SEC)
//		return 0;
//	return 1;
//	
//	if (tracker_private_data.dbg_motion == 1)
//		return 1;
	
	//比较当前状态是静止还是移动
	if (DEVICE_STAY_TIME > DEVICE_STAY_TIME_SEC)
	{
		DEVICE_STAY_TIME_SEC = 20;
		device_moving_time = 0;
		return 0;
	}
	else
	{
		if (device_moving_time == 0)
		{
			device_moving_time = app_timer_data.app_timer_second;
		}
		
		//如果是移动状态，计算总共运动了多久
		//如果超过了 90 秒则改为 150
		if ((app_timer_data.app_timer_second - device_moving_time) > 90)
		{
			DEVICE_STAY_TIME_SEC = 150;
		}
		
		return 1;
	}
	//
}


//static void deal_rising_interrupt(void*p)
//{
//	rt_kprintf("mma85452 Rising interrupt _____\r\n");
//	global_rising_interrupt_time = ApiGetSysSecondCounter();
//	global_falling_interrupt_time = 0;
//	global_gps_mov_status = DEV_MOVING;

//}

//static void deal_falling_interrupt(void*p)
//{
//	rt_kprintf("mma85452 Falling interrupt _____\r\n");
//	global_rising_interrupt_time = 0;
//	global_falling_interrupt_time = ApiGetSysSecondCounter();
//	global_gps_mov_status = DEV_STAY;
//	
//	//global_gps_mov_status = DEV_MOVING;

//}

//static EXTITrigger_TypeDef trigger_type = EXTI_Trigger_Rising;

//static void set_trigger_type(void)
//{
//	EXTI_InitTypeDef EXTI_InitStructure;
//	//设置中断触发状态 
//	EXTI_InitStructure.EXTI_Line = EXTI_Line13 ;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger = trigger_type;//EXTI_Trigger_Rising;//EXTI_Trigger_Rising_Falling; //EXTI_Trigger_Rising
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);
//}

//void EXTI15_10_IRQHandler(void)
//{
//	//改变中断状态
//	
//	if(EXTI_GetITStatus(EXTI_Line13) != RESET)
//	{
//		if (trigger_type == EXTI_Trigger_Rising)
//			ApiAddTask(deal_rising_interrupt,0); // 交给系统去异步处理
//		else
//			ApiAddTask(deal_falling_interrupt,0); // 交给系统去异步处理
//		EXTI_ClearITPendingBit(EXTI_Line13);
//	}
//	
//	if (trigger_type == EXTI_Trigger_Rising)
//	{
//		trigger_type = EXTI_Trigger_Falling;
//	}else
//	{
//		trigger_type = EXTI_Trigger_Rising;
//	}
//	
//	set_trigger_type();
//}








#define BMP180_ADDR                 0xEE     // default I2C address
#define BUFFER_SIZE                 3	  


/* ---- Registers ---- */
#define CAL_AC1           0xAA  // R   Calibration data (16 bits)
#define CAL_AC2           0xAC  // R   Calibration data (16 bits)
#define CAL_AC3           0xAE  // R   Calibration data (16 bits)    
#define CAL_AC4           0xB0  // R   Calibration data (16 bits)
#define CAL_AC5           0xB2  // R   Calibration data (16 bits)
#define CAL_AC6           0xB4  // R   Calibration data (16 bits)
#define CAL_B1            0xB6  // R   Calibration data (16 bits)
#define CAL_B2            0xB8  // R   Calibration data (16 bits)
#define CAL_MB            0xBA  // R   Calibration data (16 bits)
#define CAL_MC            0xBC  // R   Calibration data (16 bits)
#define CAL_MD            0xBE  // R   Calibration data (16 bits)
#define CONTROL           0xF4  // W   Control register 
#define CONTROL_OUTPUT    0xF6  // R   Output registers 0xF6=MSB, 0xF7=LSB, 0xF8=XLSB

// unused registers
#define SOFTRESET         0xE0
#define VERSION           0xD1  // ML_VERSION  pos=0 len=4 msk=0F  AL_VERSION pos=4 len=4 msk=f0
#define CHIPID            0xD0  // pos=0 mask=FF len=8
                                // BMP085_CHIP_ID=0x55
																
																

static unsigned char _buff[BUFFER_SIZE];

void BMP180_writemem(uint8_t _addr, uint8_t _val) {
  IICwriteByte(BMP180_ADDR,_addr,_val);
}

void BMP180_readmem(uint8_t _addr, uint8_t _nbytes, uint8_t __buff[]) {
  IICreadBytes(BMP180_ADDR,_addr,_nbytes,__buff);
}
//private_data.spiflashID = spi_flash_id;

volatile int16_t ac1,ac2,ac3,b1,b2,mb,mc,md;     //
volatile uint16_t ac4,ac5,ac6;                   //

unsigned char BMP180_getCalData(void)
{
	
	extern unsigned short I2C_Erorr_Count;
	I2C_Erorr_Count = 0;
	BMP180_readmem(CHIPID, 1, _buff);
	DEBUGL->debug("BMP180 CHIPID: %x \r\n" ,_buff[0]);
	if (I2C_Erorr_Count == 0)
		DEBUGL->debug("Init BMP180 SUCCESS \r\n");
	else
		DEBUGL->debug("Init BMP180 Failed \r\n");
	
	return _buff[0];
	
  BMP180_readmem(CAL_AC1, 2, _buff);
  ac1 = ((int16_t)_buff[0] <<8 | ((int16_t)_buff[1]));
  BMP180_readmem(CAL_AC2, 2, _buff);
  ac2 = ((int16_t)_buff[0] <<8 | ((int16_t)_buff[1]));
  BMP180_readmem(CAL_AC3, 2, _buff);
  ac3 = ((int16_t)_buff[0] <<8 | ((int16_t)_buff[1]));
  BMP180_readmem(CAL_AC4, 2, _buff);
  ac4 = ((uint16_t)_buff[0] <<8 | ((uint16_t)_buff[1]));
  BMP180_readmem(CAL_AC5, 2, _buff);
  ac5 = ((uint16_t)_buff[0] <<8 | ((uint16_t)_buff[1]));
  BMP180_readmem(CAL_AC6, 2, _buff);
  ac6 = ((uint16_t)_buff[0] <<8 | ((uint16_t)_buff[1])); 
  BMP180_readmem(CAL_B1, 2, _buff);
  b1 = ((int16_t)_buff[0] <<8 | ((int16_t)_buff[1])); 
  BMP180_readmem(CAL_B2, 2, _buff);
  b2 = ((int16_t)_buff[0] <<8 | ((int16_t)_buff[1])); 
  BMP180_readmem(CAL_MB, 2, _buff);
  mb = ((int16_t)_buff[0] <<8 | ((int16_t)_buff[1]));
  BMP180_readmem(CAL_MC, 2, _buff);
  mc = ((int16_t)_buff[0] <<8 | ((int16_t)_buff[1]));
  BMP180_readmem(CAL_MD, 2, _buff);
  md = ((int16_t)_buff[0] <<8 | ((int16_t)_buff[1])); 
	
	DEBUGL->debug("BMP180 : %02x %02x %02x %02x %02x %02x \r\n",ac1,ac2,ac3,ac4,ac5,ac6);
}


void initBMP180(void)
{
	
}