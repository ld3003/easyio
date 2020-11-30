/* IOI2C.c file
   编写者：lisn3188
   网址：www.chiplab7.com
   作者E-mail：lisn3188@163.com
   编译环境：MDK-Lite  Version: 4.23
   初版时间: 2012-06-25
   测试： 本程序已在第七实验室的[Captain 飞控板]上完成测试

   占用资源：
   1. I2C 接口 PB6  PB7 引脚

   功能：
   提供I2C接口操作API 。
   使用IO模拟方式
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


//IO方向设置
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

/**************************实现函数********************************************
 *函数原型:		void IIC_Init(void)
 *功　　能:		初始化I2C对应的接口引脚。
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



/**************************实现函数********************************************
 *函数原型:		void IIC_Start(void)
 *功　　能:		产生IIC起始信号
 *******************************************************************************/
static void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	SDA_H;//IIC_SDA=1;	  	  
	SCL_H;//IIC_SCL=1;
	delay_us(2);
	SDA_L;//IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(2);
	SCL_L;//IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}

/**************************实现函数********************************************
 *函数原型:		void IIC_Stop(void)
 *功　　能:	    //产生IIC停止信号
 *******************************************************************************/	  
static void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	SCL_L;//IIC_SCL=0;
	SDA_L;//IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
	delay_us(2);
	SCL_H;//IIC_SCL=1; 
	SDA_H;//IIC_SDA=1;//发送I2C总线结束信号
	delay_us(2);							   	
}

/**************************实现函数********************************************
 *函数原型:		u8 IIC_Wait_Ack(void)
 *功　　能:	    等待应答信号到来 
//返回值：1，接收应答失败
//        0，接收应答成功
 *******************************************************************************/
static u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
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
			I2C_Erorr_Count ++;	 //I2C 错误
			return 1;
		}
		delay_us(1);
#endif
	}
	SCL_L;//IIC_SCL=0;//时钟输出0 	   
	return 0;  
} 

/**************************实现函数********************************************
 *函数原型:		void IIC_Ack(void)
 *功　　能:	    产生ACK应答
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

/**************************实现函数********************************************
 *函数原型:		void IIC_NAck(void)
 *功　　能:	    产生NACK应答
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

/**************************实现函数********************************************
 *函数原型:		void IIC_Send_Byte(u8 txd)
 *功　　能:	    IIC发送一个字节
 *******************************************************************************/		  
static void IIC_Send_Byte(u8 txd)
{                        
	u8 t;   
	u8 ttt = txd;
	//SDA_OUT(); 	    
	SCL_L;//IIC_SCL=0;//拉低时钟开始数据传输
	
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

/**************************实现函数********************************************
 *函数原型:		u8 IIC_Read_Byte(unsigned char ack)
 *功　　能:	    //读1个字节，ack=1时，发送ACK，ack=0，发送nACK 
 *******************************************************************************/  
static u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
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
		IIC_Ack(); //发送ACK 
	else
		IIC_NAck();//发送nACK  
	return receive;
}	 				     


/**************************实现函数********************************************
 *函数原型:		unsigned char I2C_ReadOneByte(unsigned char I2C_Addr,unsigned char addr)
 *功　　能:	    读取指定设备 指定寄存器的一个值
 输入	I2C_Addr  目标设备地址
 addr	   寄存器地址
 返回   读出来的值
 *******************************************************************************/ 
static unsigned char I2C_ReadOneByte(unsigned char I2C_Addr,unsigned char addr)
{
	unsigned char res=0;

	IIC_Start();	
	IIC_Send_Byte(I2C_Addr);	   //发送写命令
	res++;
	IIC_Wait_Ack();
	IIC_Send_Byte(addr); res++;  //发送地址
	IIC_Wait_Ack();	  
	//IIC_Stop();//产生一个停止条件	
	IIC_Start();
	IIC_Send_Byte(I2C_Addr+1); res++;          //进入接收模式			   
	IIC_Wait_Ack();
	res=IIC_Read_Byte(0);	   
	IIC_Stop();//产生一个停止条件

	return res;
}


/**************************实现函数********************************************
 *函数原型:		u8 IICreadBytes(u8 dev, u8 reg, u8 length, u8 *data)
 *功　　能:	    读取指定设备 指定寄存器的 length个值
 输入	dev  目标设备地址
 reg	  寄存器地址
 length 要读的字节数
 *data  读出的数据将要存放的指针
 返回   读出来的字节数量
 *******************************************************************************/ 
u8 IICreadBytes(u8 dev, u8 reg, u8 length, u8 *data){
	u8 count = 0;

	IIC_Start();
	//for(;;)
	IIC_Send_Byte(dev);	   //发送写命令
	IIC_Wait_Ack();
	IIC_Send_Byte(reg);   //发送地址
	IIC_Wait_Ack();	  
	IIC_Start();
	IIC_Send_Byte(dev+1);  //进入接收模式	
	IIC_Wait_Ack();

	for(count=0;count<length;count++){

		if(count!=length-1)data[count]=IIC_Read_Byte(1);  //带ACK的读数据
		else  data[count]=IIC_Read_Byte(0);	 //最后一个字节NACK
	}
	IIC_Stop();//产生一个停止条件
	return count;
}

/**************************实现函数********************************************
 *函数原型:		u8 IICwriteBytes(u8 dev, u8 reg, u8 length, u8* data)
 *功　　能:	    将多个字节写入指定设备 指定寄存器
 输入	dev  目标设备地址
 reg	  寄存器地址
 length 要写的字节数
 *data  将要写的数据的首地址
 返回   返回是否成功
 *******************************************************************************/ 
u8 IICwriteBytes(u8 dev, u8 reg, u8 length, u8* data){

	u8 count = 0;
	IIC_Start();
	IIC_Send_Byte(dev);	   //发送写命令
	IIC_Wait_Ack();
	IIC_Send_Byte(reg);   //发送地址
	IIC_Wait_Ack();	  
	for(count=0;count<length;count++){
		IIC_Send_Byte(data[count]); 
		IIC_Wait_Ack(); 
	}
	IIC_Stop();//产生一个停止条件

	return 1; //status == 0;
}

/**************************实现函数********************************************
 *函数原型:		u8 IICreadByte(u8 dev, u8 reg, u8 *data)
 *功　　能:	    读取指定设备 指定寄存器的一个值
 输入	dev  目标设备地址
 reg	   寄存器地址
 *data  读出的数据将要存放的地址
 返回   1
 *******************************************************************************/ 
u8 IICreadByte(u8 dev, u8 reg, u8 *data){
	*data=I2C_ReadOneByte(dev, reg);
	return 1;
}

/**************************实现函数********************************************
 *函数原型:		unsigned char IICwriteByte(unsigned char dev, unsigned char reg, unsigned char data)
 *功　　能:	    写入指定设备 指定寄存器一个字节
 输入	dev  目标设备地址
 reg	   寄存器地址
 data  将要写入的字节
 返回   1
 *******************************************************************************/ 
static unsigned char IICwriteByte(unsigned char dev, unsigned char reg, unsigned char data){
	return IICwriteBytes(dev, reg, 1, &data);
}

/**************************实现函数********************************************
 *函数原型:		u8 IICwriteBits(u8 dev,u8 reg,u8 bitStart,u8 length,u8 data)
 *功　　能:	    读 修改 写 指定设备 指定寄存器一个字节 中的多个位
 输入	dev  目标设备地址
 reg	   寄存器地址
 bitStart  目标字节的起始位
 length   位长度
 data    存放改变目标字节位的值
 返回   成功 为1 
 失败为0
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

/**************************实现函数********************************************
 *函数原型:		u8 IICwriteBit(u8 dev, u8 reg, u8 bitNum, u8 data)
 *功　　能:	    读 修改 写 指定设备 指定寄存器一个字节 中的1个位
 输入	dev  目标设备地址
 reg	   寄存器地址
 bitNum  要修改目标字节的bitNum位
 data  为0 时，目标位将被清0 否则将被置位
 返回   成功 为1 
 失败为0
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
	I2C_AcknowledgeConfig(I2C1,ENABLE); //使能应答
	I2C_GenerateSTART(I2C1,ENABLE);  //发送一个开始位
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT)){;}//等待EV5
	I2C_Send7bitAddress(I2C1,0x39,I2C_Direction_Transmitter); //发送从地址
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){;} //等待EV6
	I2C_SendData(I2C1,addr); //发送要写入数据的地址
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)){;} //等待EV8
	I2C_SendData(I2C1,data); //发送要写入的数据
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)){;} //等待EV8
	I2C_GenerateSTOP(I2C1,ENABLE); //发送停止位
}

u8 I2C_Read(u8 nAddr)
{
	I2C_AcknowledgeConfig(I2C1,ENABLE); //使能应答
	I2C_GenerateSTART(I2C1,ENABLE); //发送一个开始位
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT)){;} //等待EV5
	I2C_Send7bitAddress(I2C1,0x39,I2C_Direction_Transmitter); //发送一个伪写指令 
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){;}//等待EV6
	I2C_SendData(I2C1,nAddr);//发送读地址   
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)){;} //等待EV8

	I2C_GenerateSTART(I2C1,ENABLE); //发送一个开始位
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT)){;} //等待EV5
	I2C_Send7bitAddress(I2C1,0x39,I2C_Direction_Receiver); //发送一个读指令
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)){;} //等待EV6
	I2C_AcknowledgeConfig(I2C1,DISABLE);  //应答使能关闭
	I2C_GenerateSTOP(I2C1,ENABLE);   //发送一个停止位
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED)){;} //等待EV7
	return I2C_ReceiveData(I2C1);  //返回读到的数据
}

#else


/* IOI2C.c file
   编写者：lisn3188
   网址：www.chiplab7.com
   作者E-mail：lisn3188@163.com
   编译环境：MDK-Lite  Version: 4.23
   初版时间: 2012-06-25
   测试： 本程序已在第七实验室的[Captain 飞控板]上完成测试

   占用资源：
   1. I2C 接口 PB6  PB7 引脚

   功能：
   提供I2C接口操作API 。
   使用IO模拟方式
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



//IO方向设置
#define SDA_IN()  SDAPortIN()
#define SDA_OUT() SDAPortOUT();



#define SDA_H  GPIO_SetBits(SDA_PORT, SDA_PORT_PIN)
#define SDA_L  GPIO_ResetBits(SDA_PORT, SDA_PORT_PIN)

#define SCL_H  GPIO_SetBits(SCL_PORT, SCL_PORT_PIN)
#define SCL_L  GPIO_ResetBits(SCL_PORT, SCL_PORT_PIN)

#define READ_SDA GPIO_ReadInputDataBit(SDA_PORT, SDA_PORT_PIN)

unsigned short I2C_Erorr_Count = 0;

/**************************实现函数********************************************
 *函数原型:		void IIC_Init(void)
 *功　　能:		初始化I2C对应的接口引脚。
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



/**************************实现函数********************************************
 *函数原型:		void IIC_Start(void)
 *功　　能:		产生IIC起始信号
 *******************************************************************************/
static void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	SDA_H;//IIC_SDA=1;	  	  
	SCL_H;//IIC_SCL=1;
	delay_us(2);
	SDA_L;//IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(2);
	SCL_L;//IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}

/**************************实现函数********************************************
 *函数原型:		void IIC_Stop(void)
 *功　　能:	    //产生IIC停止信号
 *******************************************************************************/	  
static void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	SCL_L;//IIC_SCL=0;
	SDA_L;//IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
	delay_us(2);
	SCL_H;//IIC_SCL=1; 
	SDA_H;//IIC_SDA=1;//发送I2C总线结束信号
	delay_us(2);							   	
}

/**************************实现函数********************************************
 *函数原型:		u8 IIC_Wait_Ack(void)
 *功　　能:	    等待应答信号到来 
//返回值：1，接收应答失败
//        0，接收应答成功
 *******************************************************************************/
static u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
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
			I2C_Erorr_Count ++;	 //I2C 错误
			return 1;
		}
		delay_us(1);
#endif
	}
	SCL_L;//IIC_SCL=0;//时钟输出0 	   
	return 0;  
} 

/**************************实现函数********************************************
 *函数原型:		void IIC_Ack(void)
 *功　　能:	    产生ACK应答
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

/**************************实现函数********************************************
 *函数原型:		void IIC_NAck(void)
 *功　　能:	    产生NACK应答
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

/**************************实现函数********************************************
 *函数原型:		void IIC_Send_Byte(u8 txd)
 *功　　能:	    IIC发送一个字节
 *******************************************************************************/		  
static void IIC_Send_Byte(u8 txd)
{                        
	u8 t;   
	SDA_OUT(); 	    
	SCL_L;//IIC_SCL=0;//拉低时钟开始数据传输
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

/**************************实现函数********************************************
 *函数原型:		u8 IIC_Read_Byte(unsigned char ack)
 *功　　能:	    //读1个字节，ack=1时，发送ACK，ack=0，发送nACK 
 *******************************************************************************/  
static u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
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
		IIC_Ack(); //发送ACK 
	else
		IIC_NAck();//发送nACK  
	return receive;
}	 				     


/**************************实现函数********************************************
 *函数原型:		unsigned char I2C_ReadOneByte(unsigned char I2C_Addr,unsigned char addr)
 *功　　能:	    读取指定设备 指定寄存器的一个值
 输入	I2C_Addr  目标设备地址
 addr	   寄存器地址
 返回   读出来的值
 *******************************************************************************/ 
static unsigned char I2C_ReadOneByte(unsigned char I2C_Addr,unsigned char addr)
{
	unsigned char res=0;

	IIC_Start();	
	IIC_Send_Byte(I2C_Addr);	   //发送写命令
	res++;
	IIC_Wait_Ack();
	IIC_Send_Byte(addr); res++;  //发送地址
	IIC_Wait_Ack();	  
	//IIC_Stop();//产生一个停止条件	
	IIC_Start();
	IIC_Send_Byte(I2C_Addr+1); res++;          //进入接收模式			   
	IIC_Wait_Ack();
	res=IIC_Read_Byte(0);	   
	IIC_Stop();//产生一个停止条件

	return res;
}


/**************************实现函数********************************************
 *函数原型:		u8 IICreadBytes(u8 dev, u8 reg, u8 length, u8 *data)
 *功　　能:	    读取指定设备 指定寄存器的 length个值
 输入	dev  目标设备地址
 reg	  寄存器地址
 length 要读的字节数
 *data  读出的数据将要存放的指针
 返回   读出来的字节数量
 *******************************************************************************/ 
u8 IICreadBytes(u8 dev, u8 reg, u8 length, u8 *data){
	u8 count = 0;

	IIC_Start();
	IIC_Send_Byte(dev);	   //发送写命令
	IIC_Wait_Ack();
	IIC_Send_Byte(reg);   //发送地址
	IIC_Wait_Ack();	  
	IIC_Start();
	IIC_Send_Byte(dev+1);  //进入接收模式	
	IIC_Wait_Ack();

	for(count=0;count<length;count++){

		if(count!=length-1)data[count]=IIC_Read_Byte(1);  //带ACK的读数据
		else  data[count]=IIC_Read_Byte(0);	 //最后一个字节NACK
	}
	IIC_Stop();//产生一个停止条件
	return count;
}

/**************************实现函数********************************************
 *函数原型:		u8 IICwriteBytes(u8 dev, u8 reg, u8 length, u8* data)
 *功　　能:	    将多个字节写入指定设备 指定寄存器
 输入	dev  目标设备地址
 reg	  寄存器地址
 length 要写的字节数
 *data  将要写的数据的首地址
 返回   返回是否成功
 *******************************************************************************/ 
u8 IICwriteBytes(u8 dev, u8 reg, u8 length, u8* data){

	u8 count = 0;
	IIC_Start();
	IIC_Send_Byte(dev);	   //发送写命令
	IIC_Wait_Ack();
	IIC_Send_Byte(reg);   //发送地址
	IIC_Wait_Ack();	  
	for(count=0;count<length;count++){
		IIC_Send_Byte(data[count]); 
		IIC_Wait_Ack(); 
	}
	IIC_Stop();//产生一个停止条件

	return 1; //status == 0;
}

/**************************实现函数********************************************
 *函数原型:		u8 IICreadByte(u8 dev, u8 reg, u8 *data)
 *功　　能:	    读取指定设备 指定寄存器的一个值
 输入	dev  目标设备地址
 reg	   寄存器地址
 *data  读出的数据将要存放的地址
 返回   1
 *******************************************************************************/ 
u8 IICreadByte(u8 dev, u8 reg, u8 *data){
	*data=I2C_ReadOneByte(dev, reg);
	return 1;
}

/**************************实现函数********************************************
 *函数原型:		unsigned char IICwriteByte(unsigned char dev, unsigned char reg, unsigned char data)
 *功　　能:	    写入指定设备 指定寄存器一个字节
 输入	dev  目标设备地址
 reg	   寄存器地址
 data  将要写入的字节
 返回   1
 *******************************************************************************/ 
unsigned char IICwriteByte(unsigned char dev, unsigned char reg, unsigned char data){
	return IICwriteBytes(dev, reg, 1, &data);
}

/**************************实现函数********************************************
 *函数原型:		u8 IICwriteBits(u8 dev,u8 reg,u8 bitStart,u8 length,u8 data)
 *功　　能:	    读 修改 写 指定设备 指定寄存器一个字节 中的多个位
 输入	dev  目标设备地址
 reg	   寄存器地址
 bitStart  目标字节的起始位
 length   位长度
 data    存放改变目标字节位的值
 返回   成功 为1 
 失败为0
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

/**************************实现函数********************************************
 *函数原型:		u8 IICwriteBit(u8 dev, u8 reg, u8 bitNum, u8 data)
 *功　　能:	    读 修改 写 指定设备 指定寄存器一个字节 中的1个位
 输入	dev  目标设备地址
 reg	   寄存器地址
 bitNum  要修改目标字节的bitNum位
 data  为0 时，目标位将被清0 否则将被置位
 返回   成功 为1 
 失败为0
 *******************************************************************************/ 
static u8 IICwriteBit(u8 dev, u8 reg, u8 bitNum, u8 data){
	u8 b;
	IICreadByte(dev, reg, &b);
	b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
	return IICwriteByte(dev, reg, b);
}

//------------------End of File----------------------------


#endif