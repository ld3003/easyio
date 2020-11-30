#ifndef __LOCAL_CONF__
#define __LOCAL_CONF__


//用户自定义
#define MODEM_IMEI 								modem_imei
//#define MODEM_IMEI 								"123456789012301"		//深圳测试用
//#define MODEM_IMEI 								"123456789012303"
//#define NETWORK_APN_NAME					"cmnet"
#define NETWORK_PPP_USERNAME			""
#define NETWORK_PPP_PASSWORD			""


//#define XMPP_SERVER_HOSTNAME			"easy-io.net"
//#define XMPP_SERVER_PORT 					5222
//#define XMPP_USERNAME						"test"
//#define XMPP_PASSWORD							"test"


#define XMPP_SERVER_HOSTNAME			tracker_private_data.conf->HOST_ADDRESS		//"jareymobile.com.tw"
#define XMPP_SERVER_PORT 					tracker_private_data.conf->HOST_PORT			//20030
#define XMPP_USERNAME 						xmpp_uname
#define XMPP_PASSWORD							password

#define DEFAULT_HOSTNAE						"jareymobile.com.tw"
#define DEFAULT_HOSTPORT					20030
#define DEFAULT_ACCOUNT           ""
#define DEFAULT_UPLOAD_PREIOD 600

#define PRESENCE_UPDATE_PREIOD		tracker_private_data.conf->PRESENCE_UPLOAD_PREIOD			//10

enum {
	DEBUG_LEVEL_NONE = -1,
	DEBUG_LEVEL_DEBUG = 0,
	DEBUG_LEVEL_INFO = 1,
	DEBUG_LEVEL_WARN = 2,
	DEBUG_LEVEL_ERROR = 3,
	DEBUG_LEVEL_FATAL = 4
};
#define DEBUG_LEVEL DEBUG_LEVEL_DEBUG

//下面一组是配置参数

//#define DEVICE_MOTION							//设备永远移动状态
//#define DISABLE_DIS_FILTER				//启动移动距离过滤
////#define DISABLE_POWERSAVING_3G	//关闭3g休眠
////#define DISABLE_LED							//关闭LED
////#define ENABLE_SERIAL_DEBUG_AT  //开启用串口1进行at调试

//#define CONF_GPS_WORK_TYPE 2						//0 永远关闭 1，永远开启，2，休眠模式
//#define GPS_DATA_TEST						//启用gps模拟数据

//#define DEVICE_MOTION							//设备永远移动状态
//#define DISABLE_DIS_FILTER				//启动移动距离过滤
//#define DISABLE_POWERSAVING_3G	//关闭3g休眠
//#define DISABLE_LED							//关闭LED
//#define ENABLE_SERIAL_DEBUG_AT  //开启用串口1进行at调试

#define CONF_GPS_WORK_TYPE 2						//0 永远关闭 1，永远开启，2，休眠模式
//#define GPS_DATA_TEST						//启用gps模拟数据

#endif