#include "ota_http_tcp.h"
#include "lwip_raw_api_tcp_template.h"
#include "MD5/md5sum.h"
#include "flash_opt.h"
#include "XMPP/xmpp.h"
#include "my_stdc_func/my_stdc_func.h"
#include "my_stdc_func/debugl.h"

#include "w25q16.h"
#include "spiflash_config.h"
#include "app_timer.h"
#include "json/cJSON.h"
#include "gps_tracker.h"
#include "process_xmpp.h"

#include <rtthread.h>

//HTTP_TCP_CLIENT

static char download_from[32];

struct OTA_DOWNLOAD_INFO {
	unsigned short version;
	char version_name[16];
	char filename[32];
	int fw_file_size;
	char mode[16];
	char checksum[32+4];
};

static struct OTA_DOWNLOAD_INFO target_download_fw_info;


char test_ota_flag = OTA_FLAG_VERSIONNUM_MODE_CFG;		//����Ƿ��ǵ���OTA�ı���, 0 ���� �� 1 ����CONFIG���������� �� 2 ������config������ mode ����

unsigned int start_ota_active_timep = 0; //���ڼ�¼OTA�����ʱ�䣬�Ա���г�ʱ����

char check_flash(unsigned int address , int size , unsigned char *target_md5_data);


enum{
	HTTP_STATUS_INIT,
	HTTP_STATUS_START,
	HTTP_STATUS_STARTED,
	HTTP_DOWNLOADING,
	HTTP_DOWNLOAD_FINISH,
	HTTP_ERROR,
	HTTP_FINISH,
};

enum{
	HTTP_GET_CONFIG,
	HTTP_GET_FW,
};

static md5_state_t state;
static md5_byte_t digest[16];
static char hex_output[16*2 + 1];

static int http_tcp_status = HTTP_STATUS_INIT;
static int http_get_status = HTTP_GET_CONFIG;

static unsigned int download_size;	//��ǰ���صĳ���
static unsigned int content_length; //�ܳ���

//#define TEST_OTA

#define OTA_HOST_NAME						"www.jareymobile.com.tw"
#define OTA_CONFIG_FILENAME			"/ota/ct03ota.bin.conf"
#define OTA_FIRMWARE_FILENAME		"/ota/ct03ota.bin"


static char http_host[64] = "" ; //{OTA_HOST_NAME}; //www.jareymobile.com.tw/ota/ct03_ota.conf
static int http_port = 80;

static void send_httpget_pkg(char *path , char *host)
{
	char *buffer = rt_malloc(128);
	if (buffer > 0)
	{
		
		snprintf(buffer,128,"GET %s HTTP/1.1\r\nHOST: %s\r\nConnection: keep-alive\r\n\r\n",path,host);
		DEBUGL->debug("REQUEST : %s",buffer);
		MY_TCP_Client_write(&tcp_client_buffer[HTTP_TCP_CLIENT],(unsigned char*)buffer,strlen(buffer));
		rt_free(buffer);
	}else
	{
		http_tcp_status = HTTP_ERROR;
		
	}
}


int HTTP_GetContentLength(char *revbuf)
{
    char *p1 = NULL, *p2 = NULL;
    int HTTP_Body = 0;

    p1 = strstr(revbuf,"Content-Length");
    if(p1 == NULL)
        return -1;
    else
    {
        p2 = p1+strlen("Content-Length")+ 2; 
        HTTP_Body = atoi(p2);
        return HTTP_Body;
    }

}


static void recv_http_buf(unsigned char *buf , int len)
{
	//if (content_length)
	switch(http_get_status)
	{
		case HTTP_GET_FW:
		{
			
			char str[64];
			snprintf(str,sizeof(str),"download %d %d \r\n",download_size ,content_length);
			//xmpp_send_msg(download_from,str);
			DEBUGL->info("%s",str);
			
			md5_append(&state, (const md5_byte_t *)buf,len);
			append_w25x_append_buffer(buf,len);
			//
			break;
		}
		case HTTP_GET_CONFIG:
		{
			//json
			char *out;cJSON *json,*json_value;
	
			json=cJSON_Parse((char *)buf);
			if (!json) {rt_kprintf("Error before: [%s]\n",cJSON_GetErrorPtr());}
			else
			{
				
				json_value = cJSON_GetObjectItem( json , "filename");
				if( json_value->type == cJSON_String )
				{
					rt_kprintf("filename:%s\r\n",json_value->valuestring);
					snprintf(target_download_fw_info.filename,sizeof(target_download_fw_info.filename),"%s",json_value->valuestring);
				}
				
				json_value = cJSON_GetObjectItem( json , "version_name");
				if( json_value->type == cJSON_String )
				{
					rt_kprintf("version_name:%s\r\n",json_value->valuestring);
					snprintf(target_download_fw_info.filename,sizeof(target_download_fw_info.filename),"%s",json_value->valuestring);
				}
				
				json_value = cJSON_GetObjectItem( json , "checksum");
				if( json_value->type == cJSON_String )
				{
					rt_kprintf("checksum:%s\r\n",json_value->valuestring);
					snprintf(target_download_fw_info.checksum,sizeof(target_download_fw_info.checksum),"%s",json_value->valuestring);
				}
				
				json_value = cJSON_GetObjectItem( json , "mode");
				if( json_value->type == cJSON_String )
				{
					 snprintf(target_download_fw_info.mode,sizeof(target_download_fw_info.mode),"%s",json_value->valuestring);
					 rt_kprintf("mode:%s\r\n",json_value->valuestring);  
				}
				
				json_value = cJSON_GetObjectItem( json , "size");
				if( json_value->type == cJSON_Number )
				{
					 rt_kprintf("filesize:%d\r\n",json_value->valueint);  
					target_download_fw_info.fw_file_size = json_value->valueint;
				}
				
				json_value = cJSON_GetObjectItem( json , "version");
				if( json_value->type == cJSON_Number )
				{
					 rt_kprintf("version:%d\r\n",json_value->valueint);  
					target_download_fw_info.version = json_value->valueint;
				}
				
				//out=cJSON_Print(json);
				cJSON_Delete(json);
				//rt_kprintf("%s\n",out);
				//rtfree(out);
			}
			
			DEBUGL->info("CONFIG BODY: %s",buf);
			break;
		}
	}
	
	
	//
}

static void recv(void *arg,unsigned char *buf , int len)
{
	
	//ÿ���յ����ݣ��͸���һ�¼�¼OTA����timer
	start_ota_active_timep = app_timer_data.app_timer_second ;
	
	switch(http_tcp_status)
	{
		case HTTP_STATUS_STARTED:
		{
			
			DEBUGL->debug("response : %s \r\n",buf);
			content_length = HTTP_GetContentLength((char*)buf);
			
			if (content_length > 0)
			{
				char *data_pos = (strstr((char*)buf,"\r\n\r\n"));
				if (data_pos > 0)
				{
					data_pos += 4;
					download_size = 0;
					

					http_tcp_status = HTTP_DOWNLOADING;
					
					download_size = (unsigned int)len - ((unsigned int)data_pos - (unsigned int)buf);
					recv_http_buf((u8*)data_pos,(unsigned int)len - ((unsigned int)data_pos - (unsigned int)buf));
					
					
					if (download_size == content_length)
					{	
						http_tcp_status = HTTP_DOWNLOAD_FINISH;
					}
				}
				//
			}else
			{
				http_tcp_status = HTTP_ERROR;
			}
			break;
		}
		
		case HTTP_DOWNLOADING:
			download_size += len;
			recv_http_buf((u8*)buf,len);
			
			if (download_size == content_length)
			{
				http_tcp_status = HTTP_DOWNLOAD_FINISH;
			}
			break;
		
		default:
			break;
	
	}
	
}



static void connected(void *arg)
{
	DEBUGL->debug("http server connected !!!\r\n");
	
	#if 1
	switch(http_get_status)
	{
		case HTTP_GET_FW:
			DEBUGL->debug("@@@@@@@@@@@@@@@@@@@@@@@@@ downloading fw ...\r\n");
		
			snprintf(http_host,sizeof(http_host),OTA_HOST_NAME);
			send_httpget_pkg(OTA_FIRMWARE_FILENAME,OTA_HOST_NAME);
		
			//send_httpget_pkg("/rthread.bin","www.easy-io.net");
			md5_init(&state);
		
			init_write_flash(SPI_FLASH_FW_ADDRESS,0);
		
			break;
		case HTTP_GET_CONFIG:
			rt_kprintf("@@@@@@@@@@@@@@@@@@@@@@@@@ downloading config ...\r\n");

			snprintf(http_host,sizeof(http_host),OTA_HOST_NAME);
			send_httpget_pkg(OTA_CONFIG_FILENAME,OTA_HOST_NAME);
		
			break;
	}
	#endif
	
	//send_httpget_pkg("/rtthread.bin","www.easy-io.net");
}



#include "firmware.h"

static void disconn(void *arg)
{
	unsigned char *buff;
	int rcnt=0;
	int total = content_length;
	unsigned int di;
	
	
	if (http_get_status == HTTP_GET_FW)
	{

		md5_byte_t downfw_digest[16]; //����̼������ع����м�������� MD5SUM
		//����Flash д�룬������MD5
		struct FIRMWARE_HEADER firmware_header;
		char str[64];
		snprintf(str,sizeof(str),"FW MD5:");
		
		md5_finish(&state, digest);
		
		w25x_append_end();
		
		for (di = 0; di < 16; ++di)
		{
			char md5str[4];
			snprintf(md5str,sizeof(md5str),"%02x",digest[di]);
			strcat(str,md5str);
		}
		DEBUGL->info("%s\r\n",str);

		memcpy(downfw_digest,digest,sizeof(downfw_digest));

		
		
		//�ж�MD5�Ƿ�� config �ĵ��е� MD5 ���Ǻ� , �����Ƿ�Ҳ���
		if ((strstr(str,target_download_fw_info.checksum) > 0) && (content_length == target_download_fw_info.fw_file_size))
		{
			//���飬flash�б�����Ƿ�������е�����MD5�Ǻϣ�Ϊ���ж�Flash�Ƿ񱣴�ɹ�
			if (check_flash(SPI_FLASH_FW_ADDRESS,content_length,downfw_digest) == 1)
			{
				
				//д���ò���
				readconfig();
				memcpy(spiflash_conf.TARGET_FW_MD5,digest,16);
				spiflash_conf.TARGET_FW_SIZE = content_length;
				spiflash_conf.ota_status_machine = OTA_STATUS_MACHINE_DOWNLOAD_FINISH; //����״̬��
				writeconfig();
				
				DEBUGL->debug("content_length %d \r\n",content_length);
				SPI_Flash_Read((unsigned char*)&firmware_header,SPI_FLASH_FW_ADDRESS,sizeof(firmware_header));							//��ȡ�̼�ͷ��Ϣ
				DEBUGL->debug("fhlen %d \r\n",firmware_header.size);
				
				logout_xmpp();
				
				rt_thread_sleep(RT_TICK_PER_SECOND);
				
				__set_FAULTMASK(1);
				NVIC_SystemReset();
				
				for(;;){};
			}
		}
		
	}
	
	http_tcp_status = HTTP_FINISH;
	//
}

void debugbuffer(char *name , unsigned char *buf , int len)
{
//	int i=0;
//	rt_kprintf("Buffer name : %s \r\n",name);
//	for(i=0;i<len;i++)
//	{
//		rt_kprintf("[%02X]",buf[i]);
//	}
//	rt_kprintf("\r\n");
}

#include "w25q16.h"
// ���� 0  ����У��ʧ�ܣ����� 1 ����У��ɹ�
char check_flash(unsigned int address , int size , unsigned char *target_md5_data)
{
	int i=0;
	unsigned char *buff;
	int rcnt=0;
	int total = size;
	unsigned int di;
	
	//if (http_get_status == HTTP_GET_FW)
	{
		char str[64];
	
		#define BUFFER_SIZE_RFLASH 256
		buff = rt_malloc(BUFFER_SIZE_RFLASH);
		if (buff <= 0)
		{
			DEBUGL->debug("MALLOC ERR\r\n");
			for(;;);
		}
		
		//
		//Debug flash md5
		md5_init(&state);
		
		while(total >0)
		{

			if (total > BUFFER_SIZE_RFLASH)
			{
				SPI_Flash_Read(buff,address + (rcnt*BUFFER_SIZE_RFLASH),BUFFER_SIZE_RFLASH);
				
				debugbuffer("READ",buff,BUFFER_SIZE_RFLASH);
				
				md5_append(&state,buff,BUFFER_SIZE_RFLASH);
				total -= BUFFER_SIZE_RFLASH;
				
				
				
			}else {
				SPI_Flash_Read(buff,address + (rcnt*BUFFER_SIZE_RFLASH),total);
				debugbuffer("READ",buff,total);
				md5_append(&state,buff,total);
				total = 0;
				
			}
			
			rcnt ++;
		}
		
		md5_finish(&state, digest);
		snprintf(str,sizeof(str),"FLASH MD5:");
		for (di = 0; di < 16; ++di)
		{
			char md5str[4];
			snprintf(md5str,sizeof(md5str),"%02x",digest[di]);
			strcat(str,md5str);
		}
		DEBUGL->debug("%s\r\n",str);


		//����md5�Ƿ�ƥ��
		for(i=0;i<16;i++)
		{
			if (digest[i] != target_md5_data[i])
				return 0;
		}
		
	
	}

	return 1;
	
}

void testspiflash(void)
{
	
	#define LENGTH 1000
	unsigned int di;
	char str[64];
	md5_init(&state);
	md5_append(&state,(unsigned char*)0x8040000,LENGTH);
	md5_finish(&state, digest);
	snprintf(str,sizeof(str),"FLASH MD5:");
	for (di = 0; di < 16; ++di)
	{
		char md5str[4];
		snprintf(md5str,sizeof(md5str),"%02x",digest[di]);
		strcat(str,md5str);
	}
	DEBUGL->debug("%s\r\n",str);
	
	debugbuffer("read",(unsigned char*)0x8040000,LENGTH);
		
	
	init_write_flash(4096,0);
	append_w25x_append_buffer((unsigned char*)0x8040000,LENGTH);
	w25x_append_end();
	
	check_flash(4096,LENGTH,digest);
}


static void connerr(void *arg)
{
	http_tcp_status = HTTP_ERROR;
	
}
static void routing(void *arg)
{
}

void init_ota_http_cli(void)
{
	memset(&target_download_fw_info,0x0,sizeof(target_download_fw_info));
	MY_TCP_Client_Init(&tcp_client_buffer[HTTP_TCP_CLIENT],http_host,http_port,0,1);
	INIT_TCPCLI_CB_FUNC(HTTP_TCP_CLIENT);
	//
}

static void start_ota(void)
{
	config_fq_2();
	ota_restart_http_download();
}

//check_config , ���� 0  ����ʧ�� ������ 1 ����ɹ� ���� 2 ����ɹ�������Ϊ�ֶ�����ģʽ
static char check_config(void)
{
	
	//����Ǳ���̼���ʱ���¼�������
	extern const char versionstr[];
	
	char *out;cJSON *json,*json_value;
	
	json=cJSON_Parse((char *)versionstr);
	if (!json) {rt_kprintf("Error before: [%s]\n",cJSON_GetErrorPtr());}
	else
	{
		
		json_value = cJSON_GetObjectItem( json , "version");
		if( json_value->type == cJSON_Number )
		{
			
			//��鵱ǰ�̼�,�����ͬ������
			if (json_value->valueint < target_download_fw_info.version)
			{
				
				//�������ֶ�ģʽ���
				if (test_ota_flag == OTA_FLAG_VERSIONNUM_CFG)
					return 1;
				
				//��ʼ�����Ƿ�Ϊ Auto ģʽ
				//���������Զ�������1���ֶ�����2
				if (strstr(target_download_fw_info.mode,"auto")) return 1;
				if (strstr(target_download_fw_info.mode,"manually")) return 2;
	
			}
			
		}
	}

	
	return 0;
}

void ota_http_tcp_routing(void *p)
{

	//����30���Ӽ��ota��Ȼ��ÿ12Сʱ���һ��ota
	static unsigned char ota_timer = 0;	
	if(ota_timer <= 0)
	{
		
		if (app_timer_data.app_timer_second > 60*30)			//����30���ӻ��� ota
		{
			//��� ota
			process_xmpp_msg("NULL","checkota");
			ota_timer = app_timer_data.app_timer_second;
		}else{
			if ((app_timer_data.app_timer_second - ota_timer) >= 60*60*12)
			{
				process_xmpp_msg("NULL","checkota");
				ota_timer = app_timer_data.app_timer_second;
			}
		}
		
	}
	
	
	//OTA ��ʱ����
	if (http_tcp_status != HTTP_STATUS_INIT)
	{
		if((app_timer_data.app_timer_second - start_ota_active_timep) > 60)
		{
			//2����û�з�Ӧ���ͽ������OTA	
			MY_TCP_Client_pause(&tcp_client_buffer[HTTP_TCP_CLIENT]);
			http_tcp_status = HTTP_STATUS_INIT;
			
		}
	}
	
	switch(http_tcp_status)
	{
		case HTTP_STATUS_INIT:		
			//��Ϊ�͹���ģʽ
			config_fq_1();
			//NULL
			break;
		case HTTP_STATUS_START:

			MY_TCP_Client_start(&tcp_client_buffer[HTTP_TCP_CLIENT]);
			http_tcp_status = HTTP_STATUS_STARTED;
			break;
		case HTTP_STATUS_STARTED:
			break;
		case HTTP_DOWNLOADING:
			{
				if (http_get_status == HTTP_GET_CONFIG)
				{
					tracker_private_data.ota_status = STATUS_DOWNLOAD_CONF;
				}else if (http_get_status == HTTP_GET_FW){
					tracker_private_data.ota_status = STATUS_START_DOWNLOAD;
				}
				break;
			}
		case HTTP_DOWNLOAD_FINISH:
			LOCK_TCPIP_CORE();
			TCP_RUN_TEST;
			MY_TCP_Client_stop(&tcp_client_buffer[HTTP_TCP_CLIENT]);
			TCP_RUN_TEST;
			UNLOCK_TCPIP_CORE();
			break;
		case HTTP_ERROR:
			http_tcp_status = HTTP_STATUS_START;
			break;
		case HTTP_FINISH:
			//�����ǰ������CONFIG
			if (http_get_status == HTTP_GET_CONFIG)
			{
				
				//���Config�����Ƿ�Ϸ���������Ϸ��Ͳ����м���������
				
				if (check_config() == 1)
				{
					http_tcp_status = HTTP_STATUS_START;
					http_get_status = HTTP_GET_FW;
				}else{
					//������ز���
					http_tcp_status = HTTP_STATUS_INIT;
				}
				
				//���ֻ�Ǽ��OTA״̬�򲻽��й̼�����
				if (check_config() == 2)
				{
					tracker_private_data.ota_status = STATUS_UPGRADE_MANUALLY;
					http_tcp_status = HTTP_STATUS_INIT;
					http_get_status = HTTP_GET_CONFIG;
					//
				}
				
				//
			}else if (http_get_status == HTTP_GET_FW){
				//�����������̼��ˣ���ô��ֹͣ
				LOCK_TCPIP_CORE();
				TCP_RUN_TEST;
				MY_TCP_Client_pause(&tcp_client_buffer[HTTP_TCP_CLIENT]);
				TCP_RUN_TEST;
				UNLOCK_TCPIP_CORE();
				http_tcp_status = HTTP_STATUS_INIT;
				http_get_status = HTTP_GET_FW;
			}
			break;
		
	}
	//
}

void ota_restart_http_download(void)
{
	
	if (tracker_private_data.gsm_signal < 10)
		goto FAIL_RET;
	if (tracker_private_data.bat_val < 3700)
		goto FAIL_RET;
	
	MY_TCP_Client_stop(&tcp_client_buffer[HTTP_TCP_CLIENT]);
	http_tcp_status = HTTP_STATUS_START;
	return;
	FAIL_RET:
	DEBUGL->debug("START OTA FAILED %d %d \r\n",tracker_private_data.gsm_signal,tracker_private_data.bat_val);
	return;
	//

}


char ota_start_http_download(char *from , char *body)
{
	//�ж��ǲ��ǿ���״̬
	if (http_tcp_status == HTTP_STATUS_INIT)
	{
		snprintf(http_host,sizeof(http_host),"%s",OTA_HOST_NAME);
		start_ota_active_timep = app_timer_data.app_timer_second ;
		
		//���³�ʼ��
		init_ota_http_cli();
		
		snprintf(download_from,sizeof(download_from),from);
		http_tcp_status = HTTP_STATUS_START;
		return 1;
	}
	return 0;
}
