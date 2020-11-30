#ifndef __gps_tracker__
#define __gps_tracker__

#include "spiflash_config.h"
#include "XMPP/xmpp.h"
#include "ota_http_tcp.h"
#include "sim900.h"

struct TRACKER_PRIVATE {
	struct USER_APP_CONFIG *conf;
	struct XMPP_PMA *xmpp_pma;
	
	unsigned char test_flag;
	
	unsigned int gsm_signal;
	unsigned char gps_signal;
	unsigned int bat_val;
	
	unsigned short spiflashID;
	unsigned char bmp180ID;
	unsigned char mma845xTest;
	
	unsigned char ota_status;//��¼��ǰOTA��״̬���Ƿ��и������ݣ��Ƿ���Ҫ���µȵ� , ģʽΪ STATUS_WAIT_CHECK �ȴ���һ��OTA���
	
	unsigned char dbg_motion;
	unsigned char dbg_gps_distance;
	
	unsigned int pre_poweroff_on_time;		//��¼�������İ���ʱ��
	unsigned char pre_poweroff_on;			//Ԥ�����ػ��Ĳ���
	
	unsigned char mma8452_power_on_off;
	unsigned char SYSTEM_STATUS;
	

	
	
};

enum {
	SYSTEM_POWER_ON,
	SYSTEM_POWER_OFF,
	SYSTEM_CONFIG_MOD,
};

extern struct TRACKER_PRIVATE tracker_private_data;

#define INIT_TRACKER_PRIVATE memset(&tracker_private_data,0x0,sizeof(tracker_private_data)); tracker_private_data.conf = &user_app_conf;tracker_private_data.xmpp_pma = &xmpp_pma;tracker_private_data.ota_status = STATUS_WAIT_CHECK;

void init_gps_tracker_timer(void);

#define VERSION_JSON_STR "{\"version_name\":\"V0.8\" , \"version\":41}"

/***
�汾���¼�¼
V0.8:����İ汾
V0.9:
*/

#endif