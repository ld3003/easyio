#ifndef __xmpp_h__
#define __xmpp_h__


#define XMPP_JID_MAXLEN 128
#define XMPP_MESSAGE_MAXLEN 128

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
	enum {

#ifdef ENABLE_SSL
		XMPP_SSLHANDSHAKE_0,
		XMPP_SSLHANDSHAKE_1,
		XMPP_SSLHANDSHAKE_2,
		XMPP_SSLHANDSHAKE_3,
#endif
		XMPP_SINIT,
		XMPP_SLOGIN,
		XMPP_WAITRESULT,
		XMPP_SLOGINSUCC,
		XMPP_SLOGINFAIL,
		XMPP_BINDING,
		XMPP_BINDSUCC,
	};
	
	
	enum {
		RECV_NORMAL_XML_PKG = 0,
		RECV_IBBDAA_FMT,
		RECV_IBBDAA_FINISH,
	};





	struct XMPP_PMA
	{
		char username[64];
		char password[64];
		char domain[64];
		int port;
		char jid[XMPP_JID_MAXLEN];

		unsigned int connceted_timep;
		unsigned int disconnect_timep;
		unsigned int ping_timep;
		unsigned int pkg_timep;
		unsigned int recv_ping_cnt;
		unsigned int recv_pkg_cnt;

		int xmpp_status;
		int recv_xml_pkg_status;
		
		char tmp_id[32];
		char tmp_jid[64];

		//static int xmpp_status = XMPP_SINIT; //XMPP ???

	};



	struct XMPP_CB_FUNC {
		int (*send_buffer)(unsigned char *buf , int len);
		void (*dis_connect)(void);
		void (*login)(void);
		void (*recv_message)(char *from , char *body);
		void (*recv_package)(char *xml_pkg);
		char (*recv_subscribe)(char *from);				//����1���ܺ������룬����0 �ܾ���������
		void (*error)(int errcode);
		
		void (*ibbstart)(void);
		void (*ibbrecv)(char *base64 , int len);
		void (*ibbend)(void);
	};




	extern struct XMPP_CB_FUNC xmpp_cb_func;
	extern struct XMPP_PMA xmpp_pma;
	/* ��ʼ��XMPP CLI */
	extern void init_xmpp_cli(const char *username , const char *password , const char *domain , const long hostport);

	/* ����Presence�ӿ�*/
	extern int xmpp_set_presence(const char * presence_show , const char *presence_status);

	/* ����Message �ӿ�*/
	extern int xmpp_send_msg(const char * to , const char * body);

	/* ����Ping �ӿ�*/
	extern int xmpp_send_ping(void);
	
	/* �ǳ� */
	extern int logout_xmpp(void);

	/* ���� �ص�����*/
	extern void set_xmpp_cb_func(struct XMPP_CB_FUNC *func);

	/* TCP���� */
	extern void disconn_xmpp_event(void);
	
	/*TCP Auth ���ӳɹ�*/
	extern void connected_xmpp_event(void);
	
	/*TCP Bind�ɹ�*/
	extern void bind_xmpp_event(void);
	
	/*�յ�TCP����*/
	extern int recv_data_event(char *xmldata,int len);
	
	/*���ͽ��ܺ�������*/
	extern int send_subscribed(char *jid);
	
	/***
	XMPP �����ȶ��ԵĸĽ�
	2015.07.20 : �жϽ��ܵ�ping���Ƿ�Ϸ���������Ϸ�����Ϊ�������Ѿ�ʧЧ���ر�TCP����
	*/
	
	 /*xmpp login packet start send time*/
	extern unsigned int xmpp_start_login_time;
	extern unsigned int xmpp_start_bind_time;
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
