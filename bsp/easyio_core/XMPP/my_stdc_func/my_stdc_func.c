#include "my_stdc_func.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
int str_to_int(char * buf , int len)
{
	char xbuf[10]; int ret = -1;
	memset(xbuf,0x0,sizeof(buf));
	memcpy(xbuf,buf,len);
	sscanf(xbuf,"%d",&ret);
	return ret;

	//
}


int ascii_2_hex(UBYTE *O_data, UBYTE *N_data, int len) 
{ 
	int i,j,tmp_len;
	UBYTE tmpData; 
	UBYTE *O_buf = O_data;
	UBYTE *N_buf = N_data;
	for(i = 0; i < len; i++)
	{
		if ((O_buf[i] >= '0') && (O_buf[i] <= '9')) 
		{ 
			tmpData = O_buf[i] - '0'; 
		} 
		else  if ((O_buf[i] >= 'A') && (O_buf[i] <= 'F'))  //A....F 
		{ 
			tmpData = O_buf[i] - 0x37; 
		} 
		else if((O_buf[i] >= 'a') && (O_buf[i] <= 'f'))  //a....f 
		{ 
			tmpData = O_buf[i] - 0x57; 
		} 
		else 
		{ 
			return -1; 
		} 
		O_buf[i] =  tmpData; 
	} 
	for(tmp_len = 0,j = 0; j < i; j+=2) 
	{ 
		N_buf[tmp_len++] = (O_buf[j]<<4) | O_buf[j+1]; 
	} 
	return tmp_len;  
}

int hex_2_ascii(UBYTE *data, CHAR *buffer, int len) 
{ 
    const CHAR  ascTable[17] = {"0123456789ABCDEF"}; 
    CHAR *tmp_p = buffer; 
    int i, pos; 
    pos = 0; 
    for(i = 0; i < len; i++) 
    { 
         tmp_p[pos++] = ascTable[data[i] >> 4]; 
         tmp_p[pos++] = ascTable[data[i] & 0x0f]; 
 	} 
    tmp_p[pos] = '\0'; 
    return pos;               
}


int get_parmer(unsigned char *str,unsigned char *name,unsigned char *context,unsigned char chr)
{
	int cnt=0;
	unsigned char *parm;
	while(*context){
		if((*(context-1)!=chr)&&(*(context-1)!='?')){
			context++;
			continue;
		}
		if((memcmp(context,name,strlen((const char *)name))==0)&&
			(*(context+strlen((const char *)name))=='=')){//名字匹配而且以‘=’开始参数
			parm=context+strlen((const char *)name)+1;//"name=
			context+=(strlen((const char *)name)+1);
			while((*context!=chr)&&(*context!='\0')){//&标示参数结束
				cnt++;
				context++;
			}
			memcpy(str,parm,cnt);//copy parm to str
			*(str+cnt)='\0';
			return 0;
		}
		else
			context++;	
	}
	return -1;//cann't find the parm
}

int atohl(char *str,char hex)
{
        char bit_cnt=0;
        int index=1;
        int num=0;
        char chr;
        while(*str++){
                bit_cnt++;              
        }
        str-=2;
        while(bit_cnt--){
                if(!hex){
                        chr=*str--;
                        if((chr>'0'-1)&&(chr<'9'+1)){
                                num+=(chr-'0')*index;
                                index*=10;
                        }
                        else
                                break;
                }
                else{
                        chr=*str--;
                        if((chr>'0'-1)&&(chr<'9'+1)){
                                num+=(chr-'0')*index;
                                index*=16;
                        }
                        else if((chr>'a'-1)&&(chr<'f'+1)){
                                num+=((chr-'a')+10)*index;
                                index*=16;
                        }
                        else if((chr>'A'-1)&&(chr<'F'+1)){
                                num+=((chr-'A')+10)*index;
                                index*=16;
                        }
                        else
                                break;
                }
        }
        return num;
}

int change_str(char * input)
{
	int i;
	int n = strlen(input);
	for (i=0;i<n;i++)
	{
		if (input[i] == '\r' || input[i] == '\n')
		{
			input[i] = '\0';
			return 0;
		}
	}
	
	return 0;
}


static char ch(char c)
{
	if(c>=97&&c<=122)
		c-=32;
	return c;
}
void str_conv(char *str1)
{
	while(*str1)
	{
		if(*str1>='A'&&*str1<='Z')
		{
			*str1+=('a'-'A');
		}
		str1++;
	}
}

void  checkstr(char *input)
{

        int i = 0;
        int len = strlen(input);
        for(i=0;i<len;i++)
        {
                if ((input[i] < 33 ) || (input[i] > 126 ))
                {
                        input[i] = 0x0;
                        return;
                }
                //
        }

}

int get_parma_count(char *str)
{
	int i = 0;int len;
	int ret = 1;
	char *body =
		strstr(str,"=");
	if (body <= 0)
		return -1;

	body += 1;
	len = strlen(body);
	for(i=0;i<len;i++)
	{
		if (body[i] == ',' )
			ret ++;
		//
	}
	return ret;
}


char* get_parma(int index , char *instr)
{

	char *parma;int i;
	static char in[128];
	//char *in ;
	char *body ;

	//in = rt_malloc(128);
	if (in <=0 ) {return 0;};

	snprintf(in,sizeof(in),"%s",instr);
	body = strstr(in,"=");
	if (body < 0)
	{
		//rt_free(in);
		return 0;
	}
	body = body ++;

	parma = strtok(body,",");
	if (index == 1)
	{
		checkstr(parma);
		//rt_free(in);
		return parma;
	}

	for(i=1;i<index;i++)
	{
		parma = strtok(0,",");
	}

	checkstr(parma);

	//rt_free(in);
	return parma;

}
char* get_parma_2(int index , char *instr) //这个函数再取参数的时候不会去掉特殊字符
{

	char *parma;int i;
	static char in[128];
	//char *in ;
	char *body ;

	//in = rt_malloc(128);
	if (in <=0 ) {return 0;};

	snprintf(in,sizeof(in),"%s",instr);
	body = strstr(in,"=");
	if (body < 0)
	{
		//rt_free(in);
		return 0;
	}
	body = body ++;

	parma = strtok(body,",");
	if (index == 1)
	{
		return parma;
	}

	for(i=1;i<index;i++)
	{
		parma = strtok(0,",");
	}

	//rt_free(in);
	return parma;

}

char *parser_multi_parameter_str(int index , char *str_in , char sp, char *str_out , unsigned char out_lmt)
{
	int i=0,j=0,k=0;
	int len = strlen(str_in);
	int p_idx = 0;
	for(i=0;i<len;i++)
	{
		if (str_in[i] == sp) p_idx ++;
		if (p_idx == index)
		{
			if (str_in[i] != sp)
			{
				str_out[j++] = str_in[i];
				if (j>=(out_lmt-1))
				{
					goto out;
				}
			}
			//
		}

		if (p_idx > index)
			goto out;
	}

out:
	str_out[j] = 0x0;
	return str_out;
}

/*
#include <stdio.h>

int main()
{
        char ex[] = "AJOISLDHFWUEHDSFSJKLSJDFKJS";

        int i = 0;
        for(;i<sizeof(ex);i++)
        {
                printf("buffer[%d] = 0x%x;\n",i,ex[i]);
                //
        }
}
*/
void get_genstr(char *buffer)
{
	//AJOISLDHFWUEHDSFSJKLSJDFKJS
	buffer[0] = 0x41;
	buffer[1] = 0x4a;
	buffer[2] = 0x4f;
	buffer[3] = 0x49;
	buffer[4] = 0x53;
	buffer[5] = 0x4c;
	buffer[6] = 0x44;
	buffer[7] = 0x48;
	buffer[8] = 0x46;
	buffer[9] = 0x57;
	buffer[10] = 0x55;
	buffer[11] = 0x45;
	buffer[12] = 0x48;
	buffer[13] = 0x44;
	buffer[14] = 0x53;
	buffer[15] = 0x46;
	buffer[16] = 0x53;
	buffer[17] = 0x4a;
	buffer[18] = 0x4b;
	buffer[19] = 0x4c;
	buffer[20] = 0x53;
	buffer[21] = 0x4a;
	buffer[22] = 0x44;
	buffer[23] = 0x46;
	buffer[24] = 0x4b;
	buffer[25] = 0x4a;
	buffer[26] = 0x53;
	buffer[27] = 0x0;
}


char *get_p(char *str_in , char *str_out , unsigned char out_lmt)
{
	int i=0;
	int len = strlen(str_in);
	unsigned char status = 0;
	unsigned char str_out_index = 0;

	for(i=0;i<len;i++)
	{
		switch(status)
		{
			case 0:
				if ((str_in[i] == '\"') || (str_in[i] == '\''))
				{
					status = 1; //第一次找到了引号
					//
				}
				break;
			case 1:
				if ((str_in[i] == '\"') || (str_in[i] == '\''))
				{
					status = 2; //第二次找到了引号
				}else{
					/* 第一次找到了引号，后就进行数据处理 */
					str_out[str_out_index++] = str_in[i];
					if (str_out_index >= (out_lmt-1))
						status = 3; //没有空间进行数据输出了
				}
				break;
			case 2:
			case 3:
				goto __out;
			default:
				break;
		}
	}
__out:
	str_out[str_out_index] = 0x0; //末尾一个字节为0X0
	if (status == 2)
		return str_in;
	else
		return 0;

}

/**
	example : 
	char input[] = "abc=abc,\"123\"";
	char output[16];
	get_str_fragment(intput,"\"","\"",output,16);
	printf : 123
*/
int get_str_fragment(const char * in_str , const char *start_str  , const char *end_str , char *out_buf , int buflen)
{
        //
        int i;
        unsigned int j;
        char * str1,*str2,*str3;
        int keywordlen = strlen(start_str);
        int inlen = strlen(in_str);

        str1 = strstr(in_str,start_str);
        str2 = strstr(str1+keywordlen,end_str);

        //printf("str1 : %s \r\n",str1);
        //printf("str2 : %s \r\n",str2);

        i=j=0;
        if ((str1>0) && (str2>0) && (str2 > str1))
        {
                j = ((unsigned int)str2 - (unsigned int)str1) - keywordlen;
					
								if ((j<=0) || ( j >= buflen))
								{
									out_buf[0] = 0x0;
									return 0;
								}
					
                //printf("str2-str1 : %d\r\n",j);
                for(i=0;i<j;i++)
                {
                        out_buf[i] = str1[keywordlen + i];
                }
                out_buf[i] = 0x0;
                //
        }

        return 0;
}



void set_4bit_h(unsigned char * _byte , unsigned char val)
{
        unsigned char val_tmp,val_tmp2;
        val_tmp = val * 16;
        val_tmp2 =  *_byte * 0x0F;
        *_byte = val_tmp + val_tmp2;
        //
}

void set_4bit_l(unsigned char * _byte , unsigned char val)
{
        unsigned char val_tmp;
        val_tmp =  *_byte & 0xF0;
        val_tmp += val;
        *_byte = val_tmp;
        //
}

unsigned char get_4bit_h(unsigned char _byte)
{
        return _byte >> 4;
}

unsigned char get_4bit_l(unsigned char _byte)
{
        unsigned char val_tmp;
        val_tmp =  _byte << 4;
        return val_tmp >> 4;
}




