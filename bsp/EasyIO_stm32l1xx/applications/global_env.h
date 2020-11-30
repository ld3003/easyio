#ifndef __global_env__
#define __global_env__

#include "nmea/parse.h"


typedef enum {
	IMME_POWERMOD,
	LOW_POWERMOD,
	NORMAL_POWERMOD
}POWER_MOD;

typedef enum {
	GPS_POWER_OFF,
	GPS_POWER_ON,
	GPS_POWER_AUTO,
}GPS_POWER;


typedef enum {
	DEV_MOVING,
	DEV_STAY,
}GPS_MOV_STATUS;

typedef enum {
	GPS_STATUS_A,
	GPS_STATUS_V,
}GPS_LOC_STATUS;

typedef enum {
	BAT_AC,
	BAT_FULL,
	BAT_FULL_0,
	BAT_FULL_1,
	BAT_FULL_2,
	BAT_FULL_3,
	BAT_FULL_4,
	BAT_FULL_5,
	
}BAT_STATUS;

//struct GPS_INFO {
//	float google_LAT;
//	float google_LON;
//	float LAT;
//	float LON;
//	
//	float	SPEED;
//	
//	int satellite_cnt;
//	
//	unsigned char valid;
//	
//	nmeaGPRMC gprmc;
//	nmeaGPGSA gpgsa;
//	nmeaGPGSV gpgsv[8];
//	unsigned char gpgsv_count;
//	
//	unsigned char valid_sat_count; //��Ч����
//	unsigned char in_use_sat_count;//����ʹ�õ�����
//	
//	unsigned char result_sat_count;
//	//unsigned char poscont
//	

//};

struct USER_CONFIG {
	unsigned int broadcastgps_time;
	unsigned int broadcastgps_timep; //���һ�θ���presen ��ʱ���
};

/**
	��Դģʽ
*/
extern POWER_MOD global_powermod;			//defalyt NORMAL_POWERMOD

/**
	��Դģʽ
*/
extern char global_boadcast_gps;			//default -1;

/**
	GPS ����ģʽ
*/
extern GPS_POWER gloabl_gpspower;			//defalut GPS_POWER_AUTO;

/**
	���״̬
*/
extern BAT_STATUS global_bat_status;	//default BAT_FULL


///**
//	GPS��Ϣ��������
//*/
//extern struct GPS_INFO global_gps_info;

/**
	�û������趨
*/
extern struct USER_CONFIG global_user_config;



/**
	�豸������ʱ��
*/
extern int global_reboot_time;


/**
	����presence�ַ���
*/
void make_presence_status_str(void);

#endif