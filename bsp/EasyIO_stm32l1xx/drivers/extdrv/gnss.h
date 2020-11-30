#ifndef __gnss_h__
#define __gnss_h__

#include "nmea/parse.h"

struct GPS_INFO {
	
	//real data 实时的解析数据:
	nmeaGPRMC gprmc;unsigned char parse_gprmc_succ;
	nmeaGPGSA gpgsa;
	nmeaGPGSV gpgsv[8];
	int real_max_gps_sig; //最大的GPS信号值
	
	
	//history data:
	//最近一次解析成功的数据
	float LAT;
	float LON;
	float google_LAT;
	float google_LON;
	float	SPEED;
	float DIS;
	nmeaGPRMC his_gprmc;
	nmeaGPGSA his_gpgsa;
	nmeaGPGSV his_gpgsv[8];
	
	unsigned char gpgsv_count;
	int satellite_cnt;
	unsigned char valid;
	
	unsigned char valid_sat_count; //有效星数
	unsigned char in_use_sat_count;//正在使用的星数
	unsigned char available_sat_count; //可用的卫星数量
	

};


extern struct GPS_INFO gps_info;
#define BUFFER_LEN 1024+512
extern unsigned char gps_buffer_tmp[BUFFER_LEN];
extern unsigned int gps_buffer_tmp_length;

extern unsigned char gps_raw_data[BUFFER_LEN];

void init_gnss(void);
void parser_gps_rawdata(unsigned char *xbuffer , unsigned int size);
int get_gps_line(const char *in , char *out , unsigned int size , char *buffer);




#endif