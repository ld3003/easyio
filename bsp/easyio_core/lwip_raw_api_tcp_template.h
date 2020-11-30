#ifndef __lwip_raw_api_template__
#define __lwip_raw_api_template__

#include "lwip/tcp.h"
#include "lwip/dns.h"
#include "lwip/tcpip.h"

#include "common.h"



#define TCP_RUN_TEST //rt_kprintf("#################### RUN_TEST FILE %s FUNC %s LINE %d \r\n",__FILE__,__FUNCTION__,__LINE__);

enum{
	TCP_PAUSE,//��ͣ
	
	TCP_INIT,
	TCP_DNS_START,
	TCP_DNS_FINISH,
	TCP_CONNECTING,
	TCP_CONNECTED,
	
	TCP_DNS_ERR,
	TCP_CONNECT_ERR,
	TCP_DISCONNECT,
	TCP_RECONNECT_DELAY,
	
};


struct TcpClientCon{
	unsigned char used;
	struct tcp_pcb *pcb;
	struct ip_addr ipaddr;
	long port;
	char active_status;
	char hostname[64];
	char connect_type;
	unsigned int reconnection_time;
	char keepalive;
	
	unsigned int recv_ack_timestamp;
	
	unsigned char connecting_count; //�������ӵĴ���
	
	void (*recv)(void *arg,unsigned char *buf , int len);		//�յ����ݵĻص�����
	void (*connected)(void *arg);														//�ɹ����ӵ�tcpserver�Ļص�����
	void (*disconn)(void *arg);															//�Ͽ�tcp���ӵĻص�����
	void (*connerr)(void *arg);															//���Ӵ���Ļص�����
	void (*routing)(void *arg);															//
	
	void *arg;																							//�û��������
	
};




extern struct TcpClientCon tcp_client_buffer[TCP_CLI_COUNT];
void init_tcp_client_buf(void);
void rout_tcp_client_buf(void);
void disconn_tcp_client_buf(void);

/**
	������һ��TCP���� type == 1 ʱ�����⵽tcp�����жϣ����Զ��������� type == 0 ʱ���ߺ󲻻���������
	keepalive > 0 ������������tcp����ʱ�Զ�����������ȷ��tcpά������
*/
void MY_TCP_Client_Init(struct TcpClientCon *cli , char *hostname , long port , char type , char keepalive);

/**
  TCP״̬����麯������Ҫϵͳ���ڵ��ã��û�App�������
*/
void MY_TCP_Client_Routing(struct TcpClientCon *cli);

/**
  ֹͣһ��tcp����
*/
void MY_TCP_Client_stop(struct TcpClientCon *cli);

/**
  ��ʼһ��tcp����
*/
void MY_TCP_Client_start(struct TcpClientCon *cli);

/**
  ��ͣһ��tcp���ӣ����������Ϊ�û����ߵ�tcp���Ӷ���
*/
void MY_TCP_Client_pause(struct TcpClientCon *cli);

/**
  ��һ��tcp������д������
*/
err_t MY_TCP_Client_write(struct TcpClientCon *cli , unsigned char *buffer , int len);

#define INIT_TCPCLI_CB_FUNC(X) tcp_client_buffer[X].connected = connected;	\
	tcp_client_buffer[X].connerr = connerr;	\
	tcp_client_buffer[X].connerr = connerr;	\
	tcp_client_buffer[X].disconn = disconn;	\
	tcp_client_buffer[X].routing = routing;	\
	tcp_client_buffer[X].recv		 = recv;
	
#define INIT_TCPCLI_PTR_CB_FUNC_(X) X->connected = connected;	\
	X->connerr = connerr;	\
	X->connerr = connerr;	\
	X->disconn = disconn;	\
	X->routing = routing;	\
	X->recv		 = recv;

#endif
