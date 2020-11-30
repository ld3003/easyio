#ifndef __my_stdc_func_h__
#define __my_stdc_func_h__

#define UBYTE unsigned char
#define CHAR char

extern int str_to_int(char * buf , int len);
extern int ascii_2_hex(UBYTE *O_data, UBYTE *N_data, int len);
extern int hex_2_ascii(UBYTE *data, CHAR *buffer, int len);

int ftoa(char *str, float num, int n);
char* get_parma(int index , char *instr); //���������1��ʼ
int get_parmer(unsigned char *str,unsigned char *name,unsigned char *context,unsigned char chr);
int atohl(char *str,char hex);
int get_str_fragment(const char * in_str , const char *start_str  , const char *end_str , char *out_buf , int buflen);
int get_parma_count_type2(char *str);
int get_parma_count(char *str);
void get_genstr(char *buffer);

//������
int get_parma_count_type2(char *str); //���� arg1:arg2:arg3:arg4 ���͵��ַ���
char* get_parma_type2(int index , char *instr); //���� arg1:arg2:arg3:arg4 ���͵��ַ���

#define SETBIT(x,n) x = ~((~x)&(~(1<<n)))
#define CLEARBIT(x,n) x = x&(~(1<<n))
#define GETBIT(x,n) (x&(1<<n)) >> n

extern void set_4bit_h(unsigned char * _byte , unsigned char val);
extern void set_4bit_l(unsigned char * _byte , unsigned char val);
extern unsigned char get_4bit_h(unsigned char _byte);
extern unsigned char get_4bit_l(unsigned char _byte);

extern void GetHost(char * src, char * web, char * file, int * port);

int is_string(unsigned char * buf , int len);

#define RUN_TEST DEBUGL->debug("__easyio_runtest FUNC:%s LINE:%d \r\n",__FUNCTION__,__LINE__);

void debug_buf(char *name , unsigned char *buf , int len);

extern unsigned short sdk_stream_crc16_calc(const unsigned char* pMsg, unsigned int nLen);

void filter_str(char *str);		//ȥ���ַ����е������ַ���ֻ�����ǺϷ�ascII��

unsigned int conv_version_num(unsigned char version_h , unsigned char version_l);
#endif
