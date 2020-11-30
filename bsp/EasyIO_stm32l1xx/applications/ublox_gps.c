#include <rtthread.h>
#include "common.h"
#include "ublox_gps.h"
#include "gnss.h"
#include "led.h"
#include "app_ev.h"
#include "gnss.h"

#include "gsmmux/easyio_cmux.h"
#include "at_cmd.h"
#include "my_stdc_func/debugl.h"
#include "app_timer.h"
#include <stdio.h>
#include <string.h>
#include <rtthread.h>


#ifdef GPS_DATA_TEST
const char xgps_valid_data[] = "\
$GNRMC,041142.00,A,3907.80639,N,11704.58908,E,0.144,,040915,,,A*62\r\n\
$GNVTG,,T,,M,0.144,N,0.268,K,A*30\r\n\
$GNGGA,041142.00,3907.80639,N,11704.58908,E,1,08,2.57,1.7,M,-6.0,M,,*53\r\n\
$GNGSA,A,3,17,28,01,04,32,11,,,,,,,3.81,2.57,2.81*15\r\n\
$GNGSA,A,3,69,75,,,,,,,,,,,3.81,2.57,2.81*10\r\n\
$GPGSV,3,1,10,01,52,047,33,03,41,119,,04,24,050,27,08,11,088,*7A\r\n\
$GPGSV,3,2,10,11,40,057,31,17,44,297,27,19,18,081,,28,81,296,30*77\r\n\
$GPGSV,3,3,10,30,23,212,,32,30,071,20*7D\r\n\
$GLGSV,2,1,08,68,09,024,,69,28,073,25,70,18,131,,74,27,195,20*6D\r\n\
$GLGSV,2,2,08,75,54,263,24,76,23,326,,84,35,046,20,85,57,331,*6C\r\n\
$GNGLL,3907.80639,N,11704.58908,E,041142.00,A,A*73\r\n\
";


const char *gprmcstr[]  = {"$GPRMC,044723.4,A,2503.067668,N,12135.581009,E,20.1,259.2,040316,,,A*5F\r\n",
													"$GPRMC,044734.4,A,2503.054112,N,12135.491367,E,28.6,259.8,040316,,,A*5D\r\n",
													"$GPRMC,044739.4,A,2503.048793,N,12135.447956,E,29.1,261.3,040316,,,A*57\r\n",
													"$GPRMC,044741.4,A,2503.045465,N,12135.430391,E,27.8,260.1,040316,,,A*5A\r\n",
	
													"$GPRMC,044429.4,A,2504.005267,N,12137.379502,E,0.0,0.0,040316,,,A*6C\r\n",
													"$GPRMC,044445.4,A,2504.018050,N,12137.379069,E,10.2,358.8,040316,,,A*51\r\n",
													"$GPRMC,044456.4,A,2504.101224,N,12137.337484,E,0.0,0.0,040316,,,A*63\r\n",
													"$GPRMC,044509.4,A,2503.967108,N,12137.332146,E,0.0,0.0,040316,,,A*64\r\n",
	
	
													"$GPRMC,044800.4,A,2503.024726,N,12135.291057,E,29.8,259.9,040316,,,A*5B\r\n"};

													#define gprmcstr_COUNT 9
													
#endif




#include "gps_tracker.h"

rt_device_t gps_serial_fd = RT_NULL ;
static rt_tick_t cur_tick = 0;
static unsigned int gps_buffer_tmp_index = 0;
unsigned char gps_process_flag = 0;
static void process_gps_handle(void *p)
{
	int i=0;
	
	#ifdef GPS_DATA_TEST
	static unsigned char gpscnt = 0;
	if (gpscnt >= gprmcstr_COUNT)
		gpscnt = 0;
	snprintf((char*)gps_buffer_tmp,sizeof(gps_buffer_tmp),"%s",gprmcstr[gpscnt++]);
	
	if ((gpscnt % 3) == 0)
	{
		snprintf((char*)gps_buffer_tmp,sizeof(gps_buffer_tmp),"%s",xgps_valid_data);
	}
	
	#endif
	
	//把保存的最大GPS信号清除
	gps_info.real_max_gps_sig = 0;
	

	
	//开始解析数据
	gps_info.parse_gprmc_succ = 0;
	parser_gps_rawdata((char*)gps_buffer_tmp,strlen((char*)gps_buffer_tmp));
	
	
	if (gps_info.his_gprmc.status == 0x0)
	{
		//如果从未定位成功过,一直更新gps数据
			snprintf((char*)gps_raw_data,sizeof(gps_raw_data),"%s",gps_buffer_tmp);
		//
	} else {
		//如果定位成功过,只有在定位成功后才更新gps数据
		if ((gps_info.gprmc.status == 'A') && (gps_info.parse_gprmc_succ == 1))
		{
			snprintf((char*)gps_raw_data,sizeof(gps_raw_data),"%s",gps_buffer_tmp);
		}
		
	}
	
	#if 1
	DEBUGL->debug("\r\n\r\n\r\n\r\n");
	for(i=0;i<strlen((char *)gps_buffer_tmp);i++)
	{
		DEBUGL->debug("%c",gps_buffer_tmp[i]);
	}
	DEBUGL->debug("\r\n\r\n\r\n\r\n");
	#endif
	
	
	gps_buffer_tmp_index = 0;
	gps_process_flag = 2;
	//
}

//void process_gps_handle_dma(void *p)

void test_gpsdata_routing(void)
{
	static unsigned int __tmp_test_timer = 0;
	if (app_timer_data.app_timer_second > __tmp_test_timer + 5)
	{
		__tmp_test_timer = app_timer_data.app_timer_second;
		process_gps_handle(0);
		
	}
}

static rt_err_t usart_rx_ind(rt_device_t dev, rt_size_t size)
{
	
	
	char ch;

	if ((rt_tick_get() - cur_tick) > 1)
	{

		if (gps_process_flag == 0) //空闲
		{
			gps_buffer_tmp[gps_buffer_tmp_index] = 0x0;
			
			if (post_hw_int_event(process_gps_handle,0,0,0) == RT_EOK)
			{
				gps_process_flag = 1; //busy
			}else
			{
				gps_buffer_tmp_index = 0;
			}
			
		}else if (gps_process_flag == 2)
		{
			gps_buffer_tmp_index = 0;
			gps_process_flag = 0;
		}
	}
	
	//读取
	cur_tick = rt_tick_get();

	while (rt_device_read(gps_serial_fd, 0, &ch, 1) == 1)
	{
		if (gps_process_flag == 0)
		{
			if (gps_buffer_tmp_index < (BUFFER_LEN - 128))
			{
				#ifdef GPS_DATA_TEST
				gps_buffer_tmp_index++;
				#else
				gps_buffer_tmp[gps_buffer_tmp_index++] = ch;	
				#endif
			}
		}
	}
	
	return RT_EOK;
}




//DEBUGL->debug("##### %f , %f \r\n",gps_info.google_LAT,gps_info.google_LON);

unsigned char gps_enable_flag =0;
void enable_gps(void){
	int at_cmd_ret_code;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	if (gps_enable_flag == 1)
		return;
	gps_enable_flag = 1;
	

	memset(&GPIO_InitStructure,0x0,sizeof(GPIO_InitStructure));
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_0);
	
	cmux_at_command(2,"AT+UGPS=1,8,3\r\n",AT_AT,RT_TICK_PER_SECOND,&at_cmd_ret_code);
	cmux_at_command(2,"AT+ULOCGNSS=9,1,3,7,0,0,1\r\n",AT_AT,RT_TICK_PER_SECOND,&at_cmd_ret_code);
	
	//AT+ULOCGNSS=15,0,3,7,0,0 恢复默认
	
};
void disable_gps(void){

	GPIO_InitTypeDef GPIO_InitStructure;
	if (gps_enable_flag == 0)
		return;
	gps_enable_flag = 0;
	

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
};

rt_err_t open_gps_uart(char *name)
{

	
	gps_serial_fd = rt_device_find(name);
	
	if (gps_serial_fd != RT_NULL && rt_device_open(gps_serial_fd, RT_DEVICE_OFLAG_RDWR) == RT_EOK)
	{
		rt_device_set_rx_indicate(gps_serial_fd, usart_rx_ind);
		//
	}else
	{
		return -RT_ERROR;
	}
	
	return -RT_EOK;
	//
}


void calculateCheckSum(uint8_t* in, rt_size_t length, uint8_t* out)
{
	uint8_t i ;
		uint8_t a = 0;
		uint8_t b = 0;

		for (i = 0; i < length; i++) {

						a = a + in[i];
						b = b + a;

		}

		out[0] = (a & 0xFF);
		out[1] = (b & 0xFF);

}


void __send_restart_cmd(unsigned short nav_bbr_mask , unsigned char reset_mode)
{
	unsigned char* msg_ptr;
	struct CfgRst message;
	message.header.sync1 = UBX_SYNC_BYTE_1;
	message.header.sync2 = UBX_SYNC_BYTE_2;
	message.header.message_class = MSG_CLASS_CFG;
	message.header.message_id = MSG_ID_CFG_RST;
	message.header.payload_length = 4;

	message.nav_bbr_mask = nav_bbr_mask;    //X2-Bitfield?
	// Startup Modes
	// Hotstart 0x000
	// Warmstart 0x0001
	// Coldstart 0xFFFF
	message.reset_mode = reset_mode;
	// Reset Modes:
	// Hardware Reset 0x00
	// Controlled Software Reset 0x01
	// Controlled Software Reset - Only GPS 0x02
	// Hardware Reset After Shutdown 0x04
	// Controlled GPS Stop 0x08
	// Controlled GPS Start 0x09

	//message.reserved = 0;

	msg_ptr = (unsigned char*) &message;
	calculateCheckSum(msg_ptr + 2, 8, message.checksum);
	
	rt_device_write(gps_serial_fd, 0, msg_ptr, sizeof(message));
	
}

void __send_gps_cmd(void)
{
	unsigned int rgps_data_tick;
	unsigned char ch;
	//B5 62 06 08 06 00 88 13 01 00 01 00 B1 49
	unsigned char gms_cmd[] = {0xB5 , 0x62 , 0x06 , 0x08 , 0x06 ,0x00 , 0x88 , 0x13 , 0x01 , 0x00 , 0x01 , 0x00 ,0xB1 ,0x49};
	rt_device_write(gps_serial_fd,0,gms_cmd,sizeof(gms_cmd));
	rt_device_set_rx_indicate(gps_serial_fd, 0x0);
	
	enable_gps();
	
	rgps_data_tick = rt_tick_get();
	rt_kprintf("READ GPS CMD RESP :");
	for(;;)
	{
		if ((rt_tick_get() - rgps_data_tick) > 200)
			break;
		
		while (rt_device_read(gps_serial_fd, 0, &ch, 1) == 1)
		{
			rt_kprintf("%02x",ch);
		}
	}
	rt_kprintf("\r\n");
	
	
	//恢复中断
	rt_device_set_rx_indicate(gps_serial_fd, usart_rx_ind);
}


void init_ublox_hw(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}