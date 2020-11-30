#include "xmpp_helper.h"
#include "BASE64/cbase64.h"
#include "xmpp_str.h"
#include <string.h>
#include <stdio.h>

#ifdef __LINUX__
#define rt_kprintf printf
#define rt_free free
#define rt_malloc malloc
#else
//#define printf rt_kprintf
//#define free rt_free
//#define malloc rt_malloc
#endif

static int xmpp_xml_id = 0;
int get_xmpp_id(void)
{
	return xmpp_xml_id++;
}
int get_auth_str(char * username , char * password , char * out)
{


#if 1
	int i=0;

	char *buffer = (char*)rt_malloc(512);
	char *strbuf;
	char *strbuf2;
	if (buffer<=0)
		return 0;
	memset(buffer,0x0,512);
	strbuf = buffer;
	strbuf2 = buffer+256;

	for(i=0;i<strlen(username);i++)
	{
		if (username[i] == '\r' ||  username[i] == '\n')
		{
			username[i] = 0x0;
			//
		}
	}

	for(i=0;i<strlen(password);i++)
	{
		if (password[i] == '\r' ||  password[i] == '\n')
		{
			password[i] = 0x0;
			//
		}
	}

	strcat(strbuf2+1,username);
	strcat(strbuf2+strlen(username)+2,password);
	encode_base64(strbuf2,strlen(username)+strlen(password)+2,strbuf); //encode_base64
	out[0] = 0x0;
	strcat(out,LOGIN_STR_P1);
	strcat(out,strbuf);
	strcat(out,LOGIN_STR_P2);

	rt_free(buffer);
#else
	static char str[] = "<auth xmlns='urn:ietf:params:xml:ns:xmpp-sasl' mechanism='PLAIN'>ADYwYTg5Mjg4YzUyZGNlMjQ4Zjg2MWVjMjFiNGQwMTc1ADk2NmFlNTgwOWZjYjllODVhZTMzOTc1MmJlNDYxZjdk</auth>";
	sprintf(out,"%s",str);
#endif
	return 0;
}

int get_bind_str(char * username , char * domain , char *out)
{

	char *strbuf2 = (char*)rt_malloc(128);
	if (strbuf2 <= 0)
		return 0;

	memset(strbuf2,0x0,128);
	strcat(strbuf2,username);
	strcat(strbuf2,"@");
	strcat(strbuf2,domain);
	snprintf(out,sizeof(IQ_BIND_P1),IQ_BIND_P1);
	strcat(out,strbuf2);
	strcat(out,IQ_BIND_P2);
	rt_free(strbuf2);
	
	return 0;
}

int get_presence_str(char * jid , char *out)
{

	sprintf(out,"%s%s%s",PRESENCE_P1,jid,PRESENCE_P2);
	return 0;
}

int get_iq_session_str(char * jid , char *out)
{
	int i = 0;
	strcat(out,IQ_SESSION_P1);
	strcat(out,jid);
	strcat(out,IQ_SESSION_P2);
	return 0;
}

int get_header1_str(char *domain_ip,char *out)
{
	sprintf(out,"%s",HEADER1_1);
	return 0;
}

int get_ping_str(char *domain , char *out,int outlen)
{
#if 0
	<iq from='juliet@capulet.lit/balcony' to='capulet.lit' id='c2s1' type='get'>
	<ping xmlns='urn:xmpp:ping'/>
	</iq>
#endif
	snprintf(out,outlen,"<iq type='get'><ping xmlns='urn:xmpp:ping'/></iq>");
	return 0;
	//
}


//<iq id="FW579-207" to="test2@easy-io.net/Spark 2.7.0" from="test3@easy-io.net/Spark 2.6.3" type="result"></iq>

int get_ping_rs_str(char *dst , char *jid , char *pingid ,  char *out , int len)
{
	//<iq to="newpower022.com" id="843-14236" type="result" form="c1f16dd94328c9e4c949e4d65bb4936c@newpower022.com/dev" xmlns="jabber:client"></iq>
	//<iq id='%s' type='result'/>
	snprintf(out,len,"<iq id='%s' type='result'/>",pingid);
	//snprintf(out,len,"<iq id=\"%s\" to=\"%s\" from=\"%s\" type=\"result\"></iq>",pingid,dst,jid);
	//snprintf(out,len,"<iq to=\"%s\" id=\"%s\" type=\"result\" form=\"%s\"></iq>",dst,pingid,jid);
	//

	return 0;
}

//<iq id="FW579-204" to="test2@easy-io.net/Spark 2.7.0" from="test3@easy-io.net/Spark 2.6.3" type="result"><si xmlns="http://jabber.org/protocol/si"><feature xmlns="http://jabber.org/protocol/feature-neg"><x xmlns="jabber:x:data" type="submit"><field var="stream-method"><value>http://jabber.org/protocol/bytestreams</value><value>http://jabber.org/protocol/ibb</value></field></x></feature></si></iq>

int get_ibb_rs_str(char *dst , char *jid , char *pingid ,  char *out , int len)
{
	//<iq to="newpower022.com" id="843-14236" type="result" form="c1f16dd94328c9e4c949e4d65bb4936c@newpower022.com/dev" xmlns="jabber:client"></iq>

	snprintf(out,len,"<iq id=\"%s\" to=\"%s\" from=\"%s\" type=\"result\"><si xmlns=\"http://jabber.org/protocol/si\"><feature xmlns=\"http://jabber.org/protocol/feature-neg\"><x xmlns=\"jabber:x:data\" type=\"submit\"><field var=\"stream-method\"><value>http://jabber.org/protocol/bytestreams</value><value>http://jabber.org/protocol/ibb</value></field></x></feature></si></iq>",pingid,dst,jid);
	//

	return 0;
}

extern char *get_p(char *str_in , char *str_out , unsigned char out_lmt);
char *get_xml_att(char *head , const char *str_in , char * str_out , char out_lim)
{
	//
	char *search_str = strstr(str_in,head);
	if (search_str <= 0)
	{
		return 0;
		//
	}
	return get_p(search_str,str_out,out_lim);
}
