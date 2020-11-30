#ifndef __move_call__
#define __move_call__

enum {
	CALLROUTING_STATUS_INIT,
	CALLROUTING_STATUS_CALLING,
	CALLROUTING_STATUS_SOSCALLING,	//会在SOS按钮按下的时候才会触发
	CALLROUTING_STATUS_WATING,
	CALLROUTING_STATUS_WATING2, 		//这里会一直等待，一直到结束为止
	CALLROUTING_STATUS_TIMEOUT,			//当MOVECALL后一段时间内，即便再检测到运动也不会对其作出判断
};
extern unsigned char callrouting_status;
void call_routing(void);

#define START_SOS_CALL if ((callrouting_status == CALLROUTING_STATUS_INIT)||(callrouting_status == CALLROUTING_STATUS_TIMEOUT)){callrouting_status = CALLROUTING_STATUS_SOSCALLING;}

//callrouting_status = CALLROUTING_STATUS_INIT

#endif