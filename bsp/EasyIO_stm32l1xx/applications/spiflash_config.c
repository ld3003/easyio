#include "spiflash_config.h"
#include "w25q16.h"
#include <string.h>
#include "my_stdc_func/my_stdc_func.h"
#include "my_stdc_func/debugl.h"
#include <rtthread.h>
#include <stdio.h>
#include "app_local_conf.h"
struct SPIFLASH_CONFIG spiflash_conf;
struct USER_APP_CONFIG user_app_conf;

void readconfig(void){
	unsigned short crc;
	char read_cnt = 0;
	
	
	spiflash_release_powerdown();
	

	REREAD:
	read_cnt ++;
	SPI_Flash_Read((unsigned char*)&spiflash_conf,CONFIG_ADDRESS,sizeof(struct SPIFLASH_CONFIG));
	
	crc = sdk_stream_crc16_calc((unsigned char *)&spiflash_conf,((unsigned int)(&spiflash_conf.CONFIG_CRC) - (unsigned int)(&spiflash_conf))); //����
	
	if (spiflash_conf.CONFIG_CRC == crc)
	{
		return ;
	}
	
	for(;;)
	{
	
		DEBUGL->debug("Read SYS CONFIG CRC ERR!");
		rt_thread_sleep(RT_TICK_PER_SECOND);
	}
}
void writeconfig(void){
	
	unsigned short crc;
	
	spiflash_release_powerdown();
	
	crc = sdk_stream_crc16_calc((unsigned char *)&spiflash_conf,((unsigned int)(&spiflash_conf.CONFIG_CRC) - (unsigned int)(&spiflash_conf))); 
	spiflash_conf.CONFIG_CRC = crc;
	
	init_write_flash(CONFIG_ADDRESS,0x0);
	append_w25x_append_buffer((unsigned char*)&spiflash_conf,sizeof(struct SPIFLASH_CONFIG));
	w25x_append_end();
	
	spiflash_powerdown();
}


void userapp_readconfig(void){
	
	char *json_buffer;
	unsigned short crc;
	char read_cnt = 0;
	
	
	spiflash_release_powerdown();
	#define flash_buffer_len 512
	#define JSON_FLAG 
	json_buffer = rt_malloc(flash_buffer_len);
	if (json_buffer > 0)
	{
		
		SPI_Flash_Read((unsigned char*)json_buffer,USER_APP_CONFIG_ADDRESS,flash_buffer_len);
		json_buffer[flash_buffer_len-1] = 0x0;
		
		//�жϴ�flash�ж��������ǲ�����Ч��json
		if (strstr(json_buffer,"JSON_FLAG") > 0)
		{

			//�������Чjson�ַ���
			parseJson(json_buffer);
			debug_user_config();
			
			//
		}else{
			//����ǲ�����Ч��BinConfig
			memcpy((unsigned char *)&user_app_conf,(unsigned char*)json_buffer,sizeof(struct USER_APP_CONFIG));
			crc = sdk_stream_crc16_calc((unsigned char *)&user_app_conf,((unsigned int)(&user_app_conf.CONFIG_CRC) - (unsigned int)(&user_app_conf))); //����
			
			//�������Ч��
			if (user_app_conf.CONFIG_CRC == crc)
			{
				//return ;
				userapp_writeconfig();
			}else {
				set_default_userconfig(1);
			}
			
			
		}
		
		
		
		
		rt_free(json_buffer);
		//
	}
	
	return;

	
	
	SPI_Flash_Read((unsigned char*)&user_app_conf,USER_APP_CONFIG_ADDRESS,sizeof(struct USER_APP_CONFIG));
	
	crc = sdk_stream_crc16_calc((unsigned char *)&user_app_conf,((unsigned int)(&user_app_conf.CONFIG_CRC) - (unsigned int)(&user_app_conf))); //����
	
	if (user_app_conf.CONFIG_CRC == crc)
	{
		return ;
	}
	
	DEBUGL->debug("Read USER CONFIG CRC ERR!");
	
	//set default value
	memset((unsigned char*)&user_app_conf,0x0,sizeof(user_app_conf));
	
	user_app_conf.PRESENCE_UPLOAD_PREIOD = DEFAULT_UPLOAD_PREIOD;
	snprintf(user_app_conf.HOST_ADDRESS,sizeof(user_app_conf.HOST_ADDRESS),"%s",DEFAULT_HOSTNAE);
	user_app_conf.HOST_PORT = 20030;
	
	userapp_writeconfig();
	
	spiflash_powerdown();
	//goto REREAD;
	
	//set default
	//
}

void set_default_userconfig(char writeflash)
{
	//set default value
	memset((unsigned char*)&user_app_conf,0x0,sizeof(user_app_conf));
	
	user_app_conf.PRESENCE_UPLOAD_PREIOD = DEFAULT_UPLOAD_PREIOD;
	snprintf(user_app_conf.HOST_ADDRESS,sizeof(user_app_conf.HOST_ADDRESS),"%s",DEFAULT_HOSTNAE);
	user_app_conf.MANUALLY_OTA = 0;
	snprintf(user_app_conf.ACCOUNT,sizeof(user_app_conf.ACCOUNT),"%s",DEFAULT_ACCOUNT);
	user_app_conf.HOST_PORT = DEFAULT_HOSTPORT;
	userapp_writeconfig();
	
	user_app_conf.APNINFO.CCID[0] = 0x0;
	user_app_conf.APNINFO.APN[0] = 0x0;
	user_app_conf.APNINFO.PASSWORD[0] = 0x0;
	user_app_conf.APNINFO.USERNAME[0] = 0x0;
	user_app_conf.PRESENCE_BC_COUNT = 0;
	
	if(writeflash == 1)
	{
		userapp_writeconfig();
		spiflash_powerdown();
	}
}

void userapp_writeconfig(void){
	
	char *jsonstr;
	unsigned short crc;
	
	spiflash_release_powerdown();
	
	
	jsonstr = makeJson();
	DEBUGL->debug("SPIFLASH WRITE JSON STR: %s \r\n",jsonstr);
	if (jsonstr > 0)
	{
		
		init_write_flash(USER_APP_CONFIG_ADDRESS,0x0);
		append_w25x_append_buffer((unsigned char*)jsonstr,strlen(jsonstr));
		w25x_append_end();
		spiflash_powerdown();
		
		rt_free(jsonstr);
		
	}
	
	return ;
	
	/*
	crc = sdk_stream_crc16_calc((unsigned char *)&user_app_conf,((unsigned int)(&user_app_conf.CONFIG_CRC) - (unsigned int)(&user_app_conf))); 
	user_app_conf.CONFIG_CRC = crc;
	
	init_write_flash(USER_APP_CONFIG_ADDRESS,0x0);
	append_w25x_append_buffer((unsigned char*)&user_app_conf,sizeof(struct USER_APP_CONFIG));
	w25x_append_end();
	
	spiflash_powerdown();
	
	*/
}


#include "json/cJSON.h"




char * makeJson(void)
{
		char * p;
    cJSON * pJsonRoot = NULL;
	
	//user_app_conf

    pJsonRoot = cJSON_CreateObject();
    if(NULL == pJsonRoot)
    {
        //error happend here
        return NULL;
    }

		cJSON_AddNumberToObject(pJsonRoot, "PRESENCE_UPLOAD_PREIOD", user_app_conf.PRESENCE_UPLOAD_PREIOD);
		cJSON_AddNumberToObject(pJsonRoot, "PRESENCE_BC_COUNT", user_app_conf.PRESENCE_BC_COUNT);
    cJSON_AddStringToObject(pJsonRoot, "HOST_ADDRESS", user_app_conf.HOST_ADDRESS);
    cJSON_AddNumberToObject(pJsonRoot, "HOST_PORT", user_app_conf.HOST_PORT);
		cJSON_AddStringToObject(pJsonRoot, "ACCOUNT", user_app_conf.ACCOUNT);
		cJSON_AddNumberToObject(pJsonRoot, "MANUALLY_OTA", user_app_conf.MANUALLY_OTA);
		
		//MOVINDEX
		cJSON_AddNumberToObject(pJsonRoot, "MOVINDEX", user_app_conf.movcall_index);
		cJSON_AddStringToObject(pJsonRoot, "phoneBook1", user_app_conf.phoneBook[0]);
		cJSON_AddStringToObject(pJsonRoot, "phoneBook2", user_app_conf.phoneBook[1]);
		cJSON_AddStringToObject(pJsonRoot, "phoneBook3", user_app_conf.phoneBook[2]);
		cJSON_AddStringToObject(pJsonRoot, "phoneBook4", user_app_conf.phoneBook[3]);
		
		//apn info
		cJSON_AddStringToObject(pJsonRoot, "CCID", user_app_conf.APNINFO.CCID);
		cJSON_AddStringToObject(pJsonRoot, "APN", user_app_conf.APNINFO.APN);
		cJSON_AddStringToObject(pJsonRoot, "USERNAME", user_app_conf.APNINFO.USERNAME);
		cJSON_AddStringToObject(pJsonRoot, "PASSWORD", user_app_conf.APNINFO.PASSWORD);
		
		cJSON_AddStringToObject(pJsonRoot, "JSON_FLAG", "JSON");
		
    p = cJSON_Print(pJsonRoot);
  // else use : 
    // char * p = cJSON_PrintUnformatted(pJsonRoot);
    if(NULL == p)
    {
        //convert json list to string faild, exit
        //because sub json pSubJson han been add to pJsonRoot, so just delete pJsonRoot, if you also delete pSubJson, it will coredump, and error is : double free
        cJSON_Delete(pJsonRoot);
        return NULL;
    }
    //free(p);
    
    cJSON_Delete(pJsonRoot);

    return p;
}


void parseJson(char * pMsg)
{
		cJSON * pSub;
		cJSON * pJson;
    if(NULL == pMsg)
    {
        return;
    }
    pJson = cJSON_Parse(pMsg);
    if(NULL == pJson)                                                                                         
    {
        // parse faild, return
      return ;
    }

		
    pSub = cJSON_GetObjectItem(pJson, "PRESENCE_UPLOAD_PREIOD");
    if(NULL != pSub)
    {
			rt_kprintf("PRESENCE_UPLOAD_PREIOD : %d\n", pSub->valueint);
			user_app_conf.PRESENCE_UPLOAD_PREIOD = pSub->valueint;
      //get object named "hello" faild
    }
		
		pSub = cJSON_GetObjectItem(pJson, "PRESENCE_BC_COUNT");
    if(NULL != pSub)
    {
			rt_kprintf("PRESENCE_BC_COUNT : %d\n", pSub->valueint);
			user_app_conf.PRESENCE_BC_COUNT = pSub->valueint;
      //get object named "hello" faild
    }
    
		
		 pSub = cJSON_GetObjectItem(pJson, "HOST_ADDRESS");
    if(NULL != pSub)
    {
			rt_kprintf("HOST_ADDRESS : %s\n", pSub->valuestring);
			sprintf(user_app_conf.HOST_ADDRESS,"%s",pSub->valuestring);
      //get object named "hello" faild
    }
    
		
		pSub = cJSON_GetObjectItem(pJson, "HOST_PORT");
    if(NULL != pSub)
    {
			rt_kprintf("HOST_PORT : %d\n", pSub->valueint);
			user_app_conf.HOST_PORT = pSub->valueint;
      //get object named "hello" faild
    }
    
		
		pSub = cJSON_GetObjectItem(pJson, "ACCOUNT");
    if(NULL != pSub)
    {
			rt_kprintf("ACCOUNT : %s\n", pSub->valuestring);
      sprintf(user_app_conf.ACCOUNT,"%s",pSub->valuestring);
    }
		
		pSub = cJSON_GetObjectItem(pJson, "MOVINDEX");
    if(NULL != pSub)
    {
			rt_kprintf("HOST_PORT : %d\n", pSub->valueint);
			user_app_conf.movcall_index = pSub->valueint;
      //get object named "hello" faild
    }
		
		
		pSub = cJSON_GetObjectItem(pJson, "phoneBook1");
    if(NULL != pSub)
    {
			rt_kprintf("phoneBook1 : %s\n", pSub->valuestring);
      sprintf(user_app_conf.phoneBook[0],"%s",pSub->valuestring);
    }
		
		pSub = cJSON_GetObjectItem(pJson, "phoneBook2");
    if(NULL != pSub)
    {
			rt_kprintf("phoneBook1 : %s\n", pSub->valuestring);
      sprintf(user_app_conf.phoneBook[1],"%s",pSub->valuestring);
    }
		
		pSub = cJSON_GetObjectItem(pJson, "phoneBook3");
    if(NULL != pSub)
    {
			rt_kprintf("phoneBook1 : %s\n", pSub->valuestring);
      sprintf(user_app_conf.phoneBook[2],"%s",pSub->valuestring);
    }
		
		pSub = cJSON_GetObjectItem(pJson, "phoneBook4");
    if(NULL != pSub)
    {
			rt_kprintf("phoneBook1 : %s\n", pSub->valuestring);
      sprintf(user_app_conf.phoneBook[3],"%s",pSub->valuestring);
    }
		
		
		pSub = cJSON_GetObjectItem(pJson, "MANUALLY_OTA");
    if(NULL != pSub)
    {
			rt_kprintf("MANUALLY_OTA : %d\n", pSub->valueint);
			user_app_conf.MANUALLY_OTA = pSub->valueint;
    }
		
		
		pSub = cJSON_GetObjectItem(pJson, "CCID");
    if(NULL != pSub)
    {
			rt_kprintf("CCID : %s\n", pSub->valuestring);
			sprintf(user_app_conf.APNINFO.CCID,"%s",pSub->valuestring);
      //get object named "hello" faild
    }
		
		pSub = cJSON_GetObjectItem(pJson, "APN");
    if(NULL != pSub)
    {
			rt_kprintf("APN : %s\n", pSub->valuestring);
			snprintf(user_app_conf.APNINFO.APN,sizeof(user_app_conf.APNINFO.APN),"%s",pSub->valuestring);
      //get object named "hello" faild
    }
		
		pSub = cJSON_GetObjectItem(pJson, "USERNAME");
    if(NULL != pSub)
    {
			rt_kprintf("USERNAME : %s\n", pSub->valuestring);
			//sprintf(user_app_conf.APNINFO.USERNAME,"%s",pSub->valuestring);
			snprintf(user_app_conf.APNINFO.USERNAME,sizeof(user_app_conf.APNINFO.USERNAME),"%s",pSub->valuestring);
      //get object named "hello" faild
    }
		
		pSub = cJSON_GetObjectItem(pJson, "PASSWORD");
    if(NULL != pSub)
    {
			rt_kprintf("PASSWORD : %s\n", pSub->valuestring);
			snprintf(user_app_conf.APNINFO.PASSWORD,sizeof(user_app_conf.APNINFO.PASSWORD),"%s",pSub->valuestring);
      //get object named "hello" faild
    }
		
    cJSON_Delete(pJson);
}

void debug_user_config(void)
{
	
	rt_kprintf("+--------------------------------------------------------------+\r\n");
	rt_kprintf("user_app_conf.PRESENCE_UPLOAD_PREIOD %d \r\n",user_app_conf.PRESENCE_UPLOAD_PREIOD);
	rt_kprintf("user_app_conf.PRESENCE_BC_COUNT %d \r\n",user_app_conf.PRESENCE_BC_COUNT);
	rt_kprintf("user_app_conf.HOST_ADDRESS %s \r\n",user_app_conf.HOST_ADDRESS);
	rt_kprintf("user_app_conf.HOST_PORT %d \r\n",user_app_conf.HOST_PORT);
	rt_kprintf("user_app_conf.ACCOUNT %s \r\n",user_app_conf.ACCOUNT);
	//MOVINDEX
	rt_kprintf("user_app_conf.MOVINDEX %d \r\n",user_app_conf.movcall_index);
	rt_kprintf("user_app_conf.phoneBook0 %s \r\n",user_app_conf.phoneBook[0]);
	rt_kprintf("user_app_conf.phoneBook1 %s \r\n",user_app_conf.phoneBook[1]);
	rt_kprintf("user_app_conf.phoneBook2 %s \r\n",user_app_conf.phoneBook[2]);
	rt_kprintf("user_app_conf.phoneBook3 %s \r\n",user_app_conf.phoneBook[3]);
	rt_kprintf("user_app_conf.MANUALLY_OTA %d \r\n",user_app_conf.MANUALLY_OTA);

	rt_kprintf("user_app_conf.APNINFO.CCID %s \r\n",user_app_conf.APNINFO.CCID);
	rt_kprintf("user_app_conf.APNINFO.APN %s \r\n",user_app_conf.APNINFO.APN);
	rt_kprintf("user_app_conf.APNINFO.USERNAME %s \r\n",user_app_conf.APNINFO.USERNAME);
	rt_kprintf("user_app_conf.APNINFO.PASSWORD %s \r\n",user_app_conf.APNINFO.PASSWORD);
	rt_kprintf("+--------------------------------------------------------------+\r\n");
}




void bkp_fw(void)
{
	//check
}