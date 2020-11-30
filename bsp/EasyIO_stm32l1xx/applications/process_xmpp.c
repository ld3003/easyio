
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <rtthread.h>

#include "process_xmpp.h"
#include "gnss.h"
#include "global_env.h"
#include "nmea/generate.h"
#include "ublox_gps.h"
#include "lowpower.h"

#include "my_stdc_func/debugl.h"
#include "my_stdc_func/my_stdc_func.h"

#include "xmpp_tcp.h"
#include "spiflash_config.h"

#include "ota_http_tcp.h"
#include "gps_tracker.h"

#include "adc.h"

#define RESP_BUFFER_LEN 512

struct XMPP_MSG_PM {
	char msg[64];
	void (*cb)(char*,char*,char*);
	void *p;
};
static unsigned char tmp_resp_buffer[RESP_BUFFER_LEN];

static void resp_hello(char *from , char *in , char *out)
{
	snprintf(out,RESP_BUFFER_LEN,"hi");
}

static void resp_getgprmc(char *from , char *in , char *out)
{
	char type[10];

	sscanf(in,"getgprmc=%s",&type);
		
	#if 1
	open_gps_2min();
	get_gps_line("$GPRMC",(char*)tmp_resp_buffer,(unsigned int)RESP_BUFFER_LEN,(char*)gps_raw_data);
	if (strlen((char*)tmp_resp_buffer) <= 0)
	{
		get_gps_line("$GNRMC",(char*)tmp_resp_buffer,(unsigned int)RESP_BUFFER_LEN,(char*)gps_raw_data);
		if (strlen((char*)tmp_resp_buffer) > 0)
		{
			for(;;)
			{
				char *tmp = strstr((char *)tmp_resp_buffer,"$GNRMC");
				if (tmp <= 0)
					break;
				tmp[2] = 'P';
			}
			//tmp_resp_buffer[2] = 'P';
			//
		}
	}
	
	#endif
	if(strstr(type,"raw") ){
			snprintf(out,RESP_BUFFER_LEN,"%s",tmp_resp_buffer);
	}else{
		 snprintf(out,RESP_BUFFER_LEN,"getgprmc=%s",tmp_resp_buffer);
	}

}

static void resp_getgprmc_nowake(char *from , char *in , char *out)
{
	char type[10];
	sscanf(in,"getgprmc_nowake=%s",&type);
	#if 1
	get_gps_line("$GPRMC",(char*)tmp_resp_buffer,(unsigned int)RESP_BUFFER_LEN,(char*)gps_raw_data);
	if (strlen((char*)tmp_resp_buffer) <= 0)
	{
		get_gps_line("$GNRMC",(char*)tmp_resp_buffer,(unsigned int)RESP_BUFFER_LEN,(char*)gps_raw_data);
		if (strlen((char*)tmp_resp_buffer) > 0)
		{
			for(;;)
			{
				char *tmp = strstr((char *)tmp_resp_buffer,"$GNRMC");
				if (tmp <= 0)
					break;
				tmp[2] = 'P';
			}
			//tmp_resp_buffer[2] = 'P';
			//
		}
	}
	
	#endif
	if(strstr(type,"raw") ){
			snprintf(out,RESP_BUFFER_LEN,"%s",tmp_resp_buffer);
	}else{
		 snprintf(out,RESP_BUFFER_LEN,"getgprmc=%s",tmp_resp_buffer);
	}

}

static void resp_getgsv(char *from , char *in , char *out)
{
	
	open_gps_2min();
	//nmea_gen_GPGSV((char*)tmp_resp_buffer,RESP_BUFFER_LEN,&gps_info.gpgsv[0]);
	get_gps_line("$GPGSV",(char*)tmp_resp_buffer,(unsigned int)RESP_BUFFER_LEN,(char*)gps_buffer_tmp);
	snprintf(out,RESP_BUFFER_LEN,"getgsv=%s",tmp_resp_buffer);
}

static void resp_getgsa(char *from , char *in , char *out)
{
	
	
	//nmea_gen_GPGSA((char*)tmp_resp_buffer,RESP_BUFFER_LEN,&gps_info.gpgsa);
	open_gps_2min();
	#if 1
	get_gps_line("$GPGSA",(char*)tmp_resp_buffer,(unsigned int)RESP_BUFFER_LEN,(char*)gps_buffer_tmp);
	if (strlen((char*)tmp_resp_buffer) <= 0)
	{
		get_gps_line("$GNGSA",(char*)tmp_resp_buffer,(unsigned int)RESP_BUFFER_LEN,(char*)gps_buffer_tmp);
		if (strlen((char*)tmp_resp_buffer) > 0)
		{
			for(;;)
			{
				char *tmp = strstr((char *)tmp_resp_buffer,"$GNGSA");
				if (tmp <= 0)
					break;
				tmp[2] = 'P';
			}
			//tmp_resp_buffer[2] = 'P';
			//
		}
	}
	#endif
	snprintf(out,RESP_BUFFER_LEN,"getgsa=%s",tmp_resp_buffer);
	//
}


static void resp_setbroadcastgps(char *from , char *in , char*out)
{
	int p_len = get_parma_count(in);
	char *tmp;
	char write_config_flag = 0;
	if (p_len >= 1)
	{
		tmp = (char *)get_parma(1,in);
		
		sscanf(tmp,"%d",&user_app_conf.PRESENCE_UPLOAD_PREIOD);
		
		//合法值
		if (user_app_conf.PRESENCE_UPLOAD_PREIOD < 5)
			user_app_conf.PRESENCE_UPLOAD_PREIOD = 5;
		if (user_app_conf.PRESENCE_UPLOAD_PREIOD > 86400)
			user_app_conf.PRESENCE_UPLOAD_PREIOD = 86400;
		
		user_app_conf.PRESENCE_BC_COUNT = 0;
		write_config_flag = 1;
		
		DEBUGL->debug("set broadcastgps_time = %d\r\n",user_app_conf.PRESENCE_UPLOAD_PREIOD);
		
		snprintf(out,RESP_BUFFER_LEN,"setbroadcastgps=OK");
	}
	
	if (p_len >= 2)
	{
		tmp = (char *)get_parma(2,in);
		sscanf(tmp,"%d",&user_app_conf.PRESENCE_BC_COUNT);
		
		if (user_app_conf.PRESENCE_BC_COUNT > 50)
		{
			user_app_conf.PRESENCE_BC_COUNT = 50;
		}
		
		write_config_flag = 1;
		//
	}
	
	if (write_config_flag == 1)
	{
		userapp_writeconfig();
	}
	//
}

static void resp_getbroadcastgps(char *from , char *in , char*out)
{
	snprintf(out,RESP_BUFFER_LEN,"getbroadcastgps=%d,%d",user_app_conf.PRESENCE_UPLOAD_PREIOD,user_app_conf.PRESENCE_BC_COUNT);
}

static void resp_setgpspower(char *from , char *in , char*out)
{
	int p_len = get_parma_count(in);
	
	if (p_len == 1)
	{
		snprintf(out,RESP_BUFFER_LEN,"setgpspower=OK");
	}
	//
}

static void resp_getgpsstatus(char *from , char *in , char*out)
{
	snprintf(out,RESP_BUFFER_LEN,"getgpsstatus=%d,%d,%d",0,1,2);
	//
}

static void resp_powersaving(char *from , char *in , char*out)
{
	
	int p_len = get_parma_count(in);
	
	if (p_len == 1)
	{
		snprintf(out,RESP_BUFFER_LEN,"powersaving=OK");
	}
	//
}

static void resp_webtraceon(char *from , char *in , char*out)
{
	
	//webtraceon=START
	//webtraceon=START
	//
	
	int x,y;

	
	sscanf(in,"webtraceon=%d,%d",&x,&y);
	snprintf(out,RESP_BUFFER_LEN,"webtraceon=START");
	
	if (webtrace_tim_preiod > 0)
	{
		if(strstr(webtrace_jid,from) == 0)
		{
			//先向之前的用户发送END信息
			xmpp_send_msg(webtrace_jid,"webtraceon=END");
			//
		}
	}
	
	//更新新的用户
	snprintf(webtrace_jid,sizeof(webtrace_jid),"%s",from);
	webtrace_cnt = x;
	webtrace_tim_preiod = y;
	
}

//resp_webtraceoff

static void resp_webtraceoff(char *from , char *in , char*out)
{
	
	//webtraceon=START
	//webtraceon=START
	//
	
	int x,y;

	
	//发送END指令，并关闭发送webtrace的计数器
	snprintf(out,RESP_BUFFER_LEN,"webtraceon=END");
	
	snprintf(webtrace_jid,sizeof(webtrace_jid),"%s",from);
	webtrace_cnt = -1;
	
}

static void resp_getpowersaving(char *from , char *in , char*out)
{
	snprintf(out,RESP_BUFFER_LEN,"getpowersaving=%d",0);
	//
}



static void resp_reboot(char *from , char *in , char*out)
{
	char *tmp;
	int p_len = get_parma_count(in);
	if (p_len >= 1)
	{
		tmp = (char *)get_parma(0,in);
		sscanf(tmp,"%d",&global_reboot_time);
	}
	//global_reboot_time
	snprintf(out,RESP_BUFFER_LEN,"OK");
	//
}

static void resp_where(char *from , char *in , char*out)
{
	open_gps_2min();
	snprintf(out,RESP_BUFFER_LEN,"where=https://www.google.com/maps/preview?q=%f,%f",gps_info.google_LAT,gps_info.google_LON);
	//
}

#include "app_ev.h"
#include "xmpp_tcp.h"
#include "lwip_raw_api_tcp_template.h"
static void disconn_xmpp_tcp(void *p)
{
	MY_TCP_Client_pause(&tcp_client_buffer[XMPP_TCP_CLIENT]);
	init_xmpp_tcp();
	//
}
static void resp_setserver(char *from , char *in , char*out)
{
	
	int p_len = get_parma_count(in);
	char *tmp;
	if (p_len == 2)
	{
		tmp = (char *)get_parma(1,in);
		sscanf(tmp,"%s",user_app_conf.HOST_ADDRESS);
		tmp = (char *)get_parma(2,in);
		sscanf(tmp,"%d",&user_app_conf.HOST_PORT);
		user_app_conf.ACCOUNT[0] = 0x0;
	}
	else
	if (p_len == 3)
	{
		tmp = (char *)get_parma(1,in);
		sscanf(tmp,"%s",user_app_conf.HOST_ADDRESS);
		tmp = (char *)get_parma(2,in);
		sscanf(tmp,"%d",&user_app_conf.HOST_PORT);
		tmp = (char *)get_parma(3,in);
		sscanf(tmp,"%s",user_app_conf.ACCOUNT);
		
	}
	userapp_writeconfig();
	//
	snprintf(out,RESP_BUFFER_LEN,"setserver=%s,%d",user_app_conf.HOST_ADDRESS,user_app_conf.HOST_PORT);
	xmpp_send_msg(from,out);
	logout_xmpp();
	
	//两秒后关闭tcp
	post_default_event_wait(disconn_xmpp_tcp,0,0,0,RT_TICK_PER_SECOND+(RT_TICK_PER_SECOND/2));
	
	//
}

static void resp_getotastatus(char *from , char *in , char*out)
{
	snprintf(out,RESP_BUFFER_LEN,"getotastatus=%d",tracker_private_data.ota_status);
}

static void resp_checkota(char *from , char *in , char*out)
{
	config_fq_2();
	test_ota_flag = OTA_FLAG_VERSIONNUM_MODE_CFG;
	ota_start_http_download(from,in);
	snprintf(out,RESP_BUFFER_LEN,"checkota=ok");
	//
}

static void resp_startota(char *from , char *in , char *out)
{
	
	//if (tracker_private_data.ota_status == 8)
	config_fq_2();
	test_ota_flag = OTA_FLAG_VERSIONNUM_CFG;
	ota_start_http_download(from,in);
	snprintf(out,RESP_BUFFER_LEN,"startota=ok");
	//
}

static void verify_code(char *str , char *out)
{
	char i=0;
	char strl = strlen(str);
	for(i=0;i<strl;i++)
	{
		switch(str[i])
		{
			case '0':
			{
				out[i] = '3';
				break;
			}
			case '1':
			{
				out[i] = '0';
				break;
			}
			case '2':
			{
				out[i] = '1';
				break;
			}
			case '3':
			{
				out[i] = '7';
				break;
			}
			case '4':
			{
				out[i] = '8';
				break;
			}
			case '5':
			{
				out[i] = '2';
				break;
			}
			case '6':
			{
				out[i] = '5';
				break;
			}
			case '7':
			{
				out[i] = '9';
				break;
			}
			case '8':
			{
				out[i] = '6';
				break;
			}
			case '9':
			{
				out[i] = '4';
				break;
			}
			
			default:
			{
				out[i] = '0';
				break;
			}
				
		}
		
	}
	
}

static void register_code(char *str , char *out)
{
	char i=0;
	char strl = strlen(str);
	for(i=0;i<strl;i++)
	{
		switch(str[i])
		{
			case '0':
			{
				out[i] = '7';
				break;
			}
			case '1':
			{
				out[i] = '4';
				break;
			}
			case '2':
			{
				out[i] = '9';
				break;
			}
			case '3':
			{
				out[i] = '5';
				break;
			}
			case '4':
			{
				out[i] = '1';
				break;
			}
			case '5':
			{
				out[i] = '6';
				break;
			}
			case '6':
			{
				out[i] = '2';
				break;
			}
			case '7':
			{
				out[i] = '8';
				break;
			}
			case '8':
			{
				out[i] = '3';
				break;
			}
			case '9':
			{
				out[i] = '0';
				break;
			}
			default:
			{
				out[i] = '0';
				break;
			}
				
		}
	}
	
}


static void resp_verify(char *from , char *in , char *out)
{
	
	char *tmp1=0;
	char *tmp2=0;
	char p_len=0;
	char strl;
	strl = strlen(from);
	
	DEBUGL->debug("######from:	[%s] \r\n",from);
	DEBUGL->debug("######in:		[%s] \r\n",in);
	
	p_len = get_parma_count(in);
	if (p_len >= 1)
	{
		tmp2 = (char *)get_parma(0,in);
		//sscanf(tmp,"%d",&global_reboot_time);
	}else{
		goto RETERROR;
	}
	
	//判断电话号码是不是等于6
	if (strl > 6)
	{
		tmp1 = rt_malloc(8);
		if (tmp1 <= 0)
			goto RETERROR;
		
		memset(tmp1,0x0,8);
		//计算本机verify_code
		verify_code(&from[strl-6],tmp1);
		DEBUGL->debug("register_code %s \r\n",tmp1);
		DEBUGL->debug("register_code %s \r\n",tmp2);
		//判断verify_code是否合法
		if (strstr(in,tmp1) > 0)
		{
			//如果合法，将返回一个注册码
			register_code(&from[strl-6],out);
		}else {
			snprintf(out,RESP_BUFFER_LEN,"ERROR:verify");
		}
		
		rt_free(tmp1);	
		
	}else{
		goto RETERROR;
	}
	
	return ;
	
	RETERROR:
	snprintf(out,RESP_BUFFER_LEN,"ERROR");
	
}

static void resp_getadc(char *from , char *in , char *out)
{
	
	//unsigned short vol = get_bat_vol_fast();
	snprintf(out,RESP_BUFFER_LEN,"voltage=%d,%d",get_bat_vol_fast(),get_bat_vol());
	//
}


static void resp_update(char *from , char *in , char*out)
{
}

#include "spiflash_config.h"
static void resp_getctversion(char *from , char *in , char*out)
{
	extern const char versionstr[];
	snprintf(out,RESP_BUFFER_LEN,"%s",versionstr);;
}

#include "ota_http_tcp.h"
static void resp_program(char *from , char *in , char*out)
{
	
	//#ifdef ENABLE_OTA
	//snprintf(out,RESP_BUFFER_LEN,"Start update fw !");
	#if 0
	if (isp_start_http_download(from,in) == 0)
	{
		snprintf(out,RESP_BUFFER_LEN,"Restart program fw !");
		ota_restart_http_download();
	}else
	{
		snprintf(out,RESP_BUFFER_LEN,"Start program fw !");
	}
	#endif

	
	
	//
}

#include "stm32l1xx.h"
static void test(char *from , char *in , char*out)
{
	if (strstr(in,"crash"))
	{
		volatile char dummy = 0;                                                  \
    DEBUGL->debug("assert failed at %s:%d \n" , __FUNCTION__, __LINE__);\
    while (dummy == 0);
	}
	
	if (strstr(in,"close"))
	{
		__RESET_PPP();
	}
	
	if (strstr(in,"frq1"))
	{
		
		config_fq_1();
		
	}
	
	if (strstr(in,"frq2"))
	{
		config_fq_2();
	}
	
	if (strstr(in,"start_motion"))
	{
		tracker_private_data.dbg_motion = 1;
	}
	
	if (strstr(in,"end_motion"))
	{
		tracker_private_data.dbg_motion = 0;
	}
	
	if (strstr(in,"diable_dis"))
	{
		tracker_private_data.dbg_gps_distance = 1;
	}
	
	if (strstr(in,"enable_dis"))
	{
		tracker_private_data.dbg_gps_distance = 0;
	}
	
	if (strstr(in,"adc"))
	{
			config_fq_2();
			initADC();
			rt_thread_sleep(10);
			DEBUGL->debug("###### BATVAL:                   [%d]\r\n",get_bat_vol());
			disableADC();
				//return 0;
			config_fq_1();
	}
	//
}

static void isp(char *from , char *in , char*out)
{
	
}

#include "at_cmd.h"
#include "gsmmux/easyio_cmux.h"
static char *resp;
static int XMPPAT(const char *in , int len)
{
	snprintf(resp,64,"%s",in);
	if (strstr(resp,"\r\n"))
		return AT_RESP_OK;
	else
		return AT_RESP_ERROR;
	//
	
	
}
static void atcmd(char *from , char *in , char*out)
{
	
	int at_cmd_ret_code;
	strcat(in,"\r\n");
	resp = rt_malloc(64);
	if (resp <= 0)
	{
		snprintf(out,RESP_BUFFER_LEN,"malloc err");
		return;
	}
	
	cmux_at_command(2,in,XMPPAT,RT_TICK_PER_SECOND*3,&at_cmd_ret_code);
	snprintf(out,RESP_BUFFER_LEN,"%s",resp);
	
	//释放
	rt_free(resp);
	
}

static void setgps(char *from , char *in , char*out)
{
	GPS_COLDSTART;
	snprintf(out,RESP_BUFFER_LEN,"%s","OK");
}

static void testgps(char *from , char *in , char*out)
{
	TEST_GPS;
	snprintf(out,RESP_BUFFER_LEN,"%s","OK");
}

static void resp_setmovecall(char *from , char *in , char*out)
{
	int pb_index;
	int p_len = get_parma_count(in);
	char *tmp;
	//movcall_index
	
	if (p_len == 1)
	{
		tmp = (char *)get_parma(1,in);
		sscanf(tmp,"%d",&pb_index);
		
		if ((pb_index > 0) && (pb_index <= 4))
		{
			user_app_conf.movcall_index = pb_index;
			userapp_writeconfig();
			//goto RETSUCCESS;
		}
		//user_app_conf.ACCOUNT[0] = 0x0;
	}else{
		user_app_conf.movcall_index = 0;
		userapp_writeconfig();
		
	}
	snprintf(out,RESP_BUFFER_LEN,"setmovecall=ok");
	RETSUCCESS:
	return ;
	
	//
}
static void resp_getmovecall(char *from , char *in , char*out)
{
	if ((user_app_conf.movcall_index>0) && (user_app_conf.movcall_index <= 4))
	{
		snprintf(out,RESP_BUFFER_LEN,"getmovecall=%s",user_app_conf.phoneBook[user_app_conf.movcall_index-1]);
	}else{
		snprintf(out,RESP_BUFFER_LEN,"getmovecall=NULL");
	}
	//
}
static void resp_setphonebook(char *from , char *in , char*out)
{
	int pb_index;
	int p_len = get_parma_count(in);
	char *tmp;
	if (p_len == 2)
	{
		tmp = (char *)get_parma(1,in);
		sscanf(tmp,"%d",&pb_index);
		
		if ((pb_index > 0) && (pb_index <= 4))
		{
			
			tmp = (char *)get_parma(2,in);
			if (strlen(tmp) <= 13)
			{
				snprintf(user_app_conf.phoneBook[pb_index-1],16,"%s",tmp);
				snprintf(out,RESP_BUFFER_LEN,"Set PhoneBook:%d OK",pb_index);
				userapp_writeconfig();
				goto RETSUCCESS;
			}
			//
		}
		
		
		//user_app_conf.ACCOUNT[0] = 0x0;
	}
	else if (p_len == 1)
	{
		tmp = (char *)get_parma(1,in);
		sscanf(tmp,"%d",&pb_index);
		
		if ((pb_index > 0) && (pb_index <= 4))
		{
			snprintf(user_app_conf.phoneBook[pb_index-1],16,"");
			snprintf(out,RESP_BUFFER_LEN,"Delete PhoneBook:%d OK",pb_index);
			userapp_writeconfig();
			goto RETSUCCESS;
		}
		
		
		//user_app_conf.ACCOUNT[0] = 0x0;
	}
	
	snprintf(out,RESP_BUFFER_LEN,"setphonebook=ARG_ERROR");

	RETSUCCESS:
	return;
	//
}

static void resp_queryphonebook(char *from , char *in , char*out)
{
	int pb_index;
	char *tmp;
	int p_len = get_parma_count(in);
	if (p_len == 1)
	{
		tmp = (char *)get_parma(1,in);
		sscanf(tmp,"%d",&pb_index);
		
		if ((pb_index > 0) && (pb_index <= 4))
		{
			snprintf(out,RESP_BUFFER_LEN,"queryphonebook=%d,%s",pb_index,user_app_conf.phoneBook[pb_index-1]);
			goto RETSUCCESS;
		}
		
		
		//user_app_conf.ACCOUNT[0] = 0x0;
	}
	snprintf(out,RESP_BUFFER_LEN,"queryphonebook=ARG_ERROR");
	RETSUCCESS:
	
	return;
	
	//
}

#include "movecall.h"
static void resp_testphonecall(char *from , char *in , char*out)
{
	if (callrouting_status == CALLROUTING_STATUS_INIT)
	{
		callrouting_status = CALLROUTING_STATUS_CALLING;
	}
	snprintf(out,RESP_BUFFER_LEN,"resp_testphonecall=%d",callrouting_status);
	//
}

struct XMPP_MSG_PM xmpp_msg_pm[] = {
	{"hello",resp_hello,0},
	{"getgprmc",resp_getgprmc,0},
	{"getgprmc_nowake",resp_getgprmc_nowake,0},
	{"getgsv",resp_getgsv,0},
	{"getgsa",resp_getgsa,0},
	{"setbroadcastgps=",resp_setbroadcastgps,0},
	{"getbroadcastgps",resp_getbroadcastgps,0},
	{"setgpspower=",resp_setgpspower,0},
	{"getgpsstatus",resp_getgpsstatus,0},
	{"powersaving=",resp_powersaving,0},
	{"webtraceon=",resp_webtraceon,0},
	{"webtraceoff=",resp_webtraceoff,0},
	{"getpowersaving",resp_getpowersaving,0},
	{"reboot",resp_reboot,0},
	{"where",resp_where,0},
	//setserver
	{"setserver",resp_setserver,0},
	{"getotastatus",resp_getotastatus,0},
	{"checkota",resp_checkota,0},
	{"startota",resp_startota,0},
	{"verify",resp_verify,0},
	
	{"setmovecall",resp_setmovecall,0},
	{"getmovecall",resp_getmovecall,0},
	{"setphonebook",resp_setphonebook,0},
	{"queryphonebook",resp_queryphonebook,0},
	
	{"testphonecall",resp_testphonecall,0},
	
	{"getadc",resp_getadc,0},

	{"update",resp_update,0},
	{"getctversion",resp_getctversion,0},
	{"program",resp_program,0},
	{"isp",isp,0},
	{"test=",test,0},
	{"AT",atcmd,0},
	{"setgps",setgps,0},
	{"testgps",testgps,0},
};

char *process_xmpp_msg(char *from , char *msg)
{
	int i=0;
	static char resp_str[RESP_BUFFER_LEN];
	
	if (strstr(msg,"NULL") > 0)
	{
		resp_str[0] = 0;
		goto RET;
	}
	
	for(i=0;i<((sizeof(xmpp_msg_pm))/(sizeof(struct XMPP_MSG_PM)));i++)
	{
		if (strstr(msg,xmpp_msg_pm[i].msg) == msg)
		{
			xmpp_msg_pm[i].cb(from,msg,resp_str);
			DEBUGL->debug("## resp str: [%s]\r\n",resp_str);
			return resp_str;
		}
	}
	//resp_str[0] = 0x0;
	sprintf(resp_str,"%s=NO_SUPPORT_CMD",msg);
	
	RET:
	return resp_str;
}

void init_web_trace_tmr(void)
{
	//
}