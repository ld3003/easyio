/////////////////////////////////////////////////
#include "xmpp_msg_queue.h"
#define xDEBUG_ON_LINUX

#ifdef DEBUG_ON_LINUX
#else
#define printf rt_kprintf
#endif

//struct TRACKER_BUFFER_QU tracker_buffer_qu;
unsigned char tracker_buffer_qu_flag = TRACKER_BUFFER_QU_STATUS_IDLE;

//void init_tbq(struct TRACKER_BUFFER_QU * tbq)
//{
//	int i =0;
//	memset(&tracker_buffer_qu,0x0,sizeof(tracker_buffer_qu));
//	for(i=0;i<TBQ_MAX_LEN;i++)
//	{
//		tbq->tb[i].next_index = -1;
//		//
//	}
//	
//	tbq->count = 0;
//	tbq->first_index = -1;
//	tbq->last_index = -1;
//}

//int push_tbq(struct TRACKER_BUFFER_QU * tbq , struct TRACKER_BUFFER_ITEM *item)
//{
//	//
//	int i=0;
//	int ret = -1;
//	for(i=0;i<TBQ_MAX_LEN;i++)
//	{
//		if (tbq->tb[i].flag == 0)
//		{
//			memcpy(&tbq->tb[i],item,sizeof(struct TRACKER_BUFFER_ITEM));
//			
//			tbq->tb[i].flag = 1;
//			tbq->tb[i].next_index = -1;
//			
//			if (tbq->last_index>=0)
//				tbq->tb[tbq->last_index].next_index = i;
//			
//			tbq->last_index = i;
//			
//			if (tbq->first_index < 0)
//			{
//				tbq->first_index = i;
//			}
//			
//			
//			ret = 0;
//			break;
//		}
//		//
//	}
//	return ret;
//	
//}

//const struct TRACKER_BUFFER_ITEM *top_tbq_item(struct TRACKER_BUFFER_QU * tbq)
//{
//	struct TRACKER_BUFFER_ITEM *item;
//	if (tbq->first_index < 0)
//		return 0;
//	return &tbq->tb[tbq->first_index];
//}

//const struct TRACKER_BUFFER_ITEM *pop_tbq(struct TRACKER_BUFFER_QU * tbq)
//{
//	struct TRACKER_BUFFER_ITEM *item;
//	if (tbq->first_index < 0)
//		return 0;
//	item = &tbq->tb[tbq->first_index];
//	item->flag = 0;
//	tbq->first_index = item->next_index;

//	if (tbq->first_index < 0)
//	{
//		tbq->last_index = -1;
//	}

//	return item;
//	//
//}

//只效验GPS坐标信息
int tracker_item_cmp_BCGPS(struct TRACKER_BUFFER_DATA *a,struct TRACKER_BUFFER_DATA *b)
{
	
//	if (a->data.gps_status != b->data.gps_status)
//		return -1;
//	if (a->data.bat_status != b->data.bat_status)
//		return -1;
	if (a->google_LAT != b->google_LAT)
		return -1;
	if (a->google_LON != b->google_LON)
		return -1;
//	if (a->data.mma845x_status != b->data.mma845x_status)
//		return -1;
	if (a->idle_status != b->idle_status)
		return -1;
	if (a->speed != b->speed)
		return -1;
	
	return 0;
}

//只检查 gps 开关，电池状态，运动状态
int tracker_item_cmp_STATUS(struct TRACKER_BUFFER_DATA *a,struct TRACKER_BUFFER_DATA *b)
{
	
	if (a->gps_status != b->gps_status)							//gps开关
		return -1;
	if (a->bat_status != b->bat_status)							//电池电量
		return -1;
	if (a->mma845x_status != b->mma845x_status)			//运动状态
		return -1;
	
	return 0;
}

#ifdef DEBUG_ON_LINUX
void main(void)
#else
void xmain(void)
#endif
{
}

/////////////////////////////////////////////////
CirQueue tracker_buffer_qu;

void InitQueue(CirQueue *Q)
{
		memset(&tracker_buffer_qu,0x0,sizeof(CirQueue));
		Q->index = 0;
		Q->index2 = 0;
    Q->count = 0;
}
void EnQueue(CirQueue *Q,Queue_DataType x)
{
	if (Q->count >= QueueSize)
		return;
	Q->count ++;
	Q->data[Q->index] = x;
	Q->index=(Q->index+1)%QueueSize;      //??????????1
}
Queue_DataType DeQueue(CirQueue *Q)
{
	Queue_DataType tmp;
	if (Q->count <= 0)
		return tmp;
	
	
	tmp = Q->data[Q->index2];
	Q->index2=(Q->index2+1)%QueueSize;
	Q->count --;
	return tmp;
	//
}

Queue_DataType *getDeQueue(CirQueue *Q)
{
	Queue_DataType *tmp;
	if (Q->count <= 0)
		return 0;
	
	tmp = &(Q->data[Q->index2]);
	
	Q->index2=(Q->index2+1)%QueueSize;
	Q->count --;
	return tmp;
}

Queue_DataType *getDeQueue2(CirQueue *Q)
{
	Queue_DataType *tmp;
	if (Q->count <= 0)
		return 0;
	
	tmp = &(Q->data[Q->index2]);
	return tmp;
}

char QueueIsEmpty(CirQueue *Q)
{
	if (Q->count <= 0)
		return 1;
	return 0;
	//
}
