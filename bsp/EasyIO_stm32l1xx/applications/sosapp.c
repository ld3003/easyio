#include "sosapp.h"
#include "sms.h"
#include "gps_tracker.h"

#include <string.h>

unsigned char sendsms_index = 0;
unsigned char sosrouting_status = SOSROUTING_STATUS_INIT;
void sos_routing(void)
{
	switch(sosrouting_status)
	{
		case SOSROUTING_STATUS_INIT:
			break;
		case SOSROUTING_STATUS_START:
		{
			sendsms_index = 0;
			sosrouting_status = SOSROUTING_STATUS_SENDSMS1;
			//³õÊ¼»¯
			break;
		}
		case SOSROUTING_STATUS_SENDSMS1:
		{
			char *phone_num = tracker_private_data.conf->phoneBook[sendsms_index++];
			if (strlen(phone_num) > 0)
			{
				send_sms_api(phone_num,"SOS");
				//
			}
			
			if (sendsms_index >= 4)
			{
				sosrouting_status = SOSROUTING_STATUS_CALL;
			}
			break;
		}
		
		case SOSROUTING_STATUS_CALL:
			break;
		default:
			break;
		//
	}
}