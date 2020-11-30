#ifndef __MMA845X_H__
#define __MMA845X_H__

#include <stm32l1xx.h>
#include "app_timer.h"

char init_mma845x(void);
void start_systimer(void);
void init_mma8452_int(void);
void m_define_trans_threshold_set(unsigned char value);
unsigned char  m_define_trans_threshold_get(unsigned char value);

#define POWER_OFF_VALUE							0		//原始状态 
#define POWER_ON_VALUE							1		//开机状态
#define POWER_STANDBY_VALUE					2		//待机状态

extern unsigned char mma845x_last_value;

extern unsigned int mma8452_moving_time ;
extern unsigned int mma8452_stay_time;


//静止或停留的时间
#define DEVICE_MOV_TIME ((app_timer_data.app_timer_second - mma8452_moving_time)%app_timer_data.app_timer_second)
#define DEVICE_STAY_TIME ((app_timer_data.app_timer_second - mma8452_stay_time)%app_timer_data.app_timer_second)


void mma845x_routing(void *p);
void config_mma845x_interrupt(void);
char init_mma845x(void);

unsigned char BMP180_getCalData(void);

int __is_moving(void);

extern unsigned char mma845x_movingid; //记录这是第几次移动

#define MMA845X_IS_MOVING __is_moving()

#endif