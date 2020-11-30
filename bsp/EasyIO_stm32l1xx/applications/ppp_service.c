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
#include "stabdby.h"

#include "spiflash_config.h"
#include "gps_tracker.h"
#include "json/cJSON.h"
#include "gps_tracker.h"
#include "sms.h"

#include "movecall.h"
#include "sosapp.h"

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
static void startModem(void);

enum {
	PPPD_STATUS_NONE,
	PPPD_STATUS_INIT,
	PPPD_STATUS_INITMODEM,
	PPPD_STATUS_INITMODEM2,
	PPPD_STATUS_PROTO_START,
	PPPD_STATUS_PROTO_SUCCESS,
	PPPD_STATUS_PROTO_STOP,
	PPPD_STATUS_PROTO_ERROR,
	PPPD_STATUS_RESET_MODEM,
	PPPD_STATUS_PROTO_TIMEOUT = 1000,
};

static int pppd_status = PPPD_STATUS_NONE;						/*PPP ״̬��*/
static int pppd_pd = -1;															/*PPP�߳��ļ�������*/


static unsigned int last_rv_serial_data_jiff = 0;			/*���ڽ������ݶ�ʱ��*/
void recv_serial_data_fn(void *p)
{
	last_rv_serial_data_jiff = app_timer_data.app_timer_second;
}



// ���MCU��GPRSģ��������ͨѶ�Ƿ�����
static void check_ppp_timeout(void)
{
	
	int SERIAL_COMM_TIMEOUT = 60*10;
		
	if (last_rv_serial_data_jiff == 0)
		last_rv_serial_data_jiff = app_timer_data.app_timer_second;
	
	DEBUGL->debug("recv ppp serial data timep %d \r\n",app_timer_data.app_timer_second - last_rv_serial_data_jiff);
	
	//���PPP 
	switch(pppd_status)
	{
		case PPPD_STATUS_PROTO_SUCCESS:
		{
			//�������PPP��������״̬�³��� SERIAL_COMM_TIMEOUT ��δ�յ�����������Ͽ���ǰPPP������������
			//���XMPP ����δ����������4������3Gģ��û�з�Ӧ�ͽ��и�λ
			if (tcp_client_buffer[XMPP_TCP_CLIENT].active_status == TCP_CONNECTED)
				SERIAL_COMM_TIMEOUT = 60 * 10;	//10����
			else	
				SERIAL_COMM_TIMEOUT = 60 * 4;	//4����
			
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
	//debug_buf("RCV MODEM",data,len);
	write_ppp(data ,len);
}


static int do_dial(void)
{
	
	
	int i=0;
	int j=0;
	char atcmd_buf[64];
	int at_cmd_ret_code;
	char apn_cnt = 0;
	char apn_cnt_index = 0;
	const APN_TABLE *atb;
	
//	apn_cnt = get_apn_cnt(network_code);
//	
//	
//	for(i=0;i<apn_cnt;i++)
//	{
//		
//		atb = get_apn(network_code,i);
//		if (atb > 0)
//		{
//			DEBUGL->debug("GET APN NAME %d %s \r\n",network_code,atb->apn);
//			snprintf(atcmd_buf,sizeof(atcmd_buf),"AT+CGDCONT=1,\"IP\",\"%s\"\r\n",atb->apn);
//			cmux_at_command(1,atcmd_buf,AT_CGDCONT,200,&at_cmd_ret_code);
//			
//			cmux_at_command(1,"ATD*99***1#\r\n",AT_ATD,6000,&at_cmd_ret_code);
//			if (at_cmd_ret_code == AT_RESP_CONNECT)
//			{
//				tracker_private_data.apn = atb;
//				pppSetAuth(PPPAUTHTYPE_PAP,atb->username,atb->password);
//				
//				break;
//			}
//			
//		} else {
//			break;
//		}
//		
//	}
//	
//	
//	
//	if (tracker_private_data.apn == 0)
//		return -1;
	
	
	DEBUGL->debug("GET APN NAME %d %s \r\n",network_code,tracker_private_data.conf->APNINFO.APN);
	snprintf(atcmd_buf,sizeof(atcmd_buf),"AT+CGDCONT=1,\"IP\",\"%s\"\r\n",tracker_private_data.conf->APNINFO.APN);
	cmux_at_command(1,atcmd_buf,AT_CGDCONT,200,&at_cmd_ret_code);
	cmux_at_command_wait(1,"ATD*99***1#\r\n",AT_ATD,6000,&at_cmd_ret_code);
	if (at_cmd_ret_code == AT_RESP_OK)
	{
		pppSetAuth(PPPAUTHTYPE_PAP,tracker_private_data.conf->APNINFO.USERNAME,tracker_private_data.conf->APNINFO.PASSWORD);
	} else {
		return -1;
	}

	
	
	cmux_ctrl(1,VPORT_RAWDATA_TYPE,serial_vport_recv_ppp_data);
	return 0;
}

void __RESET_PPP(void)
{
	pppd_status = PPPD_STATUS_RESET_MODEM;
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
				//����ʧ��
				rt_reset_system();
			}
			pppd_status = PPPD_STATUS_PROTO_START;
			break;
			
		case PPPD_STATUS_INITMODEM2:
			pppReOpen(pppd_pd);
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
		
			
			//�Ƚϼ��˵�������ֱ�Ӹ�λϵͳ�����ڿ���Ч������
			//rt_reset_system();
			__RESET_PPP();
		
			//�����ִ��󣬻��� PPP ��ʱ��ʱ�����ٵ�ǰPPP ����
			LOCK_TCPIP_CORE();
			disconn_tcp_client_buf();
			pppClose(pppd_pd);
			UNLOCK_TCPIP_CORE();
			pppd_status = PPPD_STATUS_PROTO_STOP;
			break;
		case PPPD_STATUS_PROTO_START:
			break;
		case PPPD_STATUS_PROTO_SUCCESS:

			rout_tcp_client_buf();
		
			//__RESET_PPP();
			//___flicker_led(2,100);
			break;
		case PPPD_STATUS_RESET_MODEM:
			
			//���¿���modem
			enable_dtr_always(1);
			startModem();
			enable_dtr_always(0);
			LOCK_TCPIP_CORE();
			disconn_tcp_client_buf();
			pppClose(pppd_pd);
			//Free ALL PCB
			UNLOCK_TCPIP_CORE();
			pppd_status = PPPD_STATUS_PROTO_STOP;
			//
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
		case -6:
			break;
		case -8:
		case -5:
			pppd_status = PPPD_STATUS_PROTO_ERROR;
			break;
		case 0:
			//config_ring_interrupt();
			//DEBUGL->debug("config_ring_interrupt !\r\n");
			pppd_status = PPPD_STATUS_PROTO_SUCCESS;
		default:
			break;
	
	}
}

void system_info(void* parameter)
{

		
}


void watchdog_feed(void* parameter)
{

		rt_uint32_t meminfo_total, meminfo_used, meminfo_maxused;
		rt_memory_info(&meminfo_total,&meminfo_used,&meminfo_maxused);
		DEBUGL->debug("stm32 meminfo  jiff:%u total:%u used:%u maxused:%u\n",1,meminfo_total,meminfo_used,meminfo_maxused);
}

static int enable_lowpower_mod(void)
{
	int at_cmd_ret_code;

	cmux_at_command(1,"AT+UPSV=3\r\n",AT_AT,200,&at_cmd_ret_code);
	if (AT_RESP_OK != at_cmd_ret_code)
	{
		return -1;
	}
	
	cmux_at_command(1,"AT+URING=0\r\n",AT_AT,200,&at_cmd_ret_code);
	if (AT_RESP_OK != at_cmd_ret_code)
	{
		return -1;
	}
	
//	cmux_at_command(1,"AT+UGPIOC=42,11\r\n",AT_AT,200,&at_cmd_ret_code); //����Ƿ��������
//	cmux_at_command(1,"AT+UGPIOC=42,0\r\n",AT_AT,200,&at_cmd_ret_code);	//��ΪOUTPUTģʽ
//	if (AT_RESP_OK != at_cmd_ret_code)
//	{
//		return -1;
//	}
//	
//	cmux_at_command(1,"AT+UGPIOW=42,1\r\n",AT_AT,200,&at_cmd_ret_code);	//��Ϊ output Ϊ 1
//	if (AT_RESP_OK != at_cmd_ret_code)
//	{
//		return -1;
//	}
	
	return 0;
		
}

static void startModem(void)
{
	int at_cmd_ret_code;
	__pause_cmux_thread(1);
	
	enable_dtr();
	
	//����MODEM��������CMUX
	for(;;)
	{
		init_sim900_modem();
				
		if (__restart_cmux() == RT_EOK)
			break;
		
	}
	
	

	
	/*
		AT+CSDH=1
		AT+CMGF=1
		AT+CNMI=2,2
		AT+URING=0
	*/
	
	cmux_at_command(2,"AT+CSDH=1\r\n",AT_AT,200,&at_cmd_ret_code);
	cmux_at_command(2,"AT+CMGF=1\r\n",AT_AT,200,&at_cmd_ret_code);
	cmux_at_command(2,"AT+CNMI=2,2\r\n",AT_AT,200,&at_cmd_ret_code);
	
	
	
	//�������͹���ģʽʧ�ܣ�����������ϵͳ
	if (enable_lowpower_mod() != 0)
	{
		rt_reset_system();
	}
	
}

static void startModem_chk(void)
{
	int at_cmd_ret_code;
	__pause_cmux_thread(1);
	
	enable_dtr();
	
	//����MODEM��������CMUX
	for(;;)
	{
		init_sim900_modem_chk();
				
		if (__restart_cmux() == RT_EOK)
			break;
		
	}
	
	

	
	/*
		AT+CSDH=1
		AT+CMGF=1
		AT+CNMI=2,2
		AT+URING=0
	*/
	
	cmux_at_command(2,"AT+CSDH=1\r\n",AT_AT,200,&at_cmd_ret_code);
	cmux_at_command(2,"AT+CMGF=1\r\n",AT_AT,200,&at_cmd_ret_code);
	cmux_at_command(2,"AT+CNMI=2,2\r\n",AT_AT,200,&at_cmd_ret_code);
	
	
	
	//�������͹���ģʽʧ�ܣ�����������ϵͳ
	if (enable_lowpower_mod() != 0)
	{
		rt_reset_system();
	}
	
}

//init_sim900_modem_chk

static void recv_serial_data(unsigned char *buf , int len)
{
	DEBUGL->debug("recv serial data [%d][%s] \r\n",len,buf);
	//
}



static void check_user_config_val(void)
{
		int is_need_save_flash = -1;

	if (strlen(user_app_conf.HOST_ADDRESS) < 5)
	{
		snprintf(user_app_conf.HOST_ADDRESS,sizeof(user_app_conf.HOST_ADDRESS),"%s",DEFAULT_HOSTNAE);
		is_need_save_flash = 1;
	}
	
	if (user_app_conf.HOST_PORT <= 0)
	{
		user_app_conf.HOST_PORT = DEFAULT_HOSTPORT;
		is_need_save_flash = 1;
	}
	
	if ( user_app_conf.PRESENCE_UPLOAD_PREIOD < 5 || user_app_conf.PRESENCE_UPLOAD_PREIOD > 86400  ) {
		user_app_conf.PRESENCE_UPLOAD_PREIOD = DEFAULT_UPLOAD_PREIOD;
	}

	if ( is_need_save_flash == 1 ) {
		userapp_writeconfig();
		spiflash_powerdown();
	}
}


#ifdef ENABLE_SERIAL_DEBUG_AT

#endif

const char versionstr[] = VERSION_JSON_STR;

//void initBKP(void)
//{
//	RCC_APB1PeriphClockCmd(RCC_APPeriph_BKP, ENABLE);
//}

static void install_fw(void)
{
	DEBUGL->debug("INSTALL VERSION: %s \r\n",versionstr);
}

void mainloop(void *p)
{
	extern unsigned char enable_bianpin;
	
	uint16_t spi_flash_id;
	int at_cmd_ret_code;
	GPIO_InitTypeDef GPIO_InitStructure;
	
		
	#define FW_VERSION_H			1
	#define FW_VERSION_L			0
	
//	noinit ++;
//	DEBUGL->debug("NO init ram = %d \r\n",noinit);
//	(*(int(*)())0)(); 
	
	
	DEBUGL->debug("@@@@@@@@@@@@@@@@@@@@@@@@ Version %d.%d BUILD DATE %s TIME %s \r\n",FW_VERSION_H,FW_VERSION_L,__DATE__,__TIME__);
	DEBUGL->debug("version json %s \r\n",versionstr);
	
	enable_wg();
	
	config_fq_2(); //����Ƶ�� �� 2M���ȣ��Խ�ʡ����
	
	//��ʼ������
	INIT_TRACKER_PRIVATE;
	
	//��ʼ��MODEM
	init_led_hw();
	init_ublox_hw();
	IOI2C_Init();
	init_modem_hw();
	//����ring
	config_ring_interrupt();
	//init_mma8452_int();
	//init_mma845x();
	config_mma845x_interrupt();
	initADC();
	init_gnss();
	
	//��ʼ��SPIFlash
	SPI_Flash_Init();
	spi_flash_id = SPI_Flash_ReadID();
	DEBUGL->debug("SPI Flash ID %X \r\n",spi_flash_id);
	
	__disable_AMP_EN();
	
	disableADC();
	config_fq_1(); //����Ƶ�� �� 2M���ȣ��Խ�ʡ����
	
	
	//private ��ֵ
	tracker_private_data.spiflashID = spi_flash_id;
	tracker_private_data.mma845xTest = init_mma845x();
	
	//�޸������ļ�����������̼����гɹ����
	readconfig();
	userapp_readconfig();
	check_user_config_val();
	debug_user_config();
	//Ϊ�˱�֤����������led������˸
	disable_RGB_led = 1;
	//��ʼ��Timer
	init_gps_tracker_timer();
	init_flicker_led_tmr();
	
	//m_define_trans_threshold_set
	#ifdef ALWAYS_ON
	tracker_private_data.SYSTEM_STATUS = SYSTEM_POWER_ON;
	#else
	switch(tracker_private_data.mma8452_power_on_off)
	{
		case 0x00:
			sys_power_off();
			break;
		case 0x01:
			tracker_private_data.SYSTEM_STATUS = SYSTEM_POWER_ON;
			break;
			
		case 0x02:
			tracker_private_data.SYSTEM_STATUS = SYSTEM_CONFIG_MOD;
			tracker_private_data.test_flag = 1;
			break;
	}
	
	#endif
	
	if (tracker_private_data.SYSTEM_STATUS == SYSTEM_POWER_OFF)
	{
		//����ǹػ�״̬��������ػ�
		for(;;){
			sys_power_off();
			DEBUGL->debug("POWER OFF !!!!");
			rt_thread_sleep(500);
		}
	}
	
//	if (FUNC_KEY)
//	{
//		tracker_private_data.test_flag = 1;
//		DEBUGL->debug("##########################################\r\n");
//		DEBUGL->debug("##########################################\r\n");
//		DEBUGL->debug("##########################################\r\n");
//		DEBUGL->debug("##########################################\r\n");
//		DEBUGL->debug("#################TESTMOD##################\r\n");
//		DEBUGL->debug("##########################################\r\n");
//		DEBUGL->debug("##########################################\r\n");
//		DEBUGL->debug("##########################################\r\n");
//		DEBUGL->debug("##########################################\r\n");
//		DEBUGL->debug("##########################################\r\n");
//		
//		m_define_trans_threshold_set(SYSTEM_CONFIG_MOD);
//		
//		for(;;)
//		{
//			if(!FUNC_KEY)
//			{
//				rt_reset_system();
//				
//			}
//		}
//		
//	} else {
//		
//		DEBUGL->debug("******************************************\r\n");
//		DEBUGL->debug("******************************************\r\n");
//		DEBUGL->debug("******************************************\r\n");
//		DEBUGL->debug("******************************************\r\n");
//		DEBUGL->debug("******************************************\r\n");
//		DEBUGL->debug("******************************************\r\n");
//		DEBUGL->debug("******************************************\r\n");
//		DEBUGL->debug("******************************************\r\n");
//		DEBUGL->debug("******************************************\r\n");
//		
//		
//	}
//	
	disable_RGB_led = 0;
	

	if(spiflash_conf.ota_status_machine != OTA_STATUS_MACHINE_NONE)
	{
		//������ְ汾�������Ķ�������ִ��install����
		install_fw();
		
		spiflash_conf.CURRENT_FW_VERSION_H = FW_VERSION_H;
		spiflash_conf.CURRENT_FW_VERSION_L = FW_VERSION_L;
		
		spiflash_conf.ota_status_machine = OTA_STATUS_MACHINE_NONE;
		writeconfig();
	}


	if ((spiflash_conf.CURRENT_FW_VERSION_H != FW_VERSION_H) || (spiflash_conf.CURRENT_FW_VERSION_L != FW_VERSION_L))
	{
		//��ǰ�汾��
		//������ְ汾�������Ķ�������ִ��INSTALL ����
		install_fw();
		spiflash_conf.CURRENT_FW_VERSION_H = FW_VERSION_H;
		spiflash_conf.CURRENT_FW_VERSION_L = FW_VERSION_L;
		writeconfig();
	}
	

	
	//sys_power_off();
	


	
	//û��SIM��
	SET_LED_MOD(LED_B,BLUE_STATUS_NOSIMCARD);
	
	// read config
  pppSetAuth(PPPAUTHTYPE_PAP,NETWORK_PPP_USERNAME,NETWORK_PPP_PASSWORD);
	init_tcp_client_buf();
	
//	PC5 �� POWER_EN
//	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
//	GPIO_Init(GPIOC, &GPIO_InitStructure);
//	GPIO_SetBits(GPIOC,GPIO_Pin_5);
	
	
	//���Ź���ʼ��
	//enable_wg();
	

	//��ʼ��Э��ջ
	lwip_system_init();
	pppInit();

	//��GPS��MODEM����
	open_gps_uart("uart2");
	open_modem_serial_port("uart3");
	
	//recv_serial_data(unsigned char *buf , int length);
	serial_handle[0] = recv_serial_data;
	init_user_serial_service("uart1",0);
	
	
	__init_cmux_thread();
	
	
	#ifdef TRACKER_TST
	//tracker_private_data.test_flag = 1;
	if (tracker_private_data.test_flag == 1)
	{
		int at_cmd_ret_code;
		startModem_chk();
		
		

		__disable_AMP_EN();
//		cmux_at_command(2,"AT+UEXTDCONF=0,1\r\n",AT_AT,200,&at_cmd_ret_code);
//		cmux_at_command(2,"AT+USGC=1,6,6,16000,16000,22000\r\n",AT_AT,200,&at_cmd_ret_code);
//		
		//cmux_at_command("AT+UEXTDCONF=0,1\r\n",AT_AT,200,&at_cmd_ret_code);
		//at_command("AT+USGC=1,6,6,16000,16000,22000\r\n",AT_AT,200,&at_cmd_ret_code);
		
		tracker_private_data.bmp180ID = BMP180_getCalData();
		
		
		
		//����ota����
		//ota_start_http_download(0,0);
		DISABLE_DEBUG_LEVEL_DEBUG		=		1;
		DISABLE_DEBUG_LEVEL_INFO		=		0;
		DISABLE_DEBUG_LEVEL_WARN		=		1;
		DISABLE_DEBUG_LEVEL_FATAL		=		1;
		DISABLE_DEBUG_LEVEL_ERROR		=		1;
		
		
		
		
		for(;;)
		{
			
			DEBUGL->info("TEST FW Version 1.0.1\r\n");
			DEBUGL->info("IMEI:                     [%s]\r\n",modem_imei);
			DEBUGL->info("GSMSIGNAL:                [%d]\r\n",gsm_signal);
			DEBUGL->info("GPSSIGNAL:                [%d]\r\n",tracker_private_data.gps_signal);
			DEBUGL->info("BATVAL:                   [%d]\r\n",get_bat_vol());
			
			if (tracker_private_data.bmp180ID  == 85)
				DEBUGL->info("BMP180:                   [OK]\r\n");
			else
				DEBUGL->info("BMP180:                   [FAIL]\r\n");
			
			if (tracker_private_data.mma845xTest == 1)
				DEBUGL->info("MMA845:                   [OK]\r\n");
			else
				DEBUGL->info("MMA845:                   [FAIL]\r\n");
			
			if (tracker_private_data.spiflashID == 61204)	
				DEBUGL->info("SPIFLASH:                 [OK]\r\n");
			else
				DEBUGL->info("SPIFLASH:                 [FAIL]\r\n");
			
			
			
			cmux_at_command(2,"AT+CSQ\r\n",AT_CSQ,300,&at_cmd_ret_code);
			
			led_on(0);
			led_on(1);
			led_on(2);
			mot_on();
			rt_thread_sleep(RT_TICK_PER_SECOND);
			led_off(0);
			led_off(1);
			led_off(2);
			mot_off();
		}
		
	}
	#endif
	
	
	

	
	//
	startModem();
	//startModem();
	//open_gps_5min();
	

	
	#ifdef ENABLE_SERIAL_DEBUG_AT
	for(;;){
		rt_thread_sleep(100);
	}
	#endif
	
	//��ʼ��Ӧ�ò�
	init_xmpp_tcp();
	init_ota_http_cli();
	
	//ע����Żص�����
	init_sms_callback();
	
	//״̬���ж�
	for(;;)
	{
		int at_cmd_ret_code = -1;
		call_routing();
		sos_routing();
		ppp_routing(0);	
		lowpower_routing(0);
		ota_http_tcp_routing(0);
		setting_presence_routring(0);
		generate_send_presence(1);
		webtrace_routing(0);
		rt_thread_sleep(RT_TICK_PER_SECOND*1);
		
	
	}
	return ;
}

void check_led(void)
{
	
	//�����Ԥ�ػ�������Ԥ�������ʹ�LED�ƣ��������,�����������״̬��һֱ�ر�MOT
	if (tracker_private_data.pre_poweroff_on == 1)
	{
		if ((app_timer_data.app_timer_second - tracker_private_data.pre_poweroff_on_time ) <= 1)
		{
			SET_LED_MOD(LED_R,LED_STATUS_ON);
			SET_LED_MOD(LED_G,LED_STATUS_ON);
			SET_LED_MOD(LED_B,LED_STATUS_ON);
			mot_on();
		}else{
			
			SET_LED_MOD(LED_R,RGB_LED_OFF);
			SET_LED_MOD(LED_G,RGB_LED_OFF);
			SET_LED_MOD(LED_B,RGB_LED_OFF);
			mot_off();
			
		}
		goto RETU;
	}else{
		mot_off();
	}
	
	
	if (tracker_private_data.pre_poweroff_on == 2)
	{
			config_fq_2();		//�ָ���32M
			switch(tracker_private_data.SYSTEM_STATUS)
			{
				case SYSTEM_POWER_OFF:
					if (tracker_private_data.test_flag == 1)
					{
						tracker_private_data.SYSTEM_STATUS = SYSTEM_CONFIG_MOD;
						m_define_trans_threshold_set(0x2);
						//
					}else {
						tracker_private_data.SYSTEM_STATUS = SYSTEM_POWER_ON;
						m_define_trans_threshold_set(0x1);
					}

					break;
				case SYSTEM_POWER_ON:
				case SYSTEM_CONFIG_MOD:
					
				//�����������ģʽ�¶�Ҫִ�йػ�����
					tracker_private_data.SYSTEM_STATUS = SYSTEM_POWER_OFF;
					//userapp_writeconfig();
					m_define_trans_threshold_set(0x0);
					break;
				default:
					tracker_private_data.SYSTEM_STATUS = SYSTEM_POWER_ON;
					//userapp_writeconfig();
					m_define_trans_threshold_set(0x1);
					break;
					
			}
			
//			switch(tracker_private_data.SYSTEM_STATUS)
//			{
//				case SYSTEM_POWER_ON:
//					break;
//				case SYSTEM_POWER_OFF:
//					break;
//				case SYSTEM_CONFIG_MOD:
//					break;
//				default:
//					break;
//			}
	
	
			
			//��λϵͳ
			rt_reset_system();
			
			goto RETU;
	
	}
	
		//������ڳ�磬һ����֮��û�а��¹�,XMPP�ɹ���½ , XMPP�ɹ���½���� 60�� ��Ϩ��LED
	if (((app_timer_data.app_timer_second - charge_disable_tim) > 20)&&((app_timer_data.app_timer_second - pwr_press_time) > 60) && (!IS_CHARGE) && (xmpp_pma.xmpp_status == XMPP_BINDSUCC) && ((app_timer_data.app_timer_second - xmpp_login_timep) > 60))    
	{
		disable_RGB_led = 1;
			//
	} else {
		disable_RGB_led = 0;
	}
		
		//���״̬
	if (tracker_private_data.SYSTEM_STATUS == SYSTEM_POWER_ON)
	{
			
		if (IS_CHARGE)
		{
			charge_disable_tim = app_timer_data.app_timer_second;
			SET_LED_MOD(LED_R,RED_STATUS_CHARGE);
		}
		else
		{
			SET_LED_MOD(LED_R,RED_STATUS_ON);
		}
		
		//���GPS״̬
		if ((GPS_LOWPOWER_STATUS == GPS_ON) || (GPS_LOWPOWER_STATUS == GPS_NOPOS))
		{
			if (gps_info.gprmc.status == 'A')
			{
				SET_LED_MOD(LED_G,GREEN_STATUS_GPSA);
			}else{
				SET_LED_MOD(LED_G,GREEN_STATUS_GPSV);
			}
		}
		else
		{
			SET_LED_MOD(LED_G,GREEN_STATUS_GPSOFF);
		}
	
	
		
	}
	else
	if (tracker_private_data.SYSTEM_STATUS == SYSTEM_POWER_OFF)
	{
		//��ư�������
		if (IS_CHARGE)
		{
			//д��ʱ�䣬��֤�ε������20����Ȼ������
			SET_LED_MOD(LED_R,RED_STATUS_CHARGE);
		}
		else
		{
			SET_LED_MOD(LED_R,RGB_LED_OFF);
		}
		
		//���ƣ��̵ƶ��ر�
		SET_LED_MOD(LED_G,RGB_LED_OFF);
		SET_LED_MOD(LED_B,RGB_LED_OFF);
	}
	else
	if(tracker_private_data.SYSTEM_STATUS == SYSTEM_CONFIG_MOD)
	{
		SET_LED_MOD(LED_R,LED_STATUS_ON);
		SET_LED_MOD(LED_G,LED_STATUS_ON);
		SET_LED_MOD(LED_B,LED_STATUS_ON);
		
		goto RETU;
		
		//
	}
	
	
	
	
		//��� XMPP״̬
	switch(pppd_status)
	{
		case PPPD_STATUS_PROTO_SUCCESS:
		{
			switch(xmpp_pma.xmpp_status)
			{
				
				case XMPP_SINIT:
				case XMPP_SLOGIN:
				case XMPP_SLOGINSUCC:
				case XMPP_BINDING:
					SET_LED_MOD(LED_B,BLUE_STATUS_LOGIN);
					break;
				case XMPP_BINDSUCC:
					SET_LED_MOD(LED_B,BLUE_STATUS_SUCCESS);
			}
			
			break;
		}
		default:
			SET_LED_MOD(LED_B,BLUE_STATUS_REGISTER);
			
	};
	
	RETU:
	return ;
}

