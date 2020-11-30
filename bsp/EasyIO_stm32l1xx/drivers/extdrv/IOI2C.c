/* IOI2C.c file
   ��д�ߣ�lisn3188
   ��ַ��www.chiplab7.com
   ����E-mail��lisn3188@163.com
   ���뻷����MDK-Lite  Version: 4.23
   ����ʱ��: 2012-06-25
   ���ԣ� ���������ڵ���ʵ���ҵ�[Captain �ɿذ�]����ɲ���

   ռ����Դ��
   1. I2C �ӿ� PB6  PB7 ����

   ���ܣ�
   �ṩI2C�ӿڲ���API ��
   ʹ��IOģ�ⷽʽ
   ------------------------------------
 */

#if 0

#include <stm32l1xx.h>
#include "IOI2C.h"
#include <stm32l1xx_gpio.h>
#include <stm32l1xx_rcc.h>
#include <rtthread.h>

//#include "delay.h"

static void _delay_us(rt_uint32_t us)
{
        rt_uint32_t len;
        for (;us > 0; us --)
                for (len = 0; len < 20; len++ );
}

static void delay_us(rt_uint32_t us)
{
	_delay_us(us*15);
}

#ifdef EASYIO_BOARD_1
#define SDA_PORT GPIOB
#define SCL_PORT GPIOB
#define SDA_PORT_PIN GPIO_Pin_7
#define SCL_PORT_PIN GPIO_Pin_6
#endif

#ifdef EASYIO_UBLOX_U2
#define SDA_PORT GPIOB
#define SCL_PORT GPIOB
#define SDA_PORT_PIN GPIO_Pin_7
#define SCL_PORT_PIN GPIO_Pin_6
#endif


//IO��������
#define SDA_IN()  SDAPortIN()
#define SDA_OUT() SDAPortOUT();

void __SDA_H(void)
{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	memset(&GPIO_InitStructure,0x0,sizeof(GPIO_InitStructure));
//	
//	GPIO_InitStructure.GPIO_Pin =  SDA_PORT_PIN; 
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN; 
//	GPIO_Init(SCL_PORT, &GPIO_InitStructure); 
	GPIO_SetBits(SDA_PORT, SDA_PORT_PIN);
	//
};
void __SDA_L(void)
{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	memset(&GPIO_InitStructure,0x0,sizeof(GPIO_InitStructure));
//	
//	GPIO_InitStructure.GPIO_Pin =  SDA_PORT_PIN; 
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
//	GPIO_Init(SCL_PORT, &GPIO_InitStructure); 
	GPIO_ResetBits(SDA_PORT, SDA_PORT_PIN);
	//
};

void __SCL_H(void)
{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	memset(&GPIO_InitStructure,0x0,sizeof(GPIO_InitStructure));
//	
//	GPIO_InitStructure.GPIO_Pin =  SCL_PORT_PIN; 
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
//	GPIO_Init(SCL_PORT, &GPIO_InitStructure); 
	GPIO_SetBits(SCL_PORT, SCL_PORT_PIN);
	//
};
void __SCL_L(void)
{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	memset(&GPIO_InitStructure,0x0,sizeof(GPIO_InitStructure));
//	
//	GPIO_InitStructure.GPIO_Pin =  SCL_PORT_PIN; 
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
//	GPIO_Init(SCL_PORT, &GPIO_InitStructure); 
	GPIO_ResetBits(SCL_PORT, SCL_PORT_PIN);
	//
};



#if 0
#define SDA_H  GPIO_SetBits(SDA_PORT, SDA_PORT_PIN)
#define SDA_L  GPIO_ResetBits(SDA_PORT, SDA_PORT_PIN)

#define SCL_H  GPIO_SetBits(SCL_PORT, SCL_PORT_PIN)
#define SCL_L  GPIO_ResetBits(SCL_PORT, SCL_PORT_PIN)

#else

#define SDA_H  __SDA_H()
#define SDA_L  __SDA_L()

#define SCL_H  __SCL_H()
#define SCL_L  __SCL_L()
#endif


#define READ_SDA GPIO_ReadInputDataBit(SDA_PORT, SDA_PORT_PIN)

unsigned short I2C_Erorr_Count = 0;

typedef unsigned char u8;

/**************************ʵ�ֺ���********************************************
 *����ԭ��:		void IIC_Init(void)
 *��������:		��ʼ��I2C��Ӧ�Ľӿ����š�
 *******************************************************************************/
void IOI2C_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	memset(&GPIO_InitStructure,0x0,sizeof(GPIO_InitStructure));
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin =  SDA_PORT_PIN; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
	GPIO_Init(SDA_PORT, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin =  SCL_PORT_PIN; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
	GPIO_Init(SCL_PORT, &GPIO_InitStructure); 

	
}

static void SDAPortIN(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	memset(&GPIO_InitStructure,0x0,sizeof(GPIO_InitStructure));
	GPIO_InitStructure.GPIO_Pin = SDA_PORT_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN;         
	GPIO_Init(SDA_PORT,&GPIO_InitStructure);
}

static void SDAPortOUT(void)
{
//	
	GPIO_InitTypeDef  GPIO_InitStructure;
	memset(&GPIO_InitStructure,0x0,sizeof(GPIO_InitStructure));
	GPIO_InitStructure.GPIO_Pin = SDA_PORT_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_OUT;    
	//GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(SDA_PORT,&GPIO_InitStructure);
}



/**************************ʵ�ֺ���********************************************
 *����ԭ��:		void IIC_Start(void)
 *��������:		����IIC��ʼ�ź�
 *******************************************************************************/
static void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	SDA_H;//IIC_SDA=1;	  	  
	SCL_H;//IIC_SCL=1;
	delay_us(2);
	SDA_L;//IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(2);
	SCL_L;//IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}

/**************************ʵ�ֺ���********************************************
 *����ԭ��:		void IIC_Stop(void)
 *��������:	    //����IICֹͣ�ź�
 *******************************************************************************/	  
static void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	SCL_L;//IIC_SCL=0;
	SDA_L;//IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
	delay_us(2);
	SCL_H;//IIC_SCL=1; 
	SDA_H;//IIC_SDA=1;//����I2C���߽����ź�
	delay_us(2);							   	
}

/**************************ʵ�ֺ���********************************************
 *����ԭ��:		u8 IIC_Wait_Ack(void)
 *��������:	    �ȴ�Ӧ���źŵ��� 
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
 *******************************************************************************/
static u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
	//SDA_H;//IIC_SDA=1;
	delay_us(1);	   
	SCL_H;//IIC_SCL=1;
	delay_us(1);	 
	while(READ_SDA)
	{
#if 1
		ucErrTime++;
		if(ucErrTime>50)
		{
			IIC_Stop();
			I2C_Erorr_Count ++;	 //I2C ����
			return 1;
		}
		delay_us(1);
#endif
	}
	SCL_L;//IIC_SCL=0;//ʱ�����0 	   
	return 0;  
} 

/**************************ʵ�ֺ���********************************************
 *����ԭ��:		void IIC_Ack(void)
 *��������:	    ����ACKӦ��
 *******************************************************************************/
static void IIC_Ack(void)
{
	SCL_L;//IIC_SCL=0;
	SDA_OUT();
	SDA_L;//IIC_SDA=0;
	delay_us(1);
	SCL_H;//IIC_SCL=1;
	delay_us(1);
	SCL_L;//IIC_SCL=0;
}

/**************************ʵ�ֺ���********************************************
 *����ԭ��:		void IIC_NAck(void)
 *��������:	    ����NACKӦ��
 *******************************************************************************/	    
static void IIC_NAck(void)
{
	SCL_L;//IIC_SCL=0;
	SDA_OUT();
	SDA_H;//IIC_SDA=1;
	delay_us(1);
	SCL_H;//IIC_SCL=1;
	delay_us(1);
	SCL_L;//IIC_SCL=0;
}					 				     

/**************************ʵ�ֺ���********************************************
 *����ԭ��:		void IIC_Send_Byte(u8 txd)
 *��������:	    IIC����һ���ֽ�
 *******************************************************************************/		  
static void IIC_Send_Byte(u8 txd)
{                        
	u8 t;   
	u8 ttt = txd;
	//SDA_OUT(); 	    
	SCL_L;//IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
	
	//for(;;)
	{
	txd = ttt;
	for(t=0;t<8;t++)
	{              
		//IIC_SDA=(txd&0x80)>>7;
		if ((txd&0x80)>>7)
			SDA_H;
		else
			SDA_L;

		txd<<=1; 	  
		delay_us(1);   
		SCL_H;//IIC_SCL=1;
		delay_us(1); 
		SCL_L;//IIC_SCL=0;	
		delay_us(1);
	}	

}	
} 	 

/**************************ʵ�ֺ���********************************************
 *����ԭ��:		u8 IIC_Read_Byte(unsigned char ack)
 *��������:	    //��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK 
 *******************************************************************************/  
static u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
	for(i=0;i<8;i++ )
	{
		SCL_L;//IIC_SCL=0; 
		delay_us(1);
		SCL_H;//IIC_SCL=1;
		receive<<=1;
		if(READ_SDA)receive++;   
		delay_us(1); 
	}					 
	if (ack)
		IIC_Ack(); //����ACK 
	else
		IIC_NAck();//����nACK  
	return receive;
}	 				     


/**************************ʵ�ֺ���********************************************
 *����ԭ��:		unsigned char I2C_ReadOneByte(unsigned char I2C_Addr,unsigned char addr)
 *��������:	    ��ȡָ���豸 ָ���Ĵ�����һ��ֵ
 ����	I2C_Addr  Ŀ���豸��ַ
 addr	   �Ĵ�����ַ
 ����   ��������ֵ
 *******************************************************************************/ 
static unsigned char I2C_ReadOneByte(unsigned char I2C_Addr,unsigned char addr)
{
	unsigned char res=0;

	IIC_Start();	
	IIC_Send_Byte(I2C_Addr);	   //����д����
	res++;
	IIC_Wait_Ack();
	IIC_Send_Byte(addr); res++;  //���͵�ַ
	IIC_Wait_Ack();	  
	//IIC_Stop();//����һ��ֹͣ����	
	IIC_Start();
	IIC_Send_Byte(I2C_Addr+1); res++;          //�������ģʽ			   
	IIC_Wait_Ack();
	res=IIC_Read_Byte(0);	   
	IIC_Stop();//����һ��ֹͣ����

	return res;
}


/**************************ʵ�ֺ���********************************************
 *����ԭ��:		u8 IICreadBytes(u8 dev, u8 reg, u8 length, u8 *data)
 *��������:	    ��ȡָ���豸 ָ���Ĵ����� length��ֵ
 ����	dev  Ŀ���豸��ַ
 reg	  �Ĵ�����ַ
 length Ҫ�����ֽ���
 *data  ���������ݽ�Ҫ��ŵ�ָ��
 ����   ���������ֽ�����
 *******************************************************************************/ 
u8 IICreadBytes(u8 dev, u8 reg, u8 length, u8 *data){
	u8 count = 0;

	IIC_Start();
	//for(;;)
	IIC_Send_Byte(dev);	   //����д����
	IIC_Wait_Ack();
	IIC_Send_Byte(reg);   //���͵�ַ
	IIC_Wait_Ack();	  
	IIC_Start();
	IIC_Send_Byte(dev+1);  //�������ģʽ	
	IIC_Wait_Ack();

	for(count=0;count<length;count++){

		if(count!=length-1)data[count]=IIC_Read_Byte(1);  //��ACK�Ķ�����
		else  data[count]=IIC_Read_Byte(0);	 //���һ���ֽ�NACK
	}
	IIC_Stop();//����һ��ֹͣ����
	return count;
}

/**************************ʵ�ֺ���********************************************
 *����ԭ��:		u8 IICwriteBytes(u8 dev, u8 reg, u8 length, u8* data)
 *��������:	    ������ֽ�д��ָ���豸 ָ���Ĵ���
 ����	dev  Ŀ���豸��ַ
 reg	  �Ĵ�����ַ
 length Ҫд���ֽ���
 *data  ��Ҫд�����ݵ��׵�ַ
 ����   �����Ƿ�ɹ�
 *******************************************************************************/ 
u8 IICwriteBytes(u8 dev, u8 reg, u8 length, u8* data){

	u8 count = 0;
	IIC_Start();
	IIC_Send_Byte(dev);	   //����д����
	IIC_Wait_Ack();
	IIC_Send_Byte(reg);   //���͵�ַ
	IIC_Wait_Ack();	  
	for(count=0;count<length;count++){
		IIC_Send_Byte(data[count]); 
		IIC_Wait_Ack(); 
	}
	IIC_Stop();//����һ��ֹͣ����

	return 1; //status == 0;
}

/**************************ʵ�ֺ���********************************************
 *����ԭ��:		u8 IICreadByte(u8 dev, u8 reg, u8 *data)
 *��������:	    ��ȡָ���豸 ָ���Ĵ�����һ��ֵ
 ����	dev  Ŀ���豸��ַ
 reg	   �Ĵ�����ַ
 *data  ���������ݽ�Ҫ��ŵĵ�ַ
 ����   1
 *******************************************************************************/ 
u8 IICreadByte(u8 dev, u8 reg, u8 *data){
	*data=I2C_ReadOneByte(dev, reg);
	return 1;
}

/**************************ʵ�ֺ���********************************************
 *����ԭ��:		unsigned char IICwriteByte(unsigned char dev, unsigned char reg, unsigned char data)
 *��������:	    д��ָ���豸 ָ���Ĵ���һ���ֽ�
 ����	dev  Ŀ���豸��ַ
 reg	   �Ĵ�����ַ
 data  ��Ҫд����ֽ�
 ����   1
 *******************************************************************************/ 
static unsigned char IICwriteByte(unsigned char dev, unsigned char reg, unsigned char data){
	return IICwriteBytes(dev, reg, 1, &data);
}

/**************************ʵ�ֺ���********************************************
 *����ԭ��:		u8 IICwriteBits(u8 dev,u8 reg,u8 bitStart,u8 length,u8 data)
 *��������:	    �� �޸� д ָ���豸 ָ���Ĵ���һ���ֽ� �еĶ��λ
 ����	dev  Ŀ���豸��ַ
 reg	   �Ĵ�����ַ
 bitStart  Ŀ���ֽڵ���ʼλ
 length   λ����
 data    ��Ÿı�Ŀ���ֽ�λ��ֵ
 ����   �ɹ� Ϊ1 
 ʧ��Ϊ0
 *******************************************************************************/ 
static u8 IICwriteBits(u8 dev,u8 reg,u8 bitStart,u8 length,u8 data)
{

	u8 b;
	if (IICreadByte(dev, reg, &b) != 0) {
		u8 mask = (0xFF << (bitStart + 1)) | 0xFF >> ((8 - bitStart) + length - 1);
		data <<= (8 - length);
		data >>= (7 - bitStart);
		b &= mask;
		b |= data;
		return IICwriteByte(dev, reg, b);
	} else {
		return 0;
	}
}

/**************************ʵ�ֺ���********************************************
 *����ԭ��:		u8 IICwriteBit(u8 dev, u8 reg, u8 bitNum, u8 data)
 *��������:	    �� �޸� д ָ���豸 ָ���Ĵ���һ���ֽ� �е�1��λ
 ����	dev  Ŀ���豸��ַ
 reg	   �Ĵ�����ַ
 bitNum  Ҫ�޸�Ŀ���ֽڵ�bitNumλ
 data  Ϊ0 ʱ��Ŀ��λ������0 ���򽫱���λ
 ����   �ɹ� Ϊ1 
 ʧ��Ϊ0
 *******************************************************************************/ 
static u8 IICwriteBit(u8 dev, u8 reg, u8 bitNum, u8 data){
	u8 b;
	IICreadByte(dev, reg, &b);
	b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
	return IICwriteByte(dev, reg, b);
}

//------------------End of File----------------------------




#include <stm32l1xx.h>

#include <stm32l1xx_gpio.h>
#include <stm32l1xx_i2c.h>



void I2C_GPIO_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void I2C1_Init(void)
{
	I2C_InitTypeDef  I2C_InitStructure; 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x39;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 400000;
	I2C_Cmd(I2C1, ENABLE);
	I2C_Init(I2C1, &I2C_InitStructure);
}

void I2C_Write(u8 addr, u8 data)
{
	I2C_AcknowledgeConfig(I2C1,ENABLE); //ʹ��Ӧ��
	I2C_GenerateSTART(I2C1,ENABLE);  //����һ����ʼλ
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT)){;}//�ȴ�EV5
	I2C_Send7bitAddress(I2C1,0x39,I2C_Direction_Transmitter); //���ʹӵ�ַ
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){;} //�ȴ�EV6
	I2C_SendData(I2C1,addr); //����Ҫд�����ݵĵ�ַ
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)){;} //�ȴ�EV8
	I2C_SendData(I2C1,data); //����Ҫд�������
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)){;} //�ȴ�EV8
	I2C_GenerateSTOP(I2C1,ENABLE); //����ֹͣλ
}

u8 I2C_Read(u8 nAddr)
{
	I2C_AcknowledgeConfig(I2C1,ENABLE); //ʹ��Ӧ��
	I2C_GenerateSTART(I2C1,ENABLE); //����һ����ʼλ
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT)){;} //�ȴ�EV5
	I2C_Send7bitAddress(I2C1,0x39,I2C_Direction_Transmitter); //����һ��αдָ�� 
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){;}//�ȴ�EV6
	I2C_SendData(I2C1,nAddr);//���Ͷ���ַ   
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)){;} //�ȴ�EV8

	I2C_GenerateSTART(I2C1,ENABLE); //����һ����ʼλ
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT)){;} //�ȴ�EV5
	I2C_Send7bitAddress(I2C1,0x39,I2C_Direction_Receiver); //����һ����ָ��
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)){;} //�ȴ�EV6
	I2C_AcknowledgeConfig(I2C1,DISABLE);  //Ӧ��ʹ�ܹر�
	I2C_GenerateSTOP(I2C1,ENABLE);   //����һ��ֹͣλ
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED)){;} //�ȴ�EV7
	return I2C_ReceiveData(I2C1);  //���ض���������
}

#else


/* IOI2C.c file
   ��д�ߣ�lisn3188
   ��ַ��www.chiplab7.com
   ����E-mail��lisn3188@163.com
   ���뻷����MDK-Lite  Version: 4.23
   ����ʱ��: 2012-06-25
   ���ԣ� ���������ڵ���ʵ���ҵ�[Captain �ɿذ�]����ɲ���

   ռ����Դ��
   1. I2C �ӿ� PB6  PB7 ����

   ���ܣ�
   �ṩI2C�ӿڲ���API ��
   ʹ��IOģ�ⷽʽ
   ------------------------------------
 */
 
#include <stm32l1xx.h>
#include "IOI2C.h"
#include <stm32l1xx_gpio.h>
#include <stm32l1xx_rcc.h>
#include <rtthread.h>

//#include "delay.h"

static void _delay_us(rt_uint32_t us)
{
        rt_uint32_t len;
        for (;us > 0; us --)
                for (len = 0; len < 20; len++ );
}

static void delay_us(rt_uint32_t us)
{
	_delay_us(us*20);
}


#define SDA_PORT GPIOB
#define SCL_PORT GPIOB
#define SDA_PORT_PIN GPIO_Pin_7
#define SCL_PORT_PIN GPIO_Pin_6



//IO��������
#define SDA_IN()  SDAPortIN()
#define SDA_OUT() SDAPortOUT();



#define SDA_H  GPIO_SetBits(SDA_PORT, SDA_PORT_PIN)
#define SDA_L  GPIO_ResetBits(SDA_PORT, SDA_PORT_PIN)

#define SCL_H  GPIO_SetBits(SCL_PORT, SCL_PORT_PIN)
#define SCL_L  GPIO_ResetBits(SCL_PORT, SCL_PORT_PIN)

#define READ_SDA GPIO_ReadInputDataBit(SDA_PORT, SDA_PORT_PIN)

unsigned short I2C_Erorr_Count = 0;

/**************************ʵ�ֺ���********************************************
 *����ԭ��:		void IIC_Init(void)
 *��������:		��ʼ��I2C��Ӧ�Ľӿ����š�
 *******************************************************************************/
void IOI2C_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	memset(&GPIO_InitStructure,0x0,sizeof(GPIO_InitStructure));
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin =  SDA_PORT_PIN; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
	GPIO_Init(SDA_PORT, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin =  SCL_PORT_PIN; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
	GPIO_Init(SCL_PORT, &GPIO_InitStructure); 
	
	
}

static void SDAPortIN(void)
{
	
	GPIO_InitTypeDef  GPIO_InitStructure;memset(&GPIO_InitStructure,0x0,sizeof(GPIO_InitStructure));
	GPIO_InitStructure.GPIO_Pin = SDA_PORT_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN;         
	GPIO_Init(SDA_PORT,&GPIO_InitStructure);
}

static void SDAPortOUT(void)
{
	
	GPIO_InitTypeDef  GPIO_InitStructure;memset(&GPIO_InitStructure,0x0,sizeof(GPIO_InitStructure));
	GPIO_InitStructure.GPIO_Pin = SDA_PORT_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_OUT;         
	GPIO_Init(SDA_PORT,&GPIO_InitStructure);
}



/**************************ʵ�ֺ���********************************************
 *����ԭ��:		void IIC_Start(void)
 *��������:		����IIC��ʼ�ź�
 *******************************************************************************/
static void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	SDA_H;//IIC_SDA=1;	  	  
	SCL_H;//IIC_SCL=1;
	delay_us(2);
	SDA_L;//IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(2);
	SCL_L;//IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}

/**************************ʵ�ֺ���********************************************
 *����ԭ��:		void IIC_Stop(void)
 *��������:	    //����IICֹͣ�ź�
 *******************************************************************************/	  
static void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	SCL_L;//IIC_SCL=0;
	SDA_L;//IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
	delay_us(2);
	SCL_H;//IIC_SCL=1; 
	SDA_H;//IIC_SDA=1;//����I2C���߽����ź�
	delay_us(2);							   	
}

/**************************ʵ�ֺ���********************************************
 *����ԭ��:		u8 IIC_Wait_Ack(void)
 *��������:	    �ȴ�Ӧ���źŵ��� 
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
 *******************************************************************************/
static u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
	SDA_H;//IIC_SDA=1;
	delay_us(1);	   
	SCL_H;//IIC_SCL=1;
	delay_us(1);	 
	while(READ_SDA)
	{
#if 1
		ucErrTime++;
		if(ucErrTime>50)
		{
			IIC_Stop();
			I2C_Erorr_Count ++;	 //I2C ����
			return 1;
		}
		delay_us(1);
#endif
	}
	SCL_L;//IIC_SCL=0;//ʱ�����0 	   
	return 0;  
} 

/**************************ʵ�ֺ���********************************************
 *����ԭ��:		void IIC_Ack(void)
 *��������:	    ����ACKӦ��
 *******************************************************************************/
static void IIC_Ack(void)
{
	SCL_L;//IIC_SCL=0;
	SDA_OUT();
	SDA_L;//IIC_SDA=0;
	delay_us(1);
	SCL_H;//IIC_SCL=1;
	delay_us(1);
	SCL_L;//IIC_SCL=0;
}

/**************************ʵ�ֺ���********************************************
 *����ԭ��:		void IIC_NAck(void)
 *��������:	    ����NACKӦ��
 *******************************************************************************/	    
static void IIC_NAck(void)
{
	SCL_L;//IIC_SCL=0;
	SDA_OUT();
	SDA_H;//IIC_SDA=1;
	delay_us(1);
	SCL_H;//IIC_SCL=1;
	delay_us(1);
	SCL_L;//IIC_SCL=0;
}					 				     

/**************************ʵ�ֺ���********************************************
 *����ԭ��:		void IIC_Send_Byte(u8 txd)
 *��������:	    IIC����һ���ֽ�
 *******************************************************************************/		  
static void IIC_Send_Byte(u8 txd)
{                        
	u8 t;   
	SDA_OUT(); 	    
	SCL_L;//IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
	for(t=0;t<8;t++)
	{              
		//IIC_SDA=(txd&0x80)>>7;
		if ((txd&0x80)>>7)
			SDA_H;
		else
			SDA_L;

		txd<<=1; 	  
		delay_us(1);   
		SCL_H;//IIC_SCL=1;
		delay_us(1); 
		SCL_L;//IIC_SCL=0;	
		delay_us(1);
	}	 
} 	 

/**************************ʵ�ֺ���********************************************
 *����ԭ��:		u8 IIC_Read_Byte(unsigned char ack)
 *��������:	    //��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK 
 *******************************************************************************/  
static u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
	for(i=0;i<8;i++ )
	{
		SCL_L;//IIC_SCL=0; 
		delay_us(1);
		SCL_H;//IIC_SCL=1;
		receive<<=1;
		if(READ_SDA)receive++;   
		delay_us(1); 
	}					 
	if (ack)
		IIC_Ack(); //����ACK 
	else
		IIC_NAck();//����nACK  
	return receive;
}	 				     


/**************************ʵ�ֺ���********************************************
 *����ԭ��:		unsigned char I2C_ReadOneByte(unsigned char I2C_Addr,unsigned char addr)
 *��������:	    ��ȡָ���豸 ָ���Ĵ�����һ��ֵ
 ����	I2C_Addr  Ŀ���豸��ַ
 addr	   �Ĵ�����ַ
 ����   ��������ֵ
 *******************************************************************************/ 
static unsigned char I2C_ReadOneByte(unsigned char I2C_Addr,unsigned char addr)
{
	unsigned char res=0;

	IIC_Start();	
	IIC_Send_Byte(I2C_Addr);	   //����д����
	res++;
	IIC_Wait_Ack();
	IIC_Send_Byte(addr); res++;  //���͵�ַ
	IIC_Wait_Ack();	  
	//IIC_Stop();//����һ��ֹͣ����	
	IIC_Start();
	IIC_Send_Byte(I2C_Addr+1); res++;          //�������ģʽ			   
	IIC_Wait_Ack();
	res=IIC_Read_Byte(0);	   
	IIC_Stop();//����һ��ֹͣ����

	return res;
}


/**************************ʵ�ֺ���********************************************
 *����ԭ��:		u8 IICreadBytes(u8 dev, u8 reg, u8 length, u8 *data)
 *��������:	    ��ȡָ���豸 ָ���Ĵ����� length��ֵ
 ����	dev  Ŀ���豸��ַ
 reg	  �Ĵ�����ַ
 length Ҫ�����ֽ���
 *data  ���������ݽ�Ҫ��ŵ�ָ��
 ����   ���������ֽ�����
 *******************************************************************************/ 
u8 IICreadBytes(u8 dev, u8 reg, u8 length, u8 *data){
	u8 count = 0;

	IIC_Start();
	IIC_Send_Byte(dev);	   //����д����
	IIC_Wait_Ack();
	IIC_Send_Byte(reg);   //���͵�ַ
	IIC_Wait_Ack();	  
	IIC_Start();
	IIC_Send_Byte(dev+1);  //�������ģʽ	
	IIC_Wait_Ack();

	for(count=0;count<length;count++){

		if(count!=length-1)data[count]=IIC_Read_Byte(1);  //��ACK�Ķ�����
		else  data[count]=IIC_Read_Byte(0);	 //���һ���ֽ�NACK
	}
	IIC_Stop();//����һ��ֹͣ����
	return count;
}

/**************************ʵ�ֺ���********************************************
 *����ԭ��:		u8 IICwriteBytes(u8 dev, u8 reg, u8 length, u8* data)
 *��������:	    ������ֽ�д��ָ���豸 ָ���Ĵ���
 ����	dev  Ŀ���豸��ַ
 reg	  �Ĵ�����ַ
 length Ҫд���ֽ���
 *data  ��Ҫд�����ݵ��׵�ַ
 ����   �����Ƿ�ɹ�
 *******************************************************************************/ 
u8 IICwriteBytes(u8 dev, u8 reg, u8 length, u8* data){

	u8 count = 0;
	IIC_Start();
	IIC_Send_Byte(dev);	   //����д����
	IIC_Wait_Ack();
	IIC_Send_Byte(reg);   //���͵�ַ
	IIC_Wait_Ack();	  
	for(count=0;count<length;count++){
		IIC_Send_Byte(data[count]); 
		IIC_Wait_Ack(); 
	}
	IIC_Stop();//����һ��ֹͣ����

	return 1; //status == 0;
}

/**************************ʵ�ֺ���********************************************
 *����ԭ��:		u8 IICreadByte(u8 dev, u8 reg, u8 *data)
 *��������:	    ��ȡָ���豸 ָ���Ĵ�����һ��ֵ
 ����	dev  Ŀ���豸��ַ
 reg	   �Ĵ�����ַ
 *data  ���������ݽ�Ҫ��ŵĵ�ַ
 ����   1
 *******************************************************************************/ 
u8 IICreadByte(u8 dev, u8 reg, u8 *data){
	*data=I2C_ReadOneByte(dev, reg);
	return 1;
}

/**************************ʵ�ֺ���********************************************
 *����ԭ��:		unsigned char IICwriteByte(unsigned char dev, unsigned char reg, unsigned char data)
 *��������:	    д��ָ���豸 ָ���Ĵ���һ���ֽ�
 ����	dev  Ŀ���豸��ַ
 reg	   �Ĵ�����ַ
 data  ��Ҫд����ֽ�
 ����   1
 *******************************************************************************/ 
unsigned char IICwriteByte(unsigned char dev, unsigned char reg, unsigned char data){
	return IICwriteBytes(dev, reg, 1, &data);
}

/**************************ʵ�ֺ���********************************************
 *����ԭ��:		u8 IICwriteBits(u8 dev,u8 reg,u8 bitStart,u8 length,u8 data)
 *��������:	    �� �޸� д ָ���豸 ָ���Ĵ���һ���ֽ� �еĶ��λ
 ����	dev  Ŀ���豸��ַ
 reg	   �Ĵ�����ַ
 bitStart  Ŀ���ֽڵ���ʼλ
 length   λ����
 data    ��Ÿı�Ŀ���ֽ�λ��ֵ
 ����   �ɹ� Ϊ1 
 ʧ��Ϊ0
 *******************************************************************************/ 
static u8 IICwriteBits(u8 dev,u8 reg,u8 bitStart,u8 length,u8 data)
{

	u8 b;
	if (IICreadByte(dev, reg, &b) != 0) {
		u8 mask = (0xFF << (bitStart + 1)) | 0xFF >> ((8 - bitStart) + length - 1);
		data <<= (8 - length);
		data >>= (7 - bitStart);
		b &= mask;
		b |= data;
		return IICwriteByte(dev, reg, b);
	} else {
		return 0;
	}
}

/**************************ʵ�ֺ���********************************************
 *����ԭ��:		u8 IICwriteBit(u8 dev, u8 reg, u8 bitNum, u8 data)
 *��������:	    �� �޸� д ָ���豸 ָ���Ĵ���һ���ֽ� �е�1��λ
 ����	dev  Ŀ���豸��ַ
 reg	   �Ĵ�����ַ
 bitNum  Ҫ�޸�Ŀ���ֽڵ�bitNumλ
 data  Ϊ0 ʱ��Ŀ��λ������0 ���򽫱���λ
 ����   �ɹ� Ϊ1 
 ʧ��Ϊ0
 *******************************************************************************/ 
static u8 IICwriteBit(u8 dev, u8 reg, u8 bitNum, u8 data){
	u8 b;
	IICreadByte(dev, reg, &b);
	b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
	return IICwriteByte(dev, reg, b);
}

//------------------End of File----------------------------


#endif