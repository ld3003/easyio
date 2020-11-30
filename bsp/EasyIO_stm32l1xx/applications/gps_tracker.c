#include "gps_tracker.h"
#include <rtthread.h>
#include "app_ev.h"
#include "my_stdc_func/debugl.h"
#include "led.h"
#include "lowpower.h"
#include "gnss.h"
#include "app_timer.h"
#include "XMPP/xmpp.h"
#include "sim900.h"
#include "mma845x.h"
#include "adc.h"
#include "led.h"
#include "XMPP/xmpp.h"
#include "xmpp_tcp.h"
#include "ppp_service.h"

struct TRACKER_PRIVATE tracker_private_data;

static struct rt_timer tracker_tmr;

//每秒执行一次

extern void system_info(void* parameter);
extern unsigned int SystemCoreClock;
extern const char versionstr[];
extern void feed_watchdog();
static void led_control(void)
{
	feed_watchdog();
	check_led();
	
}

extern void check_3gmodem_active(void);
static void tracker_tmr_task(void *p)
{
	
	rt_uint32_t meminfo_total, meminfo_used, meminfo_maxused;
	rt_memory_info(&meminfo_total,&meminfo_used,&meminfo_maxused);
	
	
	led_control();
	mma845x_routing(0);
	check_3gmodem_active();

	if (!IS_SARAU2_ACTIVE)
	{
		DEBUGL->debug("Ublox3g IDLE ! Ublox3g IDLE ! Ublox3g IDLE ! Ublox3g IDLE ! \r\n");
	}else{
		DEBUGL->debug("Ublox3g ACTIVE ! Ublox3g ACTIVE ! Ublox3g ACTIVE ! Ublox3g ACTIVE ! \r\n");
	}
	
	if (tracker_private_data.SYSTEM_STATUS == SYSTEM_POWER_OFF)
	{
		DEBUGL->debug("Power OFF Power OFF Power OFF Power OFF Power OFF\r\n");
		return ;
	}
	
	
	DEBUGL->debug("####### stm32 meminfo  jiff:%u total:%u used:%u maxused:%u\n",1,meminfo_total,meminfo_used,meminfo_maxused);
	DEBUGL->debug("####### XMPP PING_CNT:%d \r\n",xmpp_pma.recv_ping_cnt);
	DEBUGL->debug("####### SYSTEM INFO TIM:%d \r\n",app_timer_data.app_timer_second);
	DEBUGL->debug("####### SARAU2 MODE %d ,FRQ @ %d \r\n",IS_SARAU2_ACTIVE,SystemCoreClock);
	DEBUGL->debug("####### BATVAL:                   [%d]\r\n",get_bat_vol());
	

	DEBUGL->debug("####### %s\r\n",versionstr);



	//
}

static void tracker_tmr_timeout(void *p)
{
	post_hw_int_event(tracker_tmr_task,0,0,0);
	//
}

void init_gps_tracker_timer(void)
{
	memset(&tracker_tmr,0x0,sizeof(tracker_tmr));
	//100ms
	tracker_tmr_task(0);
	
	rt_timer_init(&tracker_tmr,"trk_tmr",tracker_tmr_timeout,0,RT_TICK_PER_SECOND,RT_TIMER_FLAG_PERIODIC);
  rt_timer_start(&tracker_tmr);
	
	
	//
}
