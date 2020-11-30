#ifndef __xmpp_tcp_h__
#define __xmpp_tcp_h__

extern struct TcpClientCon xmpp_tcp_client;

void init_xmpp_tcp(void);
void setting_presence_routring(void*p);
void webtrace_routing(void *p);
void generate_send_presence(char mod);
//webtrace
#define WEBTRACE_ON			1
#define WEBTRACE_OFF		0

extern char webtrace_jid[64];							/*JID*/
extern unsigned char webtrace_flag;				/*标记是否启动*/
extern int webtrace_cnt;					/*标记上传个数*/
extern unsigned int webtrace_tim_preiod;	/*标记上传时间间隔*/
extern unsigned int xmpp_login_timep;			/*记录最近一次登陆的时间*/
#endif