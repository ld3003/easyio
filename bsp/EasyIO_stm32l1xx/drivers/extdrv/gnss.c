#include <rtthread.h>

#include <string.h>
#include <stdio.h>

#include <math.h>

//#include "global_env.h"


#include "nmea/nmea.h"
#include "UBLOX_GPS.h"
#include "MMA845X.h"
#include "gnss.h"
#include "led.h"

#include "lowpower.h"
#include "my_stdc_func/debugl.h"
//#define rt_kprintf(...)

struct GPS_INFO gps_info;
#define global_gps_info gps_info


const char gps_valid_data[] = "\
$GNRMC,041142.00,A,3907.80639,N,11704.58908,E,0.144,,040915,,,A*62\r\n\
$GNVTG,,T,,M,0.144,N,0.268,K,A*30\r\n\
$GNGGA,041142.00,3907.80639,N,11704.58908,E,1,08,2.57,1.7,M,-6.0,M,,*53\r\n\
$GNGSA,A,3,17,28,01,04,32,11,,,,,,,3.81,2.57,2.81*15\r\n\
$GNGSA,A,3,69,75,,,,,,,,,,,3.81,2.57,2.81*10\r\n\
$GPGSV,3,1,10,01,52,047,33,03,41,119,,04,24,050,27,08,11,088,*7A\r\n\
$GPGSV,3,2,10,11,40,057,31,17,44,297,27,19,18,081,,28,81,296,30*77\r\n\
$GPGSV,3,3,10,30,23,212,,32,30,071,20*7D\r\n\
$GLGSV,2,1,08,68,09,024,,69,28,073,25,70,18,131,,74,27,195,20*6D\r\n\
$GLGSV,2,2,08,75,54,263,24,76,23,326,,84,35,046,20,85,57,331,*6C\r\n\
$GNGLL,3907.80639,N,11704.58908,E,041142.00,A,A*73\r\n\
";



unsigned char gps_buffer_tmp[BUFFER_LEN] = {0x0};
unsigned char gps_raw_data[BUFFER_LEN] = {0x0};

unsigned int gps_buffer_tmp_length = 0;
static float gps2m (float lat_a, float lng_a, float lat_b, float lng_b);
static int gprmcToGoogleMap( float lat, float lon , float *out_lat , float *out_lon);

/**
Example:
$GPRMC,082211.00,A,3905.80078,N,11704.92993,E,0.001,,120914,,,A*75
$GPVTG,,T,,M,0.001,N,0.002,K,A*20
$GPGGA,082211.00,3905.80078,N,11704.92993,E,1,05,3.71,47.1,M,-5.9,M,,*74
$GPGSA,A,3,23,02,06,13,10,,,,,,,,5.47,3.71,4.02*01
$GPGSV,2,1,08,02,56,305,43,06,64,039,45,09,35,080,41,10,87,168,35*78
$GPGSV,2,2,08,13,30,076,45,17,33,138,,23,20,050,36,25,17,312,*7E
$GPGLL,3905.80078,N,11704.92993,E,082211.00,A,A*62
*/

/**
字符串解析函数 ，用于提取GPS字段信息
*/

int get_gps_line(const char *in , char *out , unsigned int outsize , char *buffer)
{
	//
	char *tmp,*tmp2;
	int i=0,j=0,m,n;

	tmp = strstr(buffer,in);
	if (tmp > 0)
	{
		m = strlen(tmp);
		for(i=0;i<m;i++)
		{
			if (tmp[i] == '\n')
			{
				//判断后面是否还有字符
				tmp2 = strstr(&tmp[i],in);
				if (tmp2 == 0)
				{
					goto xxout;
				}
			}
			out[i] = tmp[i];
			if (i >= (outsize-1))
			{goto xxout;}
		}
	}else
	{
	}

	xxout:
	out[i] = 0x0;
	return i;
}
/**
将GPS坐标转换为Google坐标
*/
static int gprmcToGoogleMap( float lat, float lon , float *out_lat , float *out_lon)
{

	int dd;
	float mm;
	
	dd = (int) (lat/100);
	mm = (float) (lat-dd*100);
	lat= dd + (mm/60);
	
	dd = (int) (lon/100);
	mm = (float) (lon-dd*100);
	lon= dd + (mm/60);

	*out_lat = lat;//(int)(lat*1E6);
	*out_lon = lon;//(int)(lon*1E6);
	
	return 0;
}

/**
解析NEMA_GPGSV字符串，并做缓存处理
*/
#include "gps_tracker.h"
static int handler_gpsdata_GPGSV(char *line_str)
{
	unsigned int i=0,j=0;
	int parse_ret = 0;
	unsigned int valid_sat_count = 0;
	nmeaGPGSV gpgsv;
	parse_ret = nmea_parse_GPGSV(line_str,strlen(line_str),&gpgsv);
	
	
	if (parse_ret > 0)
	{
		if (gpgsv.pack_count <= 8)
		{
			memcpy(&global_gps_info.gpgsv[gpgsv.pack_index],&gpgsv,sizeof(nmeaGPGSV));
		}
		
		global_gps_info.gpgsv_count = gpgsv.pack_count;
		
		//获取有效星数，并保存
		for(i=0;i<gpgsv.pack_count;i++)
		{
			for(j=0;j<NMEA_SATINPACK;j++)
			{
				if (global_gps_info.gpgsv[i].sat_data[j].id > 0)
				{
					//判断DB大于 10的为有效值
					if (global_gps_info.gpgsv[i].sat_data[j].sig > 10)
					{
						valid_sat_count ++;
						//
					}
					
					//最大的数据
					if (gps_info.real_max_gps_sig < global_gps_info.gpgsv[i].sat_data[j].sig)
					{
						gps_info.real_max_gps_sig = global_gps_info.gpgsv[i].sat_data[j].sig;
					}
					
					tracker_private_data.gps_signal = gps_info.real_max_gps_sig;
					//
				}
				//
			}
			//
		}
		
		global_gps_info.valid_sat_count = valid_sat_count;
		//DEBUGL->debug("GPGSV visual satellite pkg index %d sat cnt:%d\r\n",gpgsv.pack_index,gpgsv.sat_count);
	}else {
		//DEBUGL->debug("Parser GSV ERROR \r\n");
	}
	//nmeaGPGSV
	
	return parse_ret;
	
}

/**
解析NEMA_GPGSA字符串，并做缓存处理
*/
static int handler_gpsdata_GPGSA(char *line_str)
{
	int parse_ret;
	unsigned int i =0;
	unsigned int count = 0;
	nmeaGPGSA gpgsa;
	parse_ret = nmea_parse_GPGSA(line_str,strlen(line_str),&gpgsa);
	
	if (parse_ret > 0)
	{
		memcpy(&global_gps_info.gpgsa,&gpgsa,sizeof(nmeaGPGSA));
		
		////DEBUGL->debug("GPGSAINFO:\r\n");
		for(i=0 ; i<12 ; i++)
		{
			////DEBUGL->debug("INVILED SAT NUM:%d\r\n",gpgsa.sat_prn[i]);
			if (gpgsa.sat_prn[i] > 0)
				count ++;
		}
		
		global_gps_info.in_use_sat_count = count;
		
	
	}
	
	return parse_ret;
	
	
}

/**
获取有效定位卫星数
*/

static unsigned char get_avai_sat_count(void)
{
	unsigned char ret;
	if (global_gps_info.in_use_sat_count >= 6)
	{
		ret = 0;
		//
	} else {
		if (global_gps_info.valid_sat_count < global_gps_info.in_use_sat_count)
		{
			ret = 0;
		}else {
			ret =  global_gps_info.valid_sat_count - global_gps_info.in_use_sat_count;
		}
	}
	
	global_gps_info.available_sat_count = ret;
	
	return ret;
}

static int handler_gpsdata_GPRMC(char *line_str)
{
	int parse_ret;
	char *tmp;
	//static char tmp_str[128];
	float speed , lat,lon,google_lat,google_lon;
	float gps_dis;
	
	//nmeaGPRMC gprmc;
	parse_ret = nmea_parse_GPRMC(line_str,strlen(line_str),&global_gps_info.gprmc);
	
	//经纬度坐标转换
	if (global_gps_info.gprmc.ew == 'W')
	{
		global_gps_info.gprmc.lon *= -1;
	}
	
	if (global_gps_info.gprmc.ns == 'S')
	{
		global_gps_info.gprmc.lat *= -1;
	}
	
	//因为模拟数据都是固定的，所以每次都要加上一些偏移
	#ifdef GPS_DATA_TEST
//	{
//		static float ____tmp = 0.0001;
//		____tmp += 0.0001;
//		global_gps_info.gprmc.lon += ____tmp;
//	}
	#endif
	
	
	return parse_ret;
	
}

static int handler_gpsdata_GNRMC(char *line_str)
{
	int parse_ret;
	char *tmp;
	//static char tmp_str[128];
	float speed , lat,lon,google_lat,google_lon;
	float gps_dis;
	
	//nmeaGPRMC gprmc;
	parse_ret = nmea_parse_GNRMC(line_str,strlen(line_str),&global_gps_info.gprmc);
	
	////DEBUGL->debug("%s",line_str);
	//snprintf(tmp_str,sizeof(tmp_str),"DEBUG GPRMC %f,%f\r\n",gps_info.google_LAT,gps_info.google_LON);
	////DEBUGL->debug("%s",tmp_str);
	
	return parse_ret;
	
}

//nmea_parse_GNRMC

static void update_gps_info(void)
{
	
	static char dbug_presence[128];
	char *tmp;char tmp_str[16];
	float speed , lat,lon,google_lat,google_lon,his_google_lat,his_google_lon;
	float gps_dis;
	
	//判断定位是否成功
	if (global_gps_info.gprmc.status == 'A')
	{

		speed = global_gps_info.gprmc.speed * 1.852;
		//global_gps_info.gprmc.ns
		//global_gps_info.gprmc.heig
		//转换为 GOOGLE 坐标
		gprmcToGoogleMap(global_gps_info.gprmc.lat,global_gps_info.gprmc.lon,&google_lat,&google_lon);
		gprmcToGoogleMap(global_gps_info.his_gprmc.lat,global_gps_info.his_gprmc.lon,&his_google_lat,&his_google_lon);
		
		#if 0
		 if ( ew_indicator == "W" ) then
      longitude =  longitude * -1
    end
    if ( ns_indicator == "S" ) then
      latitude = latitude * -1
    end
		#endif
		

//		//如果是第一次收到有效信息,
//		if ((global_gps_info.LAT == 0) || (global_gps_info.LON == 0))
//		{
//			//更新GPS全局数据
//			global_gps_info.LAT = lat;
//			global_gps_info.LON = lon;
//			global_gps_info.google_LAT = google_lat;
//			global_gps_info.google_LON = google_lon;
//			global_gps_info.SPEED = speed;
//			
//			//保存最后一次定位成功的数据
//			memcpy(&global_gps_info.his_gprmc,&global_gps_info.gprmc,sizeof(nmeaGPRMC));
//			
//		}
		
		//判断是否移动
		if (__is_moving() > 0)
		{
			
			//判断GPS是否移动超过10米,历史距离
			gps_dis = gps2m(google_lat,google_lon,global_gps_info.google_LAT,global_gps_info.google_LON);
			
			global_gps_info.DIS = gps_dis;
			
			//当移动距离真的大于10米，或者关闭移动距离大于10米判定标记为为 1 时
			#ifdef DISABLE_DIS_FILTER
			if (1)
			#else
			if (fabs(gps_dis) > 5)
			#endif
			{
				//google坐标并存储在 gps info 中
				
				global_gps_info.google_LAT = google_lat;
				global_gps_info.google_LON = google_lon;
				global_gps_info.SPEED = speed;
				
				

				
				
			}else
			{
				
//				snprintf(dbug_presence,sizeof(dbug_presence),"DBG [%f] [%f %f]  [%f %f]",fabs(gps_dis),global_gps_info.google_LAT,global_gps_info.google_LON,google_lat,google_lon);
//				DEBUGL->debug("CHN PinYin : yi dong ju li xiao yu  10m %d %f\r\n",tracker_private_data.dbg_gps_distance,(int)fabs(gps_dis));
//				xmpp_send_msg("liud@cts",dbug_presence);
				//xmpp_set_presence("online",dbug_presence);
			}
			
			//保存最后一次定位成功的数据
			memcpy(&global_gps_info.his_gprmc,&global_gps_info.gprmc,sizeof(nmeaGPRMC));
			

		}
		
	}
	
//	snprintf(dbug_presence,sizeof(dbug_presence),"DBG [%f] [%f %f]  [%f %f]",fabs(gps_dis),global_gps_info.google_LAT,global_gps_info.google_LON,google_lat,google_lon);
//	DEBUGL->debug(dbug_presence);
//	xmpp_send_msg("liud@cts",dbug_presence);
	
}

//解析GPS 字符串
static void handler_gpsdata(char *buffer , unsigned int buffer_len)
{
	char *tmpdata = rt_malloc(128);
	char *tmpdata2;
	unsigned char tmpdata_index = 0;
	unsigned int i=0;
	//判断动态内存是否分配成功
	if (tmpdata > 0)
	{
		
		tmpdata2 = rt_malloc(buffer_len);
		if (tmpdata2 <=0)
		{
			rt_free(tmpdata);
			return ;
		}
		
		snprintf(tmpdata2,buffer_len,"");
		
		//判断是否到缓冲结束
		while(buffer[i] != 0x00)
		{
			if (i>=buffer_len)
				break;
			
			if (buffer[i] > 127)
				break;
				
			//判断是否到行末
			if ((buffer[i] == '\r') || (buffer[i] == '\n'))
			{
				//判断是否为空行
				if (tmpdata_index == 0)
				{//ignore
				}
				else
				{
					//添加行结束位
					tmpdata[tmpdata_index++] = 0x0;
					//DEBUGL->debug("GPS_LINE: %s \r\n",tmpdata);
					if(tmpdata_index > 10)
					{
						if ((tmpdata[4] == 'S')&&(tmpdata[5] == 'A'))//GPGSA
						{
							tmpdata[2] = 'P';
							if (handler_gpsdata_GPGSA(tmpdata) > 0)
								strcat(tmpdata2,tmpdata);
							//
						}else
						if ((tmpdata[4] == 'M')&&(tmpdata[5] == 'C'))//GPRMC
						{
							tmpdata[2] = 'P';
							if (handler_gpsdata_GPRMC(tmpdata) > 0)
							{
								gps_info.parse_gprmc_succ = 1;
								strcat(tmpdata2,tmpdata);
							}
							//
						}else
						if ((tmpdata[4] == 'S')&&(tmpdata[5] == 'V'))//GPGSV
						{
							if (handler_gpsdata_GPGSV(tmpdata) > 0)
								strcat(tmpdata2,tmpdata);
							//
						}else
						//if ((tmpdata[4] == 'S')&&(tmpdata[5] == 'A'))//GPGSA
						{
							//handler_gpsdata_GPGSA(tmpdata);
						}
					}
					
					//清楚一行数据
					tmpdata_index = 0;
					//
				}
				
			}else
			{
				tmpdata[tmpdata_index++] = buffer[i];
			}
			i++;
		}
		

		//释放内存
		rt_free(tmpdata2);
		rt_free(tmpdata);
	}
	

	
}

/**
计算GPS位移
*/
#include <math.h>
static float gps2m (float lat_a, float lng_a, float lat_b, float lng_b)
{
	#define PI 3.1415926
	#define EARTH_RADIUS 6378137.0
	float radLat1 = (lat_a * PI / 180.0);
	float radLat2 = (lat_b * PI / 180.0);
	float a = radLat1 - radLat2;
	float b = (lng_a - lng_b) * PI / 180.0;
	float s = 2 * asin(sqrt(pow(sin(a / 2), 2)
	+ cos(radLat1) * cos(radLat2)
	* pow(sin(b / 2), 2)));
	s = s * EARTH_RADIUS;
	//s = round(s * 10000) / 10000;
	return s;
	//
}

/**
GPS 串口 调用接口
*/
void parser_gps_rawdata(unsigned char *xbuffer , unsigned int size)
{
	static unsigned char cnt = 1;
	
	cnt ++;
	if (cnt >= 2)
	{
		//解析GPS數據
		handler_gpsdata((char*)xbuffer,size);	//解析数据
		//更新GPS信息
		update_gps_info();
		//计算可用卫星数
		get_avai_sat_count();
		
		cnt = 0;
	}
}


/**
GPS信息测试接口
*/
void test_gps_buffer(void)
{
	parser_gps_rawdata((unsigned char*)gps_valid_data,sizeof(gps_valid_data));
	//
}


void init_gnss(void)
{
	memset(&gps_info,0x0,sizeof(gps_info));
	gps_info.DIS = -1;
}
