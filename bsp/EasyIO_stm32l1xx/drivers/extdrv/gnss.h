#ifndef __gnss_h__
#define __gnss_h__

#include "nmea/parse.h"

struct GPS_INFO {
	
	//real data ʵʱ�Ľ�������:
	nmeaGPRMC gprmc;unsigned char parse_gprmc_succ;
	nmeaGPGSA gpgsa;
	nmeaGPGSV gpgsv[8];
	int real_max_gps_sig; //����GPS�ź�ֵ
	
	
	//history data:
	//���һ�ν����ɹ�������
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
	
	unsigned char valid_sat_count; //��Ч����
	unsigned char in_use_sat_count;//����ʹ�õ�����
	unsigned char available_sat_count; //���õ���������
	

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