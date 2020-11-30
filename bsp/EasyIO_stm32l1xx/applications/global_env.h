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
//	unsigned char valid_sat_count; //有效星数
//	unsigned char in_use_sat_count;//正在使用的星数
//	
//	unsigned char result_sat_count;
//	//unsigned char poscont
//	

//};

struct USER_CONFIG {
	unsigned int broadcastgps_time;
	unsigned int broadcastgps_timep; //最后一次更新presen 的时间戳
};

/**
	电源模式
*/
extern POWER_MOD global_powermod;			//defalyt NORMAL_POWERMOD

/**
	电源模式
*/
extern char global_boadcast_gps;			//default -1;

/**
	GPS 供电模式
*/
extern GPS_POWER gloabl_gpspower;			//defalut GPS_POWER_AUTO;

/**
	电池状态
*/
extern BAT_STATUS global_bat_status;	//default BAT_FULL


///**
//	GPS信息解析缓存
//*/
//extern struct GPS_INFO global_gps_info;

/**
	用户参数设定
*/
extern struct USER_CONFIG global_user_config;



/**
	设备重启定时器
*/
extern int global_reboot_time;


/**
	创建presence字符串
*/
void make_presence_status_str(void);

#endif