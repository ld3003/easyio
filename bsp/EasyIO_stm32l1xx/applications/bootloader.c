#include <rtthread.h>
#include <components.h>

#include "common.h"

#include "ppp_service.h"
#include "modem_serial.h"
#include "netif/ppp/ppp.h"
#include "sim900.h"
#include "lwip_raw_api_test.h"
#include "lwip_raw_api_tcp_template.h"
#include "XMPP/xmpp.h"
#include "app_ev.h"
#include "ublox_gps.h"
#include "xmpp_tcp.h"
#include "serial_srv.h"
#include "IOI2C.h"
#include "MMA845X.h"
#include "gnss.h"
#include "led.h"
#include "lowpower.h"
#include "ota_http_tcp.h"
#include "app_timer.h"
#include "watch_dog.h"
#include "gsmmux/easyio_cmux.h"
#include "at_cmd.h"
#include "my_stdc_func/my_stdc_func.h"
#include "adc.h"
#include "lowpower.h"
#include "serial_srv.h"

#include "my_stdc_func/debugl.h"
#include "w25q16.h"

extern rt_err_t rt_thread_sleep(rt_tick_t tick);

#if 0
/* Error codes. */
#define PPPERR_NONE      0 /* No error. */
#define PPPERR_PARAM    -1 /* Invalid parameter. */
#define PPPERR_OPEN     -2 /* Unable to open PPP session. */
#define PPPERR_DEVICE   -3 /* Invalid I/O device for PPP. */
#define PPPERR_ALLOC    -4 /* Unable to allocate resources. */
#define PPPERR_USER     -5 /* User interrupt. */
#define PPPERR_CONNECT  -6 /* Connection lost. */
#define PPPERR_AUTHFAIL -7 /* Failed authentication challenge. */
#define PPPERR_PROTOCOL -8 /* Failed to meet protocol. */
#endif

extern int lwip_system_init(void);
extern void TCP_Client_Routing(void);
static void linkStatusCB(void *ctx, int errCode, void *arg);

enum {
	PPPD_STATUS_NONE,
	PPPD_STATUS_INIT,
	PPPD_STATUS_INITMODEM,
	PPPD_STATUS_PROTO_START,
	PPPD_STATUS_PROTO_SUCCESS,
	PPPD_STATUS_PROTO_STOP,
	PPPD_STATUS_PROTO_ERROR,
	PPPD_STATUS_PROTO_TIMEOUT = 1000,
};

static int pppd_status = PPPD_STATUS_NONE;						/*PPP 状态机*/
static int pppd_pd = -1;															/*PPP线程文件描述符*/


static unsigned int last_rv_serial_data_jiff = 0;			/*串口接收数据定时器*/
void recv_serial_data_fn(void *p)
{
	last_rv_serial_data_jiff = app_timer_data.app_timer_second;
}
// 检查MCU与GPRS模块间的数据通讯是否正常
static void check_ppp_timeout(void)
{
	
	#define SERIAL_COMM_TIMEOUT 60*10
		
	if (last_rv_serial_data_jiff == 0)
		last_rv_serial_data_jiff = app_timer_data.app_timer_second;
	
	DEBUGL->debug("recv ppp serial data timep %d \r\n",app_timer_data.app_timer_second - last_rv_serial_data_jiff);
	
	//如果PPP 
	switch(pppd_status)
	{
		case PPPD_STATUS_PROTO_SUCCESS:
		{
			//如果是在PPP正常连接状态下超过 SERIAL_COMM_TIMEOUT 秒未收到串口数据则断开当前PPP进行重新连接
			if ((app_timer_data.app_timer_second - last_rv_serial_data_jiff) > SERIAL_COMM_TIMEOUT)
			{
				pppd_status = PPPD_STATUS_PROTO_TIMEOUT;
				DEBUGL->debug("Serial Timeout \r\n");
				last_rv_serial_data_jiff = app_timer_data.app_timer_second;
			}
			break;
		}
		default:
			last_rv_serial_data_jiff = app_timer_data.app_timer_second;
			break;
	}
	
	//
}

extern void write_ppp(unsigned char *data , int len);
extern void recv_serial_data_fn(void *p);
static void serial_vport_recv_ppp_data(unsigned char *data , int len)
{
	recv_serial_data_fn(0);
	write_ppp(data ,len);
}


static int do_dial(void)
{
	
	/*
	进行5次拨号，如果失败则退出
	*/
	
	int i=0;
	char atcmd_buf[64];
	int at_cmd_ret_code;
	for(i=1;i<=5;i++)
	{
		DEBUGL->debug("GET APN NAME %s \r\n",get_apn(network_code));
		snprintf(atcmd_buf,sizeof(atcmd_buf),"AT+CGDCONT=1,\"IP\",\"%s\"\r\n",get_apn(network_code));
		cmux_at_command(1,atcmd_buf,AT_CGDCONT,200,&at_cmd_ret_code);
		
		if (at_cmd_ret_code == AT_RESP_OK)
			break;
		
		if (i==5)
			return -1;
		
	}
	
	
	for(i=1;i<=5;i++)
	{
		cmux_at_command(1,"ATD*99***1#\r\n",AT_ATD,6000,&at_cmd_ret_code);
		if (at_cmd_ret_code == AT_RESP_CONNECT)
			break;
		
		if (i==20)
			return -1;
		
	}
	
	
	cmux_ctrl(1,VPORT_RAWDATA_TYPE,serial_vport_recv_ppp_data);
	return 0;
}

static void ppp_routing(void *p)
{
	int at_cmd_ret_code;
	check_ppp_timeout();
	switch(pppd_status)
	{
		case PPPD_STATUS_NONE:
			pppd_status = PPPD_STATUS_INITMODEM;
			break;
		case PPPD_STATUS_INITMODEM:
			
			if (pppd_pd < 0)
				pppd_pd = pppOpen(rt_device_find("uart3"),linkStatusCB,0);
			else
			{
				//close tcp
				pppReOpen(pppd_pd);
			}
			
			if (do_dial() == -1)
			{
				//拨号失败
				rt_reset_system();
			}
			pppd_status = PPPD_STATUS_PROTO_START;
			break;
		case PPPD_STATUS_PROTO_STOP:
			pppd_status = PPPD_STATUS_NONE;
			break;
		case PPPD_STATUS_PROTO_TIMEOUT:
		case PPPD_STATUS_PROTO_ERROR:
			
		
			//checkat
			#if 1
			//cmux_at_command();
			cmux_at_command(2,"AT+CSQ\r\n",AT_AT,200,&at_cmd_ret_code);
			#endif
		
			
			//比较极端的做法，直接复位系统，现在看来效果不错
			rt_reset_system();
		
			//当出现错误，或者 PPP 超时的时候将销毁当前PPP 链接
			LOCK_TCPIP_CORE();
			disconn_tcp_client_buf();
			pppClose(pppd_pd);
			UNLOCK_TCPIP_CORE();
			pppd_status = PPPD_STATUS_PROTO_STOP;
			break;
		case PPPD_STATUS_PROTO_START:
		case PPPD_STATUS_PROTO_SUCCESS:

			rout_tcp_client_buf();
			___flicker_led(2,100);
			break;
		default:
			break;
	}
//
}


static void linkStatusCB(void *ctx, int errCode, void *arg)
{
	DEBUGL->debug("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ PPP STATUS %d\n",errCode);
	switch(errCode)
	{
		case -8:
			pppd_status = PPPD_STATUS_PROTO_ERROR;
			
			break;
		case -6:
			break;
		case 0:
			config_ring_interrupt();
			DEBUGL->debug("config_ring_interrupt !\r\n");
			pppd_status = PPPD_STATUS_PROTO_SUCCESS;
		default:
			break;
	
	}
}

void system_info(void* parameter)
{

		rt_uint32_t meminfo_total, meminfo_used, meminfo_maxused;
		rt_memory_info(&meminfo_total,&meminfo_used,&meminfo_maxused);
		DEBUGL->debug("stm32 meminfo  jiff:%u total:%u used:%u maxused:%u\n",1,meminfo_total,meminfo_used,meminfo_maxused);
    //rt_thread_delay( RT_TICK_PER_SECOND);
	
		DEBUGL->debug("XMPP PING_CNT:%d \r\n",xmpp_pma.recv_ping_cnt);
}

void system_check(void *p)
{
}

void watchdog_feed(void* parameter)
{

		rt_uint32_t meminfo_total, meminfo_used, meminfo_maxused;
		rt_memory_info(&meminfo_total,&meminfo_used,&meminfo_maxused);
		DEBUGL->debug("stm32 meminfo  jiff:%u total:%u used:%u maxused:%u\n",1,meminfo_total,meminfo_used,meminfo_maxused);
}

static void enable_lowpower_mod(void)
{
	int at_cmd_ret_code;
	for(;;)
	{

		//cmux_at_command(2,"AT+UPSV=1,2000\r\n",AT_AT,200,&at_cmd_ret_code);
		//AT&K3
		cmux_at_command(1,"AT&K0\r\n",AT_AT,200,&at_cmd_ret_code);
		//cmux_at_command(1,"AT+UPSV=1,500\r\n",AT_AT,200,&at_cmd_ret_code);
		cmux_at_command(1,"AT+UPSV=3\r\n",AT_AT,200,&at_cmd_ret_code);
		cmux_at_command(1,"AT+URING=3\r\n",AT_AT,200,&at_cmd_ret_code);
		//cmux_at_command(2,"AT&K0\r\n",AT_AT,200,&at_cmd_ret_code);
		//cmux_at_command(2,"ATK3=3\r\n",AT_AT,200,&at_cmd_ret_code);
		if (AT_RESP_OK == at_cmd_ret_code)
			break;
	}
		
	//
}

static void startModem(void)
{
	
	__pause_cmux_thread(1);
	
	//启动MODEM，并开启CMUX
	for(;;)
	{
		init_sim900_modem();
		if (__restart_cmux() == RT_EOK)
			break;
		
	}
	

	#ifdef DISABLE_POWERSAVING_3G
	#else
	enable_lowpower_mod();
	#endif
	//
}

#ifdef ENABLE_SERIAL_DEBUG_AT
static void recv_serial_data(unsigned char *buf , int len)
{
	DEBUGL->debug("recv serial data [%s] \r\n",buf);
	//
}
#endif

#include "bootloader.h"
typedef  void (*pFunction)(void);
void JumpToApp(u32 appAddr)
{
	pFunction JumpToApplication;
	u32 JumpAddress;
	
	//appAddr = CPU2_FW_ADDRESS;
	
	JumpAddress = *(u32*) (appAddr + 4);
	JumpToApplication = (pFunction)JumpAddress;
  /* Initialize user application's Stack Pointer */
  __set_MSP(*(vu32*) appAddr);
  JumpToApplication();
}


static unsigned int power_key_input_counter = 0;
void mainloop(void *p)
{
	
	extern unsigned char enable_bianpin;
	
	uint16_t spi_flash_id;
	int at_cmd_ret_code;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	DEBUGL->debug("@@@@@@@@@@@@@@@@@@@@@@@@ BUILD DATE %s TIME %s \r\n",__DATE__,__TIME__);
	


	//初始化MODEM
	init_led_hw();
	init_gnss_hw();
	IOI2C_Init();
	init_modem_hw();
	init_mma8452_int();
	init_mma845x();
	initADC();
	init_gnss();
	
	SPI_Flash_Init();
	spi_flash_id = SPI_Flash_ReadID();
	DEBUGL->debug("SPI Flash ID %X \r\n",spi_flash_id);
	
	JumpToApp(EASYIO_ADDRESS);
	
	
	//test_flash_md5();


	
	led_on(0);
	led_on(1);
	led_on(2);
	
	init_flicker_led_tmr();
	
	// read config
  pppSetAuth(PPPAUTHTYPE_PAP,NETWORK_PPP_USERNAME,NETWORK_PPP_PASSWORD);
	init_tcp_client_buf();
	

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC,GPIO_Pin_5);
	
	
	//看门狗初始化
	//enable_wg();
	

	//初始化协议栈
	lwip_system_init();
	pppInit();

	//打开GPS与MODEM串口
	open_gps_uart("uart2");
	open_modem_serial_port("uart3");
	
	__init_cmux_thread();
	
	
	startModem();
	
		
	#ifdef ENABLE_SERIAL_DEBUG_AT
	serial_handle[0] = recv_serial_data;
	init_user_serial_service("uart1",0);
	for(;;){
		rt_thread_sleep(100);
	}
	#endif
	

	//初始化应用层
	init_xmpp_tcp();
	init_ota_http_cli();
	
	
	
	
	//状态机判断
	for(;;)
	{
		int at_cmd_ret_code = -1;
		
		system_info(0);
		system_check(0);
		
		#if 1
		ppp_routing(0);
		
		mma845x_routing(0);
		lowpower_routing(0);
	
		ota_http_tcp_routing(0);
		setting_presence_routring(0);
		#endif

		rt_thread_sleep(RT_TICK_PER_SECOND*1);
		
		get_bat_vol();
		
		#ifdef GPS_DATA_TEST
		test_gpsdata_routing();
		#endif
		
		DEBUGL->debug("@@@@@@@@@@@@@@@@@@@@@@@@ FRQ @ %d \r\n",SystemCoreClock);
		
		//AT+UGZDA=1
		//cmux_at_command(2,"AT+UGGGA=1\r\n",AT_AT,200,&at_cmd_ret_code);
		//cmux_at_command(2,"AT+UGAOF=\"http://alp.u-blox.com/current_14d.alp\",0,1,3\r\n",AT_AT,200,&at_cmd_ret_code);
		
	}
	return ;
}


