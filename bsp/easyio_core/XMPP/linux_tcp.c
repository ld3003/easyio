

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#include "xmpp.h"

#define MAXLINE 1024

int port = 5222;


void xmpp_tcp_connected(void *p)
{
	connected_xmpp_event();
	printf("connect\r\n");
	//
}
void xmpp_tcp_connerr(void *p)
{
	disconn_xmpp_event();
	printf("connerr\r\n");
	//
}
void xmpp_tcp_disconnect(void *p)
{
	disconn_xmpp_event();

	printf("disconnect\r\n");
	//
}

void xmpp_tcp_recv(unsigned char *buf , unsigned int len)
{
	recv_data_event(buf,len);
	printf("recv buf len %d \r\n",len);
	//
}

#if 0
a                void (*ibbstart)(void);
                void (*ibbrecv)(char *base64 , int len);
                void (*ibbend)(void);

#endif
#include "MD5/md5sum.h"
#include "BASE64/cbase64.h"
static struct dc_stream ds;
static md5_state_t state;
static md5_byte_t digest[16];

void ibbstart(void)
{
	start_decode_base64_stream(&ds);
	md5_init(&state);
	//
}

void ibbrecv(char *base64 , int len)
{
	unsigned char tmp;
	int i=0;
	printf("====== \r\n");
	for(i=0;i<len;i++)
	{
		printf("%c",base64[i]);
	
	}

	printf("\r\n");
	for(i=0;i<len;i++)
	{
		//printf("%c",base64[i]);

		if (base64[i] == '=')
		{
			start_decode_base64_stream(&ds);
			continue;
		}

		if (decode_base64_stream(base64[i],&tmp,&ds) == 0)
		{
			printf("[%02X]",tmp);

			md5_append(&state, (const md5_byte_t *)(&tmp),1);
		}
	}
	printf("\r\n");
	//
}

void ibbend(void)
{
	unsigned int di;
	char str[64];
	snprintf(str,sizeof(str),"FW MD5:");

	md5_finish(&state, digest);

	for (di = 0; di < 16; ++di)
	{
		char md5str[4];
		snprintf(md5str,sizeof(md5str),"%02x",digest[di]);
		strcat(str,md5str);
	}

	printf("IBB MD5 : %s \r\n",str);
	//
}

int send_buffer(unsigned char *buf , unsigned int size);
void dis_connect(void);
void login(void);
void recv_message(char *from , char *body);


static int sock_fd;

int main(int argc, char *argv[])
{
	struct sockaddr_in pin;
	char buf[MAXLINE];
	char str[MAXLINE];
	int n;
	struct XMPP_CB_FUNC func;

	struct hostent *ht = NULL;
	
	//signal(SIGPIPE,SIG_IGN);

	//printf("11111");
	init_xmpp_cli("test3","test3","115.28.44.147",5222);
	//void init_xmpp_cli(const char *username , const char *password , const char *domain , const long hostport);
	memset(&func,0x0,sizeof(func));
	func.send_buffer = send_buffer;
	func.dis_connect = dis_connect;
	func.login = login;
	func.recv_message = recv_message;
	func.ibbstart = ibbstart;
	func.ibbend = ibbend;
	func.ibbrecv = ibbrecv;
	set_xmpp_cb_func(&func);
	

	for(;;)
	{

		bzero(&pin, sizeof(pin));
		pin.sin_family = AF_INET;
		inet_pton(AF_INET, "115.28.44.147", &pin.sin_addr);
		pin.sin_port = htons(port);

		sock_fd = socket(AF_INET, SOCK_STREAM, 0);
		n=connect(sock_fd, (void *)&pin, sizeof(pin));
		if (-1 == n)
		{
			printf("conn err\r\n");
			xmpp_tcp_connerr(0);
			close(sock_fd);
			sleep(1);
			continue;
		}

		xmpp_tcp_connected(0);

		for(;;)
		{

			n=read(sock_fd, buf, MAXLINE-1);

			if (n > 0)
			{
				buf[n] = 0x0;
				xmpp_tcp_recv(buf,n);

				//
			}else
			{
				close(sock_fd);
				break;


			}
		}


	}

	return 0;
}

int send_buffer(unsigned char *buf , unsigned int size)
{
	return write(sock_fd,buf,size);
	//
}

void dis_connect(void)
{
	close(sock_fd);
	//
}

void login(void)
{
	printf("login\r\n");
	//
}

void recv_message(char *from , char *body)
{

	printf("From %s Body %s \r\n",from,body);
	xmpp_send_msg(from,body);
	//
}
