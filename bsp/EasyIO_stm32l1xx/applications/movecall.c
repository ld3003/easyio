#include "movecall.h"
#include "at_cmd.h"
#include "gsmmux/easyio_cmux.h"
#include "app_timer.h"
#include "spiflash_config.h"
#include "MMA845x.h"

//__is_moving

unsigned char callrouting_status = CALLROUTING_STATUS_INIT;

static unsigned int CALLROUTING_STATUS_CALLING_timer = 0;
static unsigned char moving_id = 0;

void call_routing(void)
{
	//
	int at_cmd_ret_code;
	
	switch(callrouting_status)
	{
		case CALLROUTING_STATUS_INIT:
			if (__is_moving())
			{
				if (moving_id != mma845x_movingid)
				{
					//如果设备移动，且不是最新的移动，则
					callrouting_status = CALLROUTING_STATUS_CALLING;
					//
				}
				//
			}
			break;
		case CALLROUTING_STATUS_SOSCALLING:
			
			if ((user_app_conf.movcall_index>0)&&(user_app_conf.movcall_index<=4))
			{
				char buffer[32];
				snprintf(buffer,32,"ATD%s;\r\n",user_app_conf.phoneBook[user_app_conf.movcall_index-1]);
				CALLROUTING_STATUS_CALLING_timer = app_timer_data.app_timer_second;
				cmux_at_command(2,buffer,AT_AT,300,&at_cmd_ret_code);
				callrouting_status = CALLROUTING_STATUS_WATING2;
				//
			}else{
				callrouting_status = CALLROUTING_STATUS_INIT;
			}
			
			break;
		case CALLROUTING_STATUS_CALLING:
			//开始拨打电话
		
			if ((user_app_conf.movcall_index>0)&&(user_app_conf.movcall_index<=4))
			{
				char buffer[32];
				snprintf(buffer,32,"ATD%s;\r\n",user_app_conf.phoneBook[user_app_conf.movcall_index-1]);
				CALLROUTING_STATUS_CALLING_timer = app_timer_data.app_timer_second;
				cmux_at_command(2,buffer,AT_AT,300,&at_cmd_ret_code);
				callrouting_status = CALLROUTING_STATUS_WATING;
				//
			}else{
				callrouting_status = CALLROUTING_STATUS_INIT;
			}
		

			break;
		case CALLROUTING_STATUS_WATING:
			if ((app_timer_data.app_timer_second - CALLROUTING_STATUS_CALLING_timer) > 15)
			{
				//挂机
				cmux_at_command(2,"ATH\r\n",AT_AT,300,&at_cmd_ret_code);
				cmux_at_command(2,"ATH\r\n",AT_AT,300,&at_cmd_ret_code);
				callrouting_status = CALLROUTING_STATUS_TIMEOUT;
			}
			break;
		case CALLROUTING_STATUS_WATING2:
			if ((app_timer_data.app_timer_second - CALLROUTING_STATUS_CALLING_timer) > 65)
			{
				//挂机
				cmux_at_command(2,"ATH\r\n",AT_AT,300,&at_cmd_ret_code);
				cmux_at_command(2,"ATH\r\n",AT_AT,300,&at_cmd_ret_code);
				callrouting_status = CALLROUTING_STATUS_TIMEOUT;
			}
			break;
		case CALLROUTING_STATUS_TIMEOUT:
			//超时了
			if ((app_timer_data.app_timer_second - CALLROUTING_STATUS_CALLING_timer) > 180)
			{
				CALLROUTING_STATUS_CALLING_timer = 0;
				callrouting_status = CALLROUTING_STATUS_INIT;
			}
			break;
		default:
			break;
	}
}