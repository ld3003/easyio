#ifndef __xmpp_msg_queue__
#define __xmpp_msg_queue__


#define TBQ_MAX_LEN 64


#pragma pack(push)
#pragma  pack(1)
struct TRACKER_BUFFER_DATA {
	unsigned char gps_status; //A,V,OFF
	unsigned char bat_status; //
	unsigned char idle_status;
	unsigned char mma845x_status;
	float google_LAT;
	float google_LON;
	float speed;
	char     year;       /**< Years since 1900 */
  char     mon;        /**< Months since January - [0,11] */
  char     day;        /**< Day of the month - [1,31] */
  char     hour;       /**< Hours since midnight - [0,23] */
  char     min;        /**< Minutes after the hour - [0,59] */
  char     sec;        /**< Seconds after the minute - [0,59] */
	//unsigned char att[4];
};
#pragma pack(pop)

//struct TRACKER_BUFFER_ITEM {
//	unsigned char flag;
//	int next_index;
//	struct TRACKER_BUFFER_DATA data;
//};

//struct TRACKER_BUFFER_QU {
//	unsigned char count;	//队列总数
//	int first_index;	//指向队列的首部
//	int last_index;	//指向队列的尾部
//	struct TRACKER_BUFFER_ITEM tb[TBQ_MAX_LEN];
//};


enum {
	TRACKER_BUFFER_QU_STATUS_IDLE,
	TRACKER_BUFFER_QU_STATUS_BUSY,
};

extern unsigned char tracker_buffer_qu_flag;

//void init_tbq(struct TRACKER_BUFFER_QU * tbq);
//int push_tbq(struct TRACKER_BUFFER_QU * tbq , struct TRACKER_BUFFER_ITEM *item);
//const struct TRACKER_BUFFER_ITEM *top_tbq_item(struct TRACKER_BUFFER_QU * tbq);
//const struct TRACKER_BUFFER_ITEM *pop_tbq(struct TRACKER_BUFFER_QU * tbq);

int tracker_item_cmp_BCGPS(struct TRACKER_BUFFER_DATA *a,struct TRACKER_BUFFER_DATA *b);
int tracker_item_cmp_STATUS(struct TRACKER_BUFFER_DATA *a,struct TRACKER_BUFFER_DATA *b);

///////////////////////////////RING BUFFER
#define QueueSize TBQ_MAX_LEN   //???????????
typedef struct TRACKER_BUFFER_DATA Queue_DataType;

typedef struct{
		 int count;
		 int index;
		 int index2;
		 Queue_DataType data[QueueSize];
}CirQueue;

extern CirQueue tracker_buffer_qu;

void InitQueue(CirQueue *Q);
void EnQueue(CirQueue *Q,Queue_DataType x);
Queue_DataType DeQueue(CirQueue *Q);
Queue_DataType *getDeQueue(CirQueue *Q);
Queue_DataType *getDeQueue2(CirQueue *Q);
char QueueIsEmpty(CirQueue *Q);




#endif
