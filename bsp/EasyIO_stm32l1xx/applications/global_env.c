#include "global_env.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "MMA845X.h"
#include "gnss.h"
#include "my_stdc_func/debugl.h"

POWER_MOD global_powermod = NORMAL_POWERMOD;
char global_boadcast_gps = -1;
GPS_POWER gloabl_gpspower = GPS_POWER_AUTO;
BAT_STATUS global_bat_status = BAT_FULL;							//default BAT_FULL

//struct GPS_INFO global_gps_info = {0,0,0,0,0,0,0};
struct USER_CONFIG global_user_config = {
	15,
	0,
};

unsigned int global_falling_interrupt_time = 0;
unsigned int global_rising_interrupt_time = 0;

int global_reboot_time = -1;

