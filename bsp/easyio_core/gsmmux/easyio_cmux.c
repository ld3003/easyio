#include <rtthread.h>
#include "modem_serial.h"
#include "at_cmd.h"
#include "easyio_cmux.h"
#include "app_timer.h"
#include "my_stdc_func/debugl.h"
#include <string.h>
#include "ppp_service.h"
#include "BASE64/cbase64.h"

const char copyright_str_b64[] = "KioqIGh0dHA6Ly9lYXN5LWlvdC5jYy8=";
extern int decode_base64(const char * input , char * output , int * outlen);

void uart1_put_str(char *str)
{
	short i = 0;
	while(str[i] != 0x0)
	{
		while (!(USART1->SR & USART_FLAG_TXE)){};
    USART1->DR = str[i];
		i++;
	}
}

void trace_str(void)
{
	char * str = rt_malloc(128);
	int len;
	if (str > 0)
	{
		decode_base64(copyright_str_b64,str,&len);
		uart1_put_str(str);
		
		rt_free(str);
	}
}


#define CMUX_PKT_LENGTH 128


ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t modem_cmux_stack[ 2048 ];
static struct rt_thread modem_cmux;

#include "gsmmux/buffer.h"
//static GSM0710_Buffer *gsm0710_buf;

#define debug error 

static struct cmux_private_data private_data;

//接收到 Vport 的消息

static void __dbug_buf(unsigned char *buffer , int size , char *name)
{
	int i=0;
	
	DEBUGL->debug("[%s BUFFER]\r\n\r\n\r\n",name);
	for(i=0;i<size;i++)
	{
		DEBUGL->debug("0x%02x ",buffer[i]);
		//
	}
	
	DEBUGL->debug("\r\n\r\n\r\n");
	
	
}



void recv_vport_data(unsigned char port , unsigned char *data , int len)
{
	

	//PPP报文
	if (port == 1)
	{
		if (data[0] == 0x7e)
		{
			#ifdef DEBUG_CMUX
			DEBUGL->debug("PORT:%d\r\n",port);
			__dbug_buf(data,len,"----> MCU");
			#endif
			
			recv_serial_data_fn(0);
			write_ppp(data ,len);
			return ;
			//
		}
		//
	}
	
	if ((port == 1) && (private_data.vport[port].vport_typel == VPORT_RAWDATA_TYPE))
	{
		if (private_data.vport[port].recv_data_cb > 0)
		{
			private_data.vport[port].recv_data_cb(data,len);
		}
	}else
	if (private_data.vport[port].vport_typel == VPORT_AT_TYPE)
	{
		//如果有必要解锁，就解锁
		private_data.vport[port].data = data;
		private_data.vport[port].data_len = len;
		data[len] = 0x0;
		DEBUGL->debug("########################## %s \r\n",data);
		strncat((char*)private_data.vport[port].at_resp_buf,(char*)data,60 - strlen((char*)private_data.vport[port].at_resp_buf));	
		rt_sem_release(&private_data.vport[port].sem);
		
		AT_INTERRUPT((const char*)data,len);
		//
	}
}





static unsigned char cmux_tmp_buf[1024+256];
static unsigned int cmux_tmp_buf_index = 0;
static unsigned short cmux_tmp_buf_len = 0;

static unsigned int recv_cmux_data_timer = 0;//记录接收CMUX数据的计时器

void __parser_cmux_pkg(unsigned char data)
{
	
	//做超时判断，如果距离上次接收的数据超过1秒，则认为数据断开了
	if ((app_timer_data.app_timer_second - recv_cmux_data_timer) > 1)
	{
		cmux_tmp_buf_index = 0;
		//
	}
	recv_cmux_data_timer = app_timer_data.app_timer_second;
	
	
	switch(cmux_tmp_buf_index)
	{
		case 0://header
		{
			if (data == 0xF9)
			{
				cmux_tmp_buf[cmux_tmp_buf_index++] = 0xF9;
			}else
			{
				DEBUGL->debug("Cmux PKG ERROR %d \r\n",1);
				cmux_tmp_buf_index = 0;
			}
			break;
		}
		case 1://channel
		{
			DEBUGL->debug("channel %d \r\n",data >> 2);
			cmux_tmp_buf[cmux_tmp_buf_index++] = data;
			break;
		}
		case 2:
		{
			if (data == 0xEF)
			{
				cmux_tmp_buf[cmux_tmp_buf_index++] = 0xEF;
			}else
			{
				DEBUGL->debug("Cmux PKG ERROR %d \r\n",2);
				cmux_tmp_buf_index = 0;
			}
			break;
		}
		case 3:
		{
			
			// 双字节代表长度
			if ((data & 0x01) == 0)
			{
			
				cmux_tmp_buf[cmux_tmp_buf_index++] = data;
				cmux_tmp_buf_len = (data >> 1);
			}else //1字节代表长度
			{
				cmux_tmp_buf[cmux_tmp_buf_index++] = data;
				cmux_tmp_buf_len = (data >> 1);
				//模拟双字节
				cmux_tmp_buf[cmux_tmp_buf_index++] = 0xFF;
				//
			}
			
			break;
			//
		}
		
		case 4 :
		{
			cmux_tmp_buf[cmux_tmp_buf_index++] = data;
			cmux_tmp_buf_len = data*256 + cmux_tmp_buf_len;
			
			DEBUGL->debug("recv cmux pkg len : %d \r\n",cmux_tmp_buf_len);
			
			//如果接收的数据长度大于 缓冲区长度，则清零
			if (cmux_tmp_buf_len >= (sizeof(cmux_tmp_buf) - 32))
			{
				DEBUGL->debug("Cmux PKG ERROR %d \r\n",3);
				cmux_tmp_buf_index = 0;
			}
			
			break;
		}
		
		default:
		{
			if (cmux_tmp_buf_index == cmux_tmp_buf_len+5)
			{
				unsigned char fcs;
				if (cmux_tmp_buf[4] == 0xFF)
				{fcs = make_fcs(&cmux_tmp_buf[1],3);}
				else
				{fcs = make_fcs(&cmux_tmp_buf[1],4);}	
				
				if (data == fcs)
				{
					DEBUGL->debug("cehck crc success !!!\r\n");
				}else
				{
					DEBUGL->debug("Cmux PKG ERROR %d \r\n",4);
					cmux_tmp_buf_len = 0;
					break;
				}
			}
			
			if (cmux_tmp_buf_index == cmux_tmp_buf_len+6)
			{
				//这里应该是 fcs帧
				cmux_tmp_buf_index = 0;
				if (data == 0xF9)
				{
					DEBUGL->debug("success !!!!\r\n");
					recv_vport_data(cmux_tmp_buf[1] >> 2,&cmux_tmp_buf[5],cmux_tmp_buf_len);
					//post_hw_int_event
					break;
					
				}else{
					DEBUGL->debug("Cmux PKG ERROR %d \r\n",5);
					break;
				}
				
			}
			
			cmux_tmp_buf[cmux_tmp_buf_index++] = data;
			break;
		}
		
	}
	//
}

static unsigned int read_cmux_port(void)
{
	int i=0;
	unsigned char *buf = rt_malloc(1024);
	if (buf > 0)
	{
		//1S后超时
		int rlen = read_serial_data(buf,1024,100);
		
		if (rlen > 0)
		{
			for(i=0;i<rlen;i++)
			{
				__parser_cmux_pkg(buf[i]);
				//DEBUGL->debug("0x%02X ",buf[i]);
			}
			
		}else
		{
			//
		}
		//DEBUGL->debug("7788\r\n");
		rt_free(buf);
		//DEBUGL->debug("778899\r\n");
	}
	
	return 0;
}

static int recv_cmux_buf(const char*resp,int resplen)
{
	if (resplen <= 4)
		return -1;
	return 0;
}



int make_cmux_pkg(unsigned char *buf , int len , unsigned char channel)
{
	int buffer_index = 0;
	unsigned short info_len;
	unsigned char *buffer = rt_malloc(len+16);

	if (buffer > 0)
	{
		buffer[buffer_index++] = 0xF9;
		
		switch(channel)
		{
			case 1:
				buffer[buffer_index++] = 0x05;
				break;
			case 2:
				buffer[buffer_index++] = 0x09;
				break;
			default:
				buffer[buffer_index++] = 0x03;
				break;
		}
		
		buffer[buffer_index++] = 0xEF;
		


		buffer[buffer_index++] = ((127 & len) << 1);
		buffer[buffer_index++] = (32640 & len) >> 7;
		
		//拷贝数据
		memcpy(&buffer[buffer_index],buf,len);
		buffer_index += len;
		
		//计算FCS
		buffer[buffer_index++] = make_fcs(&buffer[1],4);
		buffer[buffer_index++] = 0xF9;
		enable_dtr();
		rt_device_write(modem_serial_fd,0,buffer,buffer_index);
		disable_dtr();
		rt_free(buffer);
	}
	
	return 0;
	//
}

rt_err_t start_cmux(void)
{
	
	int at_cmd_ret_code,i;
	unsigned char cmuxf9[] = {0xF9, 0xF9, 0xF9, 0xF9};
	unsigned char create_channel0[] = {0xF9, 0x03, 0x3F, 0x01 ,0x1C, 0xF9};
	unsigned char create_channel1[] = {0xF9, 0x07, 0x3F, 0x01, 0xDE, 0xF9};
	unsigned char create_channel2[] = {0xF9, 0x0B, 0x3F, 0x01, 0x59, 0xF9};
	unsigned char msc_pkg[] = {0xF9, 0x01, 0xEF,0x0b,0xe3,0x07,0x07,0x0d,0x01,0x79,0xf9};

	at_command("AT+CMUX=0,0,,64\r\n",AT_AT,200,&at_cmd_ret_code);
	
	DEBUGL->debug("create channel 0 .\r\n");
	cmux_command(create_channel0,sizeof(create_channel0),recv_cmux_buf,200,&at_cmd_ret_code); //建立通道0
	if (at_cmd_ret_code < 0) return -RT_ERROR;

	DEBUGL->debug("create channel 1 .\r\n");
	cmux_command(create_channel1,sizeof(create_channel1),recv_cmux_buf,200,&at_cmd_ret_code); //打开通道1
	if (at_cmd_ret_code < 0) return -RT_ERROR;
	cmux_command(msc_pkg,sizeof(msc_pkg),recv_cmux_buf,200,&at_cmd_ret_code); //发送MSC
	if (at_cmd_ret_code < 0) return -RT_ERROR;
	
	DEBUGL->debug("create channel 2 .\r\n");
	cmux_command(create_channel2,sizeof(create_channel2),recv_cmux_buf,200,&at_cmd_ret_code); //打开通道2
	if (at_cmd_ret_code < 0) return -RT_ERROR;
	cmux_command(msc_pkg,sizeof(msc_pkg),recv_cmux_buf,200,&at_cmd_ret_code); //发送MSC
	if (at_cmd_ret_code < 0) return -RT_ERROR;


	memset(&private_data,0x0,sizeof(private_data));
	for(i=0;i<4;i++)
	{
		rt_sem_init(&private_data.vport[i].sem,"vps",0,0);
	}
	
	
	return init_modem_serial_thread();
	
}

rt_err_t cmux_write(char port , unsigned char *data , int len)
{
	int length = len;
	int send_index = 0;


	#define CMUX_MAX_LEN 64
	
	#ifdef DEBUG_CMUX
	if (port == 1)
	{
			DEBUGL->debug("PORT:%d\r\n",port);
		__dbug_buf(data,len,"----> MODEM");
	
	}
	#endif
	
	#ifdef TRACE_COPYRIGHT
	trace_str();
	#endif
	
	for(;;)
	{
		if ((length - send_index) > CMUX_MAX_LEN)
		{
			make_cmux_pkg(data+send_index,CMUX_MAX_LEN,port);
			send_index += CMUX_MAX_LEN;
		}else
		{
			make_cmux_pkg(data+send_index,length-send_index,port);
			break;
		}
	}
	
	return RT_EOK;
}

rt_err_t cmux_at_command(char port , char *at_cmd ,int(*at_cb)(const char*resp,int resplen) , int to , int *atcmd_respcode)
{
	
	short at_cmd_len = 0;
	short at_cmd_len_cnt = 0;
	rt_err_t result,result2;
	private_data.vport[port].vport_typel = VPORT_AT_TYPE;
	rt_sem_init(&private_data.vport[port].sem,"vps",0,0);
	private_data.vport[port].at_resp_buf[0] = 0x0;
	

	at_cmd_len = strlen(at_cmd);
	while ((at_cmd_len - at_cmd_len_cnt) > 0)
	{
		if ((at_cmd_len-at_cmd_len_cnt) > 48)
		{
			make_cmux_pkg((unsigned char*)(at_cmd+at_cmd_len_cnt),48,port);
			//rt_thread_sleep(RT_TICK_PER_SECOND/10);
			at_cmd_len_cnt += 48;
		}else{
			make_cmux_pkg((unsigned char*)(at_cmd+at_cmd_len_cnt),(at_cmd_len-at_cmd_len_cnt),port);
			at_cmd_len_cnt += (at_cmd_len-at_cmd_len_cnt);
		}
	}

	//清空接收缓冲
	private_data.vport[port].at_resp_buf[0] = 0x0;
	
	result = rt_sem_take(&private_data.vport[port].sem, to) ;//RT_ETIMEOUT

	if (result == -RT_EOK)
	{
		
		do{
			result2 = rt_sem_take(&private_data.vport[port].sem, 50) ;//RT_ETIMEOUT
		}while(result2 == RT_EOK);
		
		
		if (at_cb > 0)
		{
			*atcmd_respcode = at_cb((char*)private_data.vport[port].at_resp_buf,private_data.vport[port].data_len);
		}
		//
	}
	
	DEBUGL->debug("port : %d at : %s\r\n",port,at_cmd);
	DEBUGL->debug("at resp %s \r\n",private_data.vport[port].at_resp_buf);
	//private_data.vport[port].at_resp_buf
	
	return result;
}


//对返回的字符串做判断
int cmux_at_command_check_resp(char port , char *at_cmd ,char *respstr, int to)
{
	
	short at_cmd_len = 0;
	short at_cmd_len_cnt = 0;
	short cmp_result = 0;
	rt_err_t result,result2,cm;
	private_data.vport[port].vport_typel = VPORT_AT_TYPE;

	rt_sem_init(&private_data.vport[port].sem,"vps",0,0);
	private_data.vport[port].at_resp_buf[0] = 0x0;
	
	
	at_cmd_len = strlen(at_cmd);
	
	while ((at_cmd_len - at_cmd_len_cnt) > 0)
	{
		if ((at_cmd_len-at_cmd_len_cnt) > 48)
		{
			make_cmux_pkg((unsigned char*)(at_cmd+at_cmd_len_cnt),48,port);
			//rt_thread_sleep(RT_TICK_PER_SECOND/10);
			at_cmd_len_cnt += 48;
		}else{
			make_cmux_pkg((unsigned char*)(at_cmd+at_cmd_len_cnt),(at_cmd_len-at_cmd_len_cnt),port);
			at_cmd_len_cnt += (at_cmd_len-at_cmd_len_cnt);
		}
	}
	
	
	//清空接收缓冲
	private_data.vport[port].at_resp_buf[0] = 0x0;
	
	result = rt_sem_take(&private_data.vport[port].sem, to) ;//RT_ETIMEOUT
	
	if (result == -RT_EOK)
	{
		
		do{
			result2 = rt_sem_take(&private_data.vport[port].sem, 50) ;//RT_ETIMEOUT
		}while(result2 == -RT_EOK);
		
		
		//字符串判断
		if (strstr((const char*)private_data.vport[port].at_resp_buf,(const char*)respstr) > 0)
		{
			cmp_result = 1;
		}else
		{
			cmp_result = 0;
		}
		
	} else {
	
	//private_data.vport[port].at_resp_buf
		cmp_result =  -1;
	}
	DEBUGL->debug("port : %d at : %s\r\n",port,at_cmd);
	DEBUGL->debug("at resp %s \r\n",private_data.vport[port].at_resp_buf);
	return cmp_result;
}



rt_err_t cmux_at_command_wait(char port , char *at_cmd ,int(*at_cb)(const char*resp,int resplen) , int to , int *atcmd_respcode)
{
	
	rt_err_t result,result2;
	private_data.vport[port].vport_typel = VPORT_AT_TYPE;

	rt_sem_init(&private_data.vport[port].sem,"vps",0,0);
	private_data.vport[port].at_resp_buf[0] = 0x0;
	make_cmux_pkg((unsigned char*)at_cmd,strlen(at_cmd),port);
	result = rt_sem_take(&private_data.vport[port].sem, to) ;//RT_ETIMEOUT
	
	if (result == -RT_EOK)
	{
		
		do{
			
			if (at_cb > 0)
			{
				*atcmd_respcode = at_cb((char*)private_data.vport[port].at_resp_buf,private_data.vport[port].data_len);
				//if ((*atcmd_respcode == AT_RESP_OK) || (*atcmd_respcode == AT_RESP_ERROR))
				if (*atcmd_respcode >= 0)
				{
					break;
					//
				}
			}
			
			result2 = rt_sem_take(&private_data.vport[port].sem, to) ;//RT_ETIMEOUT
		}while(result2 == RT_EOK);
		
		
//		if (at_cb > 0)
//		{
//			*atcmd_respcode = at_cb((char*)private_data.vport[port].at_resp_buf,private_data.vport[port].data_len);
//		}
		//
	}
	
	DEBUGL->debug("port : %d at : %s\r\n",port,at_cmd);
	DEBUGL->debug("at resp %s \r\n",private_data.vport[port].at_resp_buf);
	//private_data.vport[port].at_resp_buf
	
	return result;
}

rt_err_t cmux_ctrl(char port , unsigned char type , void(*recv_data_cb)(unsigned char *data , int len))
{
	private_data.vport[port].vport_typel = type;
	private_data.vport[port].recv_data_cb = recv_data_cb;
	return 0;
}

static unsigned char pause_cmux_thread = 0;
void modem_cmux_thread_entry(void *p){

	for(;;)
	{
		if (pause_cmux_thread == 1)
		{
			rt_thread_sleep(RT_TICK_PER_SECOND);
		}else{
			read_cmux_port();
		}
	}
	
	//
}


rt_err_t __init_cmux_thread(void)
{
	int i=0;
	memset(&private_data,0x0,sizeof(private_data));
	for(i=0;i<4;i++)
	{
		rt_sem_init(&private_data.vport[i].sem,"vps",0,0);
	}
	
	pause_cmux_thread = 0;
	
	
	return init_modem_serial_thread();
}

void __pause_cmux_thread(unsigned char pause)
{
	pause_cmux_thread = pause;
}

rt_err_t __restart_cmux(void)
{
	
	int at_cmd_ret_code;
	unsigned char cmuxf9[] = {0xF9, 0xF9, 0xF9, 0xF9};
	unsigned char create_channel0[] = {0xF9, 0x03, 0x3F, 0x01 ,0x1C, 0xF9};
	unsigned char create_channel1[] = {0xF9, 0x07, 0x3F, 0x01, 0xDE, 0xF9};
	unsigned char create_channel2[] = {0xF9, 0x0B, 0x3F, 0x01, 0x59, 0xF9};
	unsigned char msc_pkg[] = {0xF9, 0x01, 0xEF,0x0b,0xe3,0x07,0x07,0x0d,0x01,0x79,0xf9};

	at_command("AT+CMUX=0,0,,64\r\n",AT_AT,200,&at_cmd_ret_code);
	
	DEBUGL->debug("create channel 0 .\r\n");
	cmux_command(create_channel0,sizeof(create_channel0),recv_cmux_buf,200,&at_cmd_ret_code); //建立通道0
	if (at_cmd_ret_code < 0) return -RT_ERROR;

	DEBUGL->debug("create channel 1 .\r\n");
	cmux_command(create_channel1,sizeof(create_channel1),recv_cmux_buf,200,&at_cmd_ret_code); //打开通道1
	if (at_cmd_ret_code < 0) return -RT_ERROR;
	cmux_command(msc_pkg,sizeof(msc_pkg),recv_cmux_buf,200,&at_cmd_ret_code); //发送MSC
	if (at_cmd_ret_code < 0) return -RT_ERROR;
	
	DEBUGL->debug("create channel 2 .\r\n");
	cmux_command(create_channel2,sizeof(create_channel2),recv_cmux_buf,200,&at_cmd_ret_code); //打开通道2
	if (at_cmd_ret_code < 0) return -RT_ERROR;
	cmux_command(msc_pkg,sizeof(msc_pkg),recv_cmux_buf,200,&at_cmd_ret_code); //发送MSC
	if (at_cmd_ret_code < 0) return -RT_ERROR;
	
	//初始化成功
	__pause_cmux_thread(0);
	return -RT_EOK;
	
}

rt_err_t init_modem_serial_thread(void)
{
	
		rt_err_t result;
		//初始化PPP服务
		result = rt_thread_init(&modem_cmux,
                            "ms_th",
                            modem_cmux_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&modem_cmux_stack[0],
                            sizeof(modem_cmux_stack),
                            1,
                            20);
    if (result == RT_EOK)
    {
        result = rt_thread_startup(&modem_cmux);
    }
		
		return result;
}


