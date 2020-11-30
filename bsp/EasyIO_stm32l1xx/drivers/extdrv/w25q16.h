#ifndef __w25q16__
#define __w25q16__ 

#include "stm32l1xx.h"


#define Power_Down                0xB9       //????
#define ReleacePowerDown          0xAB       //??????



#if 0

//W25Q64BV
#define W25Q64_DeviceID 0xEF16
 
#define W25X_CMD_WriteEnable 0x06 /*Write enable instruction */
#define W25X_CMD_WriteDisable 0x04 /*! Write to Memory Disable */
#define W25X_CMD_WriteStatusReg 0x01 /* Write Status Register instruction */
 
#define W25X_CMD_PageProgram 0x02 /* Write enable instruction */
#define W25X_CMD_QuadPageProgram 0x32 /* Write enable instruction */
 
#define W25X_CMD_BlockErase64 0xD8 /* Block 64k Erase instruction */
#define W25X_CMD_BlockErase32 0x52 /* Block 32k Erase instruction */
#define W25X_CMD_ChipErase 0xC7 /* Bulk Erase instruction */
#define W25X_CMD_SectorErase 0x20 /* Sector 4k Erase instruction */
#define W25X_CMD_EraseSuspend 0x75 /* Sector 4k Erase instruction */
#define W25X_CMD_EraseResume 0x7a /* Sector 4k Erase instruction */
 
#define W25X_CMD_ReadStatusReg1 0x05 /* Read Status Register instruction */
#define W25X_CMD_ReadStatusReg2 0x35 /* Read Status Register instruction */
 
#define W25X_CMD_High_Perform_Mode 0xa3
#define W25X_CMD_Conti_Read_Mode_Ret 0xff
 
#define W25X_WakeUp 0xAB
#define W25X_JedecDeviveID 0x9F /*Read identification */
#define W25X_ManufactDeviveID 0x90 /* Read identification */
#define W25X_ReadUniqueID 0x4B
 
#define W25X_Power_Down 0xB9 /*Sector 4k Erase instruction */
 
#define W25X_CMD_ReadData 0x03 /* Read from Memory instruction */
#define W25X_CMD_FastRead 0x0b /* Read from Memory instruction */
#define W25X_CMD_FastReadDualOut 0x3b /*Read from Memory instruction */
#define W25X_CMD_FastReadDualIO 0xBB /* Read from Memory instruction */
#define W25X_CMD_FastReadQuadOut 0x6b /* Read from Memory instruction */
#define W25X_CMD_FastReadQuadIO 0xeb /* Read from Memory instruction */
#define W25X_CMD_OctalWordRead 0xe3 /* Read from Memory instruction */
 
#define W25X_DUMMY_BYTE 0xff //0xA5
//#define W25X_SPI_PAGESIZE 0x100
#define W25X_SECTION_SIZE 4096



#define W25X_FLASH_SPI SPI3
#define W25X_FLASH_SPI_CLK RCC_APB1Periph_SPI3

#define W25X_FLASH_SPI_SCK_PIN GPIO_Pin_10 /* PC.10 */
#define W25X_FLASH_SPI_SCK_GPIO_PORT GPIOC /* GPIOC */
#define W25X_FLASH_SPI_SCK_GPIO_CLK RCC_AHBPeriph_GPIOC

#define W25X_FLASH_SPI_MISO_PIN GPIO_Pin_11 /* PB.11 */
#define W25X_FLASH_SPI_MISO_GPIO_PORT GPIOC /* GPIOC */
#define W25X_FLASH_SPI_MISO_GPIO_CLK RCC_AHBPeriph_GPIOC

#define W25X_FLASH_SPI_MOSI_PIN GPIO_Pin_12 /* PB.12 */
#define W25X_FLASH_SPI_MOSI_GPIO_PORT GPIOC /* GPIOC */
#define W25X_FLASH_SPI_MOSI_GPIO_CLK RCC_AHBPeriph_GPIOC

#define W25X_FLASH_CS_PIN GPIO_Pin_15 /* PA.15 */
#define W25X_FLASH_CS_GPIO_PORT GPIOA /* GPIOA */
#define W25X_FLASH_CS_GPIO_CLK RCC_AHBPeriph_GPIOA


void SPI_Flash_Init(void);
uint16_t SPI_Flash_ReadID(void);

void init_write_flash(unsigned int addr , unsigned char *spiflash_buffer);
void append_w25x_byte(uint32_t BaseAddr , unsigned char data);
void append_w25x_append_buffer(unsigned char *buffer , int length);
void w25x_append_end(void);

void SPI_Flash_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);


#define USE_STATIC_SPIFLASH_BUFFER

#endif

#define W25X_FLASH_SPI SPI3
#define W25X_FLASH_SPI_CLK RCC_APB1Periph_SPI3

#define W25X_FLASH_SPI_SCK_PIN GPIO_Pin_10 /* PC.10 */
#define W25X_FLASH_SPI_SCK_GPIO_PORT GPIOC /* GPIOC */
#define W25X_FLASH_SPI_SCK_GPIO_CLK RCC_AHBPeriph_GPIOC

#define W25X_FLASH_SPI_MISO_PIN GPIO_Pin_11 /* PB.11 */
#define W25X_FLASH_SPI_MISO_GPIO_PORT GPIOC /* GPIOC */
#define W25X_FLASH_SPI_MISO_GPIO_CLK RCC_AHBPeriph_GPIOC

#define W25X_FLASH_SPI_MOSI_PIN GPIO_Pin_12 /* PB.12 */
#define W25X_FLASH_SPI_MOSI_GPIO_PORT GPIOC /* GPIOC */
#define W25X_FLASH_SPI_MOSI_GPIO_CLK RCC_AHBPeriph_GPIOC

#define W25X_FLASH_CS_PIN GPIO_Pin_15 /* PA.15 */
#define W25X_FLASH_CS_GPIO_PORT GPIOA /* GPIOA */
#define W25X_FLASH_CS_GPIO_CLK RCC_AHBPeriph_GPIOA


//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//W25X16 ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/6/13 
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	 
#define W25Q80 	0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
 

extern unsigned short SPI_FLASH_TYPE;		//��������ʹ�õ�flashоƬ�ͺ�		

#define	SPI_FLASH_CS PAout(2)  //ѡ��FLASH					 
////////////////////////////////////////////////////////////////////////////

extern unsigned char SPI_FLASH_BUF[4096];
//W25X16��д
#define FLASH_ID 0XEF14
//ָ���
#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 

void SPI_Flash_Init(void);
unsigned short  SPI_Flash_ReadID(void);  	    //��ȡFLASH ID
unsigned char	 SPI_Flash_ReadSR(void);        //��ȡ״̬�Ĵ��� 
void SPI_FLASH_Write_SR(unsigned char sr);  	//д״̬�Ĵ���
void SPI_FLASH_Write_Enable(void);  //дʹ�� 
void SPI_FLASH_Write_Disable(void);	//д����
void SPI_Flash_Read(unsigned char* pBuffer,unsigned int ReadAddr,unsigned short NumByteToRead);   //��ȡflash
void SPI_Flash_Write(unsigned char* pBuffer,unsigned int WriteAddr,unsigned short NumByteToWrite);//д��flash
void SPI_Flash_Erase_Chip(void);    	  //��Ƭ����
void SPI_Flash_Erase_Sector(unsigned int Dst_Addr);//��������
void SPI_Flash_Wait_Busy(void);           //�ȴ�����
void SPI_Flash_PowerDown(void);           //�������ģʽ
void SPI_Flash_WAKEUP(void);			  //����


void init_write_flash(unsigned int addr , unsigned char *spiflash_buffer);
void append_w25x_byte(uint32_t BaseAddr , unsigned char data);
void append_w25x_append_buffer(unsigned char *buffer , int length);
void w25x_append_end(void);


void spiflash_powerdown(void);
void spiflash_release_powerdown(void);


#endif


