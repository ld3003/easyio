#include "lwip_raw_api_tcp_template.h"
#include "XMPP/xmpp.h"
#include "xmpp_tcp.h"
#include "global_env.h"
#include "xmpp_msg_queue.h"
#include "lwip/tcpip.h"
#include "SHA1/sha1_password.h"
#include "gnss.h"
#include "led.h"
#include "MMA845X.h"
#include "app_timer.h"
#include "common.h"
#include "app_timer.h"
#include "my_stdc_func/my_stdc_func.h"
#include "lowpower.h"
#include "adc.h"
#include "at_cmd.h"
#include "gps_tracker.h"
#include "ublox_gps.h"

#include <stdio.h>

#define xmpp_tcp_client tcp_client_buffer[XMPP_TCP_CLIENT]
#define PRESENCE_STATUS_STRLEN 512

static unsigned int last_recv_vaild_pkg_timep = 0;
static unsigned int update_presence_tim  = 0;

unsigned int xmpp_login_timep = 0;


//连续追踪参数
char webtrace_jid[64];
int webtrace_cnt = -1;
unsigned int webtrace_tim_preiod = 0;


static void presence_item_to_string(const struct TRACKER_BUFFER_DATA *item , char *presence_status_str , char type)
{
	unsigned int _speed;
	char tmp_str[32];
	
	snprintf(presence_status_str,PRESENCE_STATUS_STRLEN,"CT03-");
	
	switch(item->gps_status)
	{
		case 'V':
			strcat(presence_status_str,"V");
			break;
		case 'A':
			strcat(presence_status_str,"A");
			break;
		case 'O':
			strcat(presence_status_str,"OFF");
			break;
		default:
			break;
	}
	
	strcat(presence_status_str,"-");
	
	if (item->mma845x_status <= 0)
		strcat(presence_status_str,"Stay");
	else
		strcat(presence_status_str,"Moving");
	
	strcat(presence_status_str,"-");
	
	strcat(presence_status_str,"Idle");
	strcat(presence_status_str,"-");
	
	switch(item->bat_status)
	{
		case 7:
			strcat(presence_status_str,"FULL");
			break;
		case 6:
			strcat(presence_status_str,"AC");
			break;
		case 5:
			strcat(presence_status_str,"BAT5");
			break;
		case 4:
			strcat(presence_status_str,"BAT4");
			break;
		case 3:
			strcat(presence_status_str,"BAT3");
			break;
		case 2:
			strcat(presence_status_str,"BAT2");
			break;
		case 1:
			strcat(presence_status_str,"BAT1");
			break;
		case 0:
			strcat(presence_status_str,"BAT0");
			break;
		default:
			strcat(presence_status_str,"FULL");
			break;
	}
	
	if (type == 1)
	{
		strcat(presence_status_str,"-");
		
		//下面是定期更新
		strcat(presence_status_str,"BC_GPS=");
		snprintf(tmp_str,sizeof(tmp_str),"%f",item->google_LAT);
		strcat(presence_status_str,tmp_str);
		strcat(presence_status_str,",");
		snprintf(tmp_str,sizeof(tmp_str),"%f",item->google_LON);
		strcat(presence_status_str,tmp_str);
		strcat(presence_status_str,",");
		_speed = item->speed;
		snprintf(tmp_str,sizeof(tmp_str),"%d",_speed);
		strcat(presence_status_str,tmp_str);
		
		//加入时间的信息
		strcat(presence_status_str,",");
		//snprintf(tmp_str,sizeof(tmp_str),"ddmmyyhhmmss");
		snprintf(tmp_str,sizeof(tmp_str),"%02d%02d%02d%02d%02d%02d",
		item->day,
		item->mon,
		item->year,
		item->hour,
		item->min,
		item->sec
		
		);
		
		strcat(presence_status_str,tmp_str);
	}

	//
	//
}

static void __xmpp_srv_connected(void *arg)
{
	
	last_recv_vaild_pkg_timep = app_timer_data.app_timer_second;
	connected_xmpp_event();
	//
}

static void __xmpp_srv_disconnect(void *arg)
{
	disconn_xmpp_event();
	//
}

static void __xmpp_srv_recv(void *arg , unsigned char *buf , int len)
{
	recv_data_event((char*)buf,len);
	//
}

void generate_send_presence(char mod)
{
	//LOCK_TCPIP_CORE();
	//struct TRACKER_BUFFER_DATA
	
	//创建item
	char *presence_buffer;
	static struct TRACKER_BUFFER_DATA his_item0;
	static struct TRACKER_BUFFER_DATA his_item1;
	
	struct TRACKER_BUFFER_DATA item;
	
	presence_buffer = rt_malloc(PRESENCE_STATUS_STRLEN);
	if (presence_buffer <= 0)
		return;

	item.google_LAT = gps_info.google_LAT;
	item.google_LON = gps_info.google_LON;
	item.speed = gps_info.SPEED;

	
	//mma845x_status = DEVICE_MOV_TIME;
	if (__is_moving() > 0)
		item.mma845x_status = 1;
	else
		item.mma845x_status = 0;
	
	//根据GPS开关状态
	if (gps_enable_flag == 1)
	{
		switch(gps_info.gprmc.status)
		{
			case 'A':
			case 'V':
				item.gps_status = gps_info.gprmc.status;
				break;
			default:
				item.gps_status = 'V';
				break;
		}
	}
	else
	{
		item.gps_status = 'O';
	}
	
	item.bat_status = get_bat_pre();
	item.year = gps_info.his_gprmc.utc.year;
	item.mon = gps_info.his_gprmc.utc.mon;
	item.day = gps_info.his_gprmc.utc.day;
	item.hour = gps_info.his_gprmc.utc.hour;
	item.min = gps_info.his_gprmc.utc.min;
	item.sec = gps_info.his_gprmc.utc.sec;
	
	//如果是模式1，nocache模式
	if (mod == 1)
	{
		if (tracker_item_cmp_STATUS(&item,&his_item1) != 0)
		{
			LOCK_TCPIP_CORE();
			//将presence item 转换为字符串
			memcpy(&his_item1,&item,sizeof(struct TRACKER_BUFFER_DATA));
			presence_item_to_string(&item,presence_buffer,0);
			if (xmpp_set_presence("online",presence_buffer) != 0)
			{
				
				//
			} else {
				
			}
			UNLOCK_TCPIP_CORE();
			
		}else
		{
			DEBUGL->debug("ignore presence !!!!!\r\n");
		}
	
		goto ret;
	}
	
	
	if (mod == 2)
	{
		if (/*tracker_item_cmp_STATUS(&item,&his_item) != 0*/1)
		{
			LOCK_TCPIP_CORE();
			//将presence item 转换为字符串
			//memcpy(&his_item,&item,sizeof(struct TRACKER_BUFFER_ITEM));
			presence_item_to_string(&item,presence_buffer,0);
			if (xmpp_set_presence("online",presence_buffer) != 0)
			{
				
				//
			} else {
				
			}
			UNLOCK_TCPIP_CORE();
			
		}else
		{
			DEBUGL->debug("ignore presence !!!!!\r\n");
		}
	
		goto ret;
	}
	
	//将presence item 转换为字符串
	if (mod == 0)
	{
		
		//确定历史状态
		if (tracker_item_cmp_BCGPS(&item,&his_item0) != 0)
		{
			LOCK_TCPIP_CORE();
			memcpy(&his_item0,&item,sizeof(struct TRACKER_BUFFER_DATA));
			presence_item_to_string(&item,presence_buffer,1);
			EnQueue(&tracker_buffer_qu,item);
			DEBUGL->debug("EnQueue %s \r\n",presence_buffer);
			
			switch(tracker_buffer_qu_flag)
			{
				case TRACKER_BUFFER_QU_STATUS_IDLE:
					
				//如果队列中的个数大于或者等于限制总数
					if (tracker_buffer_qu.count >= tracker_private_data.conf->PRESENCE_BC_COUNT)
					{
						tracker_buffer_qu_flag = TRACKER_BUFFER_QU_STATUS_BUSY;
					}
					break;
				case TRACKER_BUFFER_QU_STATUS_BUSY:
					break;
			}

			
		}else
		{
			DEBUGL->debug("ignore presence !!!!!\r\n");
		}
	}
	
	ret:
	
	rt_free(presence_buffer);
}


static void __xmpp_srv_routing(void *arg)
{
	
	LOCK_TCPIP_CORE();
	//如果是连接状态的时候就检查多久没连接了
	if (TCP_CONNECTED == xmpp_tcp_client.active_status)
	{
		
		
		if (xmpp_pma.xmpp_status == XMPP_BINDSUCC)
		{
			//如果正常登陆到了服务器
			if ((app_timer_data.app_timer_second - last_recv_vaild_pkg_timep) > 60*10)
			{
				xmpp_send_ping();
				//
			}
			
			if ((app_timer_data.app_timer_second - last_recv_vaild_pkg_timep) > 60*11)
			{
				MY_TCP_Client_stop(&xmpp_tcp_client);
				//
			}
			
		}else
		{
			//如果未正常登陆到服务器
			//
			
			if ((app_timer_data.app_timer_second - last_recv_vaild_pkg_timep) > 20)
			{
				MY_TCP_Client_stop(&xmpp_tcp_client);
			}
			
		}
	}
	
	UNLOCK_TCPIP_CORE();
	
	if (xmpp_pma.xmpp_status == XMPP_BINDSUCC)
	{
		//___flicker_led(1,100);
	}
	
	return ;

}

static void xmpp_disconn(void)
{
	MY_TCP_Client_pause(&xmpp_tcp_client);
}

static int xmpp_sendbuf(unsigned char *buf , int len)
{
	return MY_TCP_Client_write(&xmpp_tcp_client,buf,len);
	//
}
extern char *process_xmpp_msg(char *from , char *msg);
static void xmpp_rcvmsg(char *from , char *body)
{
	xmpp_send_msg(from,(char*)process_xmpp_msg(from,body));
}

static void xmpp_login(void)
{
	xmpp_login_timep = app_timer_data.app_timer_second;
	//make_presence_status_str();
	//xmpp_set_presence("online","IDLE");
	generate_send_presence(2);
	//
}
static void recv_package(char *xml_pkg)
{
	last_recv_vaild_pkg_timep = app_timer_data.app_timer_second;
	return ;
}


void webtrace_routing(void *p)
{
	static unsigned int send_trance_tim = 0;
	
	//if (webtrace_flag > 1)
	{
		if (webtrace_cnt > 0)
		{
				DEBUGL->debug("webtrace_cnt : %d \r\n",webtrace_cnt);
			if ((app_timer_data.app_timer_second - send_trance_tim) > webtrace_tim_preiod)
			{
				//如果回报间隔大于60则使用现有GPS休眠机制，不做唤醒,如果小于则唤醒2min
				if (webtrace_tim_preiod > 60)		
					xmpp_send_msg(webtrace_jid,process_xmpp_msg(webtrace_jid,"getgprmc_nowake=raw"));
				else
					xmpp_send_msg(webtrace_jid,process_xmpp_msg(webtrace_jid,"getgprmc=raw"));
				
				send_trance_tim = app_timer_data.app_timer_second;			
				webtrace_cnt --;
			}
		} 	else if (webtrace_cnt == 0 ) {
			//发送END指令
			xmpp_send_msg(webtrace_jid,"webtraceon=END");
			webtrace_cnt =-1;
		}
		
		
		//
	}
}


//定期发送presence
void setting_presence_routring(void *p)
{
	const struct TRACKER_BUFFER_DATA *itemp;
	char *presence_buffer;
	
	//检查发送
	if ((app_timer_data.app_timer_second - update_presence_tim) >= PRESENCE_UPDATE_PREIOD)
	{
		//发送presence
		generate_send_presence(0);
		update_presence_tim = app_timer_data.app_timer_second;
		//
	}
	
	switch(tracker_buffer_qu_flag)
	{
		case TRACKER_BUFFER_QU_STATUS_BUSY:
		{
			presence_buffer = rt_malloc(PRESENCE_STATUS_STRLEN);
			if (presence_buffer <= 0)
				break;
			
			itemp = getDeQueue2(&tracker_buffer_qu);
			if (itemp > 0)
			{
				
				presence_item_to_string(itemp,presence_buffer,1);
				LOCK_TCPIP_CORE();
				if (xmpp_set_presence("online",presence_buffer) == 0)
				{
					DEBUGL->debug("presence body : %s \r\n",presence_buffer);
					DEBUGL->debug("pop presence !!!!!!!!!!!!!!!!!!!!\r\n");
					getDeQueue(&tracker_buffer_qu);
				}
				UNLOCK_TCPIP_CORE();
			}
			rt_free(presence_buffer);
		
			if (tracker_buffer_qu.count == 0)
				tracker_buffer_qu_flag = TRACKER_BUFFER_QU_STATUS_IDLE;
			
			break;
		}
		case TRACKER_BUFFER_QU_STATUS_IDLE:
		{
			break;
		}
	}
	//
}


void init_xmpp_tcp(void)
{
	char xmpp_uname[32];
	struct XMPP_CB_FUNC func;
	//crtMV0xGJuoUAd3RC0RRtdPSXaw
	char *password = rt_malloc(64);
	if (password <= 0)
		return ;
	
	InitQueue(&tracker_buffer_qu);
	
	make_sha1_pwd(MODEM_IMEI,password);
	snprintf(xmpp_uname,sizeof(xmpp_uname),"ct03_%s",MODEM_IMEI);
	if (strlen(user_app_conf.ACCOUNT) > 0)
	{
		snprintf(xmpp_uname,sizeof(xmpp_uname),"%s",user_app_conf.ACCOUNT);
		//make_sha1_pwd(user_app_conf.ACCOUNT,password);
	}
	DEBUGL->debug("Account=%s , PWS=%s , Server=%s , Port=%d :\r\n",XMPP_USERNAME,XMPP_PASSWORD,XMPP_SERVER_HOSTNAME,XMPP_SERVER_PORT);
	init_xmpp_cli(XMPP_USERNAME,/*"V0y7cuomRkzR3QEUtVFEC6xd0tM="*/XMPP_PASSWORD,XMPP_SERVER_HOSTNAME,XMPP_SERVER_PORT);
	//init_xmpp_cli("test888","test888",XMPP_SERVER_HOSTNAME,XMPP_SERVER_PORT);
	rt_free(password);
	
	//创建一个断线重新连接的,openfire 自动发送心跳包，所以不需要keeplive
	MY_TCP_Client_Init(&xmpp_tcp_client,XMPP_SERVER_HOSTNAME,XMPP_SERVER_PORT,1,0);
	
	xmpp_tcp_client.connected = __xmpp_srv_connected;
	xmpp_tcp_client.disconn = __xmpp_srv_disconnect;
	xmpp_tcp_client.recv = __xmpp_srv_recv;
	xmpp_tcp_client.routing = __xmpp_srv_routing;
	
	memset(&func,0x0,sizeof(func));
	
	func.dis_connect = xmpp_disconn;
	func.send_buffer = xmpp_sendbuf;
	func.recv_message = xmpp_rcvmsg;
	func.login = xmpp_login;
	func.recv_package = recv_package;
	
	set_xmpp_cb_func(&func);
	
	MY_TCP_Client_start(&xmpp_tcp_client);
	
}
