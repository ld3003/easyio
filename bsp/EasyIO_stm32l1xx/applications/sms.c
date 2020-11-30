#include "sms.h"
#include "at_cmd.h"
#include "gsmmux/easyio_cmux.h"
#include <stdio.h>
#include "process_xmpp.h"
#include <string.h>

static void __smsrecv(const char *phonenum , const char *smsbody , const char *xat)
{
	char *resp;
	char Z = 0x1A;int at_cmd_ret_code;
	char atbuffer[32];
	snprintf(atbuffer,sizeof(atbuffer),"AT+CMGS=\"%s\"\r\n",phonenum);
	cmux_at_command(2,atbuffer,AT_CMGS,10,&at_cmd_ret_code);
	resp = process_xmpp_msg((char*)phonenum,(char*)smsbody);
	make_cmux_pkg((unsigned char*)resp,strlen(resp),2);
	make_cmux_pkg((unsigned char*)&Z,1,2);
	//
}

void init_sms_callback(void)
{
	smsrecv = __smsrecv;
	//
}

void send_sms_api(const char *phonenum , const char *smsbody)
{
	__smsrecv(phonenum,smsbody,0);
}