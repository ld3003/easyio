#include "xmpp.h"
//#include "xml/ezxml.h"
#include "xmpp_str.h"

#include "my_stdc_func/my_stdc_func.h"
#include "my_stdc_func/debugl.h"
#include "app_timer.h"
#include "xmpp_helper.h"
#include "my_stdc_func/my_stdc_func.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#ifdef __LINUX__
#define DEBUGL->debug printf
#define rt_malloc malloc
#define rt_free free
#else
#define malloc rt_malloc
#define free rt_free
#endif

//#define RUN_TEST rt_kprintf("RUNTEST ______________ %s : %d \r\n",__LINE__,__FILE__)

unsigned int xmpp_start_login_time = 0; /*xmpp login packet start send time*/
unsigned int xmpp_start_bind_time = 0; /*xmpp login packet start send time*/

struct RV_MSG_PM {
    char *from;
    char *body;
};


#define XMPP_RUN_TEST DEBUGL->debug("__xmpp_runtest FUNC:%s LINE:%d \r\n",__FUNCTION__,__LINE__);


#define xENABLE_SSL


struct XMPP_PMA xmpp_pma;
struct XMPP_CB_FUNC xmpp_cb_func;
#define RESET_PRIV_VAL()	\
    xmpp_pma.connceted_timep = 0;	\
    xmpp_pma.disconnect_timep = 0;	\
    xmpp_pma.ping_timep = 0;	\
    xmpp_pma.pkg_timep = 0;	\
    xmpp_pma.recv_ping_cnt = 0;	\
    xmpp_pma.recv_pkg_cnt = 0;
//


void init_xmpp_cli(const char *username , const char *password , const char *domain , const long hostport )
{
    
    xmpp_pma.xmpp_status = XMPP_SINIT;
    sprintf(xmpp_pma.username,"%s",username);
    sprintf(xmpp_pma.password,"%s",password);
    sprintf(xmpp_pma.domain,"%s",domain);
    xmpp_pma.port = hostport;
    
    RESET_PRIV_VAL();
    //memcpy(&network_func,network_fn,sizeof(struct XMPP_NETWORK_FUNC));
    
}

static int send_tcp_str(char *str)
{
    DEBUGL->debug(">>>>>>>>>>>>>>>>  %s \r\n",str);
    
    if (xmpp_cb_func.send_buffer > 0)
        return xmpp_cb_func.send_buffer((unsigned char*)str , strlen(str));
    else
        return -1;
    
}

/*
TCP Connection OK , start send xmpp login packet to server.
*/
void connected_xmpp_event(void)
{
    char *tmp;
    tmp = (char*)rt_malloc(512);
    if (tmp>0)
    {
				//Record start login time.
				xmpp_start_login_time = app_timer_data.app_timer_second;
			
				xmpp_pma.xmpp_status = XMPP_SLOGIN;
				//send stream:stream packet
        get_header1_str(xmpp_pma.domain,tmp);
        send_tcp_str(tmp);
			
				rt_thread_sleep(RT_TICK_PER_SECOND/10);
			
        //send auth packet
			  get_auth_str(xmpp_pma.username,xmpp_pma.password,tmp);
        send_tcp_str(tmp);
			

				rt_free(tmp);
    }
}

void bind_xmpp_event(void){
	char *tmp;
	tmp = (char*)rt_malloc(256);
	if (tmp>0)
	{
			xmpp_start_bind_time = app_timer_data.app_timer_second;
			get_bind_str(xmpp_pma.username,xmpp_pma.domain,tmp);
			send_tcp_str(tmp);
			xmpp_pma.xmpp_status = XMPP_BINDING;
			rt_free(tmp);

	}
}

void set_xmpp_cb_func(struct XMPP_CB_FUNC *func)
{
    memcpy(&xmpp_cb_func,func,sizeof(struct XMPP_CB_FUNC));
    //
}

void disconn_xmpp_event(void)
{
    xmpp_pma.xmpp_status = XMPP_SINIT; //XMPP 状态机
    RESET_PRIV_VAL();
    //
}


static void _free_p(struct RV_MSG_PM *p)
{
    rt_free(p->body);
    rt_free(p->from);
    rt_free(p);
}


//static int xxprocess_message2(void *ptr)
//{
//	struct RV_MSG_PM *p = (struct RV_MSG_PM *)ptr;
//	xmpp_send_msg(p->from,(char*)process_xmpp_msg(p->from,p->body));
//	_free_p(p);
//	return 0;
//}


static void append_str(char *str)
{
    static char ibbbuffer[1024*5] = {0x0};
    strcat(ibbbuffer,str);
    
    
}

static int process_message(struct RV_MSG_PM *p)
{
    //如果事件推送失败则销毁
    
    if (xmpp_cb_func.recv_message > 0)
        xmpp_cb_func.recv_message(p->from,p->body);
    return 0;
}


static int process_xmlstr_data(const char *xml)
{
    DEBUGL->debug("<<<<<<<< %s\n",xml);
    DEBUGL->debug("\r\n\r\n\r\nEND%%%%%%%%%%\r\n\r\n");
    
    if (xmpp_cb_func.recv_package > 0)
    {
        xmpp_cb_func.recv_package((char*)xml);
    }
    
    if (strstr(xml,"failure xmlns"))
    {
			DEBUGL->debug("@@@@@@@@@@@@@@@@@ XMPP_SLOGINFAIL\r\n");
			xmpp_pma.xmpp_status = XMPP_SLOGINFAIL;
    }
    else if (strstr(xml,"success xmlns"))
    {
        bind_xmpp_event();
        xmpp_pma.xmpp_status = XMPP_SLOGINSUCC;
       

    }
    else if (strstr(xml,"<iq type=\"result\"") && strstr(xml,"iq>"))
    {
        if (strstr(xml,"</bind></iq>"))
        {
            
            if (strstr(xml,"</jid>"))
            {
                
                get_str_fragment(xml,"<jid>","</jid>",xmpp_pma.jid,XMPP_JID_MAXLEN);
                //
                //
            }
            
            xmpp_pma.xmpp_status = XMPP_BINDSUCC;
            DEBUGL->debug("@@@@@@@@@@@@@@@@@ XMPP_BINDSUCC\r\n");
            //LOCK_TCPIP_CORE();
            xmpp_set_presence("online","");
            //UNLOCK_TCPIP_CORE();
            if (xmpp_cb_func.login > 0)
                xmpp_cb_func.login();
            //post_apisysevent(SYSEVENT_XMPP_LOGGED,0);
            
        }
        else if (strstr(xml,"<ping"))
        {
            
            //
        }
        
        //
    }else if (strstr(xml,"type=\"get\"") && strstr(xml,"iq>"))
    {		
			//收到iq报文
        if (strstr(xml,"<ping"))
        {
            char *__rs_ping;
            xmpp_pma.recv_ping_cnt ++;
            __rs_ping = (char*)rt_malloc(256);
            if (__rs_ping > 0)
            {
                char pingid[64];
							//获取IQ的id
                get_xml_att("id=",xml,pingid,64);
                
//                if (strstr(iqid,xmpp_pma.jid) <= 0)
//                {
//                    //错误测PING包，要进行断线处理
//                    //
//                    
//                    if (xmpp_cb_func.error > 0)
//                    {
//                        xmpp_cb_func.error(0);
//                    }
//                }
                
                //get_ping_rs_str
                get_ping_rs_str(xmpp_pma.domain,xmpp_pma.jid,pingid,__rs_ping,256);
                send_tcp_str(__rs_ping);
                rt_free(__rs_ping);
            }
            
            
            //
        }
    }
    else if (strstr(xml,"type=\"set\"") && strstr(xml,"<iq"))
    {
        
        if (strstr(xml,"si xmlns=\"http://jabber.org/protocol/si\""))
        {
            char *__rs_ping = (char*)rt_malloc(512);
            if (__rs_ping > 0)
            {
                char iqid[32];
                char iqfrom[64];
                get_xml_att("id=",xml,iqid,32);
                get_xml_att("from=",xml,iqfrom,64);
                
                DEBUGL->debug("AAAAAAAAAAAAAAAAAAAA from %s id %s \r\n",iqfrom,iqid);
                
                //get_ping_rs_str
                get_ibb_rs_str(iqfrom,xmpp_pma.jid,iqid,__rs_ping,512);
                send_tcp_str(__rs_ping);
                rt_free(__rs_ping);
            }
        }else if (strstr(xml,"<data xmlns="))
        {
            
            char *tmp;
            char *tmp2;
            
            DEBUGL->debug("CHANGE STATUS RECV_IBBDAA_FMT\r\n");
            get_xml_att("id=",xml,xmpp_pma.tmp_id,sizeof(xmpp_pma.tmp_id));
            get_xml_att("from=",xml,xmpp_pma.tmp_jid,sizeof(xmpp_pma.tmp_jid));
            
            
            //decode_base64
            
            xmpp_pma.recv_xml_pkg_status = RECV_IBBDAA_FMT;
            if (xmpp_cb_func.ibbstart > 0)
            {
                xmpp_cb_func.ibbstart();
            }
            XMPP_RUN_TEST;
            tmp = strstr(xml,"<data xmlns=");
            if (tmp > 0)
            {
                XMPP_RUN_TEST;
                tmp = strstr(tmp,"\">");
                if (tmp > 0)
                {
                    XMPP_RUN_TEST;
                    tmp += 2;
                    
                    tmp2 = strstr(tmp,"</data></iq>");
                    if (tmp2 > 0)
                    {
                        char *__rs_ping = (char*)rt_malloc(512);
                        
                        if (xmpp_cb_func.ibbrecv > 0)
                        {
                            XMPP_RUN_TEST;
                            xmpp_cb_func.ibbrecv(tmp,tmp2-tmp);
                        }
                        
                        if (__rs_ping > 0)
                        {
                            
                            DEBUGL->debug("IBB DATA ACK\r\n");
                            get_ping_rs_str(xmpp_pma.tmp_jid,xmpp_pma.jid,xmpp_pma.tmp_id,__rs_ping,512);
                            send_tcp_str(__rs_ping);
                            rt_free(__rs_ping);
                        }
#if 0
                        
                        xmpp_pma.recv_xml_pkg_status = RECV_NORMAL_XML_PKG;
                        if (xmpp_cb_func.ibbend > 0)
                        {
                            xmpp_cb_func.ibbend();
                        }
#endif
                        
                        //
                        //
                    }
                    else
                    {
                        if (xmpp_cb_func.ibbrecv > 0)
                        {
                            XMPP_RUN_TEST;
                            xmpp_cb_func.ibbrecv(tmp,strlen(tmp));
                        }
                    }
                }
            }
            
            //decode_base64
            
            
            //
        }else if (strstr(xml,"open xmlns=\"http://jabber.org/protocol/ibb\""))
        {
            //echo result
            char *__rs_ping = (char*)rt_malloc(512);
            if (__rs_ping > 0)
            {
                char iqid[32];
                char iqfrom[64];
                get_xml_att("id=",xml,iqid,32);
                get_xml_att("from=",xml,iqfrom,64);
                
                DEBUGL->debug("BBBBBBBBBBBBBBBBBBBBBBBB from %s id %s \r\n",iqfrom,iqid);
                
                //get_ping_rs_str
                
                get_ping_rs_str(iqfrom,xmpp_pma.jid,iqid,__rs_ping,512);
                send_tcp_str(__rs_ping);
                rt_free(__rs_ping);
            }
            //
            //close xmlns="http://jabber.org/protocol/ibb
        }else if (strstr(xml,"close xmlns"))
        {
            //echo result
            char *__rs_ping = (char*)rt_malloc(512);
            if (__rs_ping > 0)
            {
                char iqid[32];
                char iqfrom[64];
                get_xml_att("id=",xml,iqid,32);
                get_xml_att("from=",xml,iqfrom,64);
                
                DEBUGL->debug("CLOSE from %s id %s \r\n",iqfrom,iqid);
                
                //get_ping_rs_str
                
                get_ping_rs_str(iqfrom,xmpp_pma.jid,iqid,__rs_ping,512);
                send_tcp_str(__rs_ping);
                rt_free(__rs_ping);
            }
            //
        }
        
        
        
        //
    }
    else if (strstr(xml,"<presence"))
    {
        //char *type_str = strstr(xml,"type=");
        //if (type_str)
        //{
        char *body_buffer;
        body_buffer = (char*)rt_malloc(256);
        if (body_buffer > 0)
        {
#define PRESENCE_TYPE_STR_LEN	32
            char *presence_type = body_buffer;
#define PRESENCE_FROM_STR_LEN	64 
            char *presence_from = body_buffer + PRESENCE_TYPE_STR_LEN;
#define PRESENCE_TO_STR_LEN	64
            char *presence_to = body_buffer + PRESENCE_TYPE_STR_LEN + PRESENCE_FROM_STR_LEN;
            
            
            if (get_xml_att("type=",xml,presence_type,PRESENCE_TYPE_STR_LEN))
            {
                if (strstr(presence_type,"subscribe"))
                {
                    
                    
                    if (get_xml_att("from=",xml,presence_from,PRESENCE_TYPE_STR_LEN))
                    {
                        char *__buffer;
                        
                        //加入callback 如果有回调函数，用户可以选择性的接受或者拒绝好友添加
                        if (xmpp_cb_func.recv_subscribe > 0)
                        {
                            if (xmpp_cb_func.recv_subscribe(presence_from) == 1)
                            {
                                
                                __buffer  = (char*)rt_malloc(256);
                                if (__buffer)
                                {
                                    snprintf(__buffer,256,"<presence id='%d' to='%s' type='subscribed'/>",get_xmpp_id(),presence_from);
                                    send_tcp_str(__buffer);
                                    rt_free(__buffer);
                                }
                                
                            }else{
                                
                            }
                        } else {
                            
                            //如果没有回调函数，则默认接受所有添加好友请求
                            __buffer  = (char*)rt_malloc(256);
                            if (__buffer)
                            {
                                snprintf(__buffer,256,"<presence id='%d' to='%s' type='subscribed'/>",get_xmpp_id(),presence_from);
                                send_tcp_str(__buffer);
                                rt_free(__buffer);
                            }
                        }
                    }
                    
                }
                //筛选
            }
            rt_free(body_buffer);
        }
        
        //}
        //get presence type
        
        //
    }else if (strstr(xml,"<message"))
    {
        char * str1;
        char * str2;
#define __MAX_FROM_LEN XMPP_JID_MAXLEN
#define __MAX_BODY_LEN XMPP_MESSAGE_MAXLEN
        char * __recv_from;
        struct RV_MSG_PM *__p;
        char * __recv_message;
        __recv_from = (char*)rt_malloc(__MAX_FROM_LEN);
        __recv_message = (char*)rt_malloc(__MAX_BODY_LEN);
        __p  = (struct RV_MSG_PM*)rt_malloc(sizeof(struct RV_MSG_PM));
        
        
        
        
        
        if ((__recv_from <= 0) || (__p <= 0) || (__recv_message <= 0))
        {
            if (__recv_from)
                rt_free(__recv_from);
            if (__p)
                rt_free(__p);
            if (__recv_message)
                rt_free(__recv_message);
        }
        else
        {
            
            //rt_kprintf("MEM ADDRESS__________ 0x%X , 0x%X , 0x%X \n",__recv_from,__p,__recv_message);
            sprintf(__recv_from,"NULL");
            sprintf(__recv_message,"NULL");
            
            __p->from = __recv_from;
            __p->body = __recv_message;
            
            if (__recv_from > 0)
            {
                str1 = strstr(xml,"from=\"");
                if (str1 > 0)
                {
                    
                    str1 += 6;
                    str2 = strstr(str1,"\"");
                    if (str2 > 0)
                    {
                        str2[0] = 0x0;
                        snprintf(__recv_from,__MAX_FROM_LEN,str1);
                        str2[0] = '\"';
                    }
                }
                
            }
            
            if (__recv_message > 0)
            {
                str1 = strstr(xml,"<body>");
                if (str1 > 0)
                {
                    str1 += 6;
                    str2 = strstr(str1,"</body>");
                    if (str2 > 0)
                    {
                        str2[0] = 0x0;
                        snprintf(__recv_message,__MAX_BODY_LEN,str1);
                        str2[0] = '<';
                    }
                }
                
            }
            
#if 1
            process_message(__p);
            _free_p(__p);
#endif
            
        }
        
    }
    
    return 0;
}


int send_subscribed(char *jid)
{
	char *__buffer  = (char*)rt_malloc(256);
  if (__buffer)
	{
		snprintf(__buffer,256,"<presence id='%d' to='%s' type='subscribed'/>",get_xmpp_id(),jid);
		send_tcp_str(__buffer);
		rt_free(__buffer);
	}
	
	return 0;
	//
}


int recv_data_event(char *xmldata,int len)
{
    switch(xmpp_pma.recv_xml_pkg_status)
    {
    case RECV_NORMAL_XML_PKG:
    {
        process_xmlstr_data(xmldata);
        break;
    }
    case RECV_IBBDAA_FMT:
    {
        
        DEBUGL->debug("RECV IBB str %s \r\n",xmldata);
        
        if (strstr(xmldata,"close xmlns=\"http://jabber.org/protocol/ibb\"") > 0)
        {
            //
            
            DEBUGL->debug("CLOSE IBB\r\n");
            
            xmpp_pma.recv_xml_pkg_status = RECV_NORMAL_XML_PKG;
            if (xmpp_cb_func.ibbend > 0)
            {
                xmpp_cb_func.ibbend();
            }
            return 0;
        }
        
        if (strstr(xmldata,"</data></iq>"))
        {
            
#if 1
            char *tmp = strstr(xmldata,"</data></iq>");
            char *__rs_ping = (char*)rt_malloc(512);
            DEBUGL->debug("BASE64 RAWDATA TAIL\r\n");
            if (xmpp_cb_func.ibbrecv > 0)
            {
                xmpp_cb_func.ibbrecv(xmldata,tmp - xmldata);
            }
            
            
            if (__rs_ping > 0)
            {
                
                DEBUGL->debug("IBB DATA ACK\r\n");
                get_ping_rs_str(xmpp_pma.tmp_jid,xmpp_pma.jid,xmpp_pma.tmp_id,__rs_ping,512);
                send_tcp_str(__rs_ping);
                rt_free(__rs_ping);
            }
            
            
#endif
        }else if (strstr(xmldata,"<data xmlns="))
        {
            
            char *tmp,*tmp2;
            XMPP_RUN_TEST;
            
            get_xml_att("id=",xmldata,xmpp_pma.tmp_id,sizeof(xmpp_pma.tmp_id));
            get_xml_att("from=",xmldata,xmpp_pma.tmp_jid,sizeof(xmpp_pma.tmp_jid));
            
            tmp = strstr(xmldata,"<data xmlns=");
            tmp = strstr(tmp,"\">");
            if (tmp > 0)
            {
                XMPP_RUN_TEST;
                tmp += 2;
                tmp2 = strstr(tmp,"</data></iq>");
                if (tmp2 > 0)
                {
                    char *__rs_ping = (char*)rt_malloc(512);
                    
                    if (xmpp_cb_func.ibbrecv > 0)
                    {
                        XMPP_RUN_TEST;
                        xmpp_cb_func.ibbrecv(tmp,tmp2-tmp);
                    }
                    
                    if (__rs_ping > 0)
                    {
                        
                        DEBUGL->debug("IBB DATA ACK\r\n");
                        get_ping_rs_str(xmpp_pma.tmp_jid,xmpp_pma.jid,xmpp_pma.tmp_id,__rs_ping,512);
                        send_tcp_str(__rs_ping);
                        rt_free(__rs_ping);
                    }
                }else
                {
                    
                    DEBUGL->debug("RECV IBB DATA , HAVE HEAD\r\n");
                    if (xmpp_cb_func.ibbrecv > 0)
                    {
                        XMPP_RUN_TEST;
                        
                        xmpp_cb_func.ibbrecv(tmp,strlen(tmp));
                    }
                    //
                }
                
                //
                //
            }
            else
            {
                XMPP_RUN_TEST;
                DEBUGL->debug("RECV RAW DATA IBB \r\n");
                if (xmpp_cb_func.ibbrecv > 0)
                {
                    XMPP_RUN_TEST;
                    xmpp_cb_func.ibbrecv(tmp,strlen(tmp));
                }
            }
            //
        }
        else
        {
            DEBUGL->debug("BASE64 RAW DATA\r\n");
            if (xmpp_cb_func.ibbrecv > 0)
            {
                xmpp_cb_func.ibbrecv(xmldata,len);
            }
            
        }
        break;
    }
        
    default:
        break;
    }
    return 0;
}


int xmpp_send_msg(const char * to , const char * body)
{
    //<message to="test3@newpower022.com/Spark 2.6.3" type="chat"><body>Your message: 111111</body></message>
    int ret = 0;
#define MESSAGE_BUFFER_LEN 1024
    
    char *__msg_buf;
    
    if (xmpp_pma.xmpp_status != XMPP_BINDSUCC)
        return -1;
    
    if (strlen(body) <= 0)
        return ret;
    
    __msg_buf = (char*)rt_malloc(MESSAGE_BUFFER_LEN);
    if (__msg_buf > 0)
    {
        snprintf(__msg_buf,MESSAGE_BUFFER_LEN,"<message to=\"%s\" type=\"chat\"><body>%s</body></message>",to,body);
        ret = send_tcp_str(__msg_buf);
        rt_free(__msg_buf);
    }
    return ret;
}

int xmpp_send_ping(void)
{
    int ret = -1;
    char *pingstr = (char*)rt_malloc(128);
    if (pingstr > 0)
    {
        get_ping_str(xmpp_pma.domain,pingstr,128);
        ret = send_tcp_str(pingstr);
        rt_free(pingstr);
    }
    
    return ret;
    //
}

int xmpp_set_presence(const char * type , const char *presence_status)
{
    char *__presence;
    int ret = -1;
    if (xmpp_pma.xmpp_status != XMPP_BINDSUCC)
        return -1;
    
#define PRESENCE_LENGTH 1024
    __presence	= (char*)rt_malloc(PRESENCE_LENGTH);
    if (__presence > 0)
    {
			     
				if (strstr(type,"online"))
				{
					snprintf(__presence,PRESENCE_LENGTH,"<presence type='%s'><status>%s</status></presence>",type,presence_status);
				}else{
					snprintf(__presence,PRESENCE_LENGTH,"<presence><status>%s</status></presence>",presence_status);
				}

        ret = send_tcp_str(__presence);
        rt_free(__presence);
    }
    return ret;
}

int logout_xmpp(void)
{
    if (xmpp_pma.xmpp_status != XMPP_BINDSUCC)
        return -1;
    send_tcp_str("<presence type='unavailable'/></stream:stream>");
    return 0;
}






