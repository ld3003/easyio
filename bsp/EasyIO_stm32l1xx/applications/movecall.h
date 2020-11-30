#ifndef __move_call__
#define __move_call__

enum {
	CALLROUTING_STATUS_INIT,
	CALLROUTING_STATUS_CALLING,
	CALLROUTING_STATUS_SOSCALLING,	//����SOS��ť���µ�ʱ��Żᴥ��
	CALLROUTING_STATUS_WATING,
	CALLROUTING_STATUS_WATING2, 		//�����һֱ�ȴ���һֱ������Ϊֹ
	CALLROUTING_STATUS_TIMEOUT,			//��MOVECALL��һ��ʱ���ڣ������ټ�⵽�˶�Ҳ������������ж�
};
extern unsigned char callrouting_status;
void call_routing(void);

#define START_SOS_CALL if ((callrouting_status == CALLROUTING_STATUS_INIT)||(callrouting_status == CALLROUTING_STATUS_TIMEOUT)){callrouting_status = CALLROUTING_STATUS_SOSCALLING;}

//callrouting_status = CALLROUTING_STATUS_INIT

#endif