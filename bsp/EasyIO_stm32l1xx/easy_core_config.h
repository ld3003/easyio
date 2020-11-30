#ifndef __easy_core_config__
#define __easy_core_config__

//定时器个数及定义
enum {
	UPDATE_PRESENCE_TIMER,
	HTTP_TEMPLATE_TIMR,
	EI_TIMER_COUNT,
};

//TCP 类型定义
enum {
	XMPP_TCP_CLIENT = 0,
	HTTP_TCP_CLIENT,
	TEST_TCP_CLIENT,
	SERIAL_TCP_CLIENT,
	TCP_CLI_COUNT
};



#include "app_local_conf.h"




#endif