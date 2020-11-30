//#ifndef __flash_opt__
//#define __flash_opt__

//#include <rtthread.h>
//#include <stm32l1xx_flash.h>

//#if defined (STM32F10X_MD) || defined (STM32F10X_MD_VL)
// #define PAGE_SIZE                         (0x400)    /* 1 Kbyte */
// #define FLASH_SIZE                        (0x20000)  /* 128 KBytes */
//#elif defined STM32F10X_CL
// #define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
// #define FLASH_SIZE                        (0x40000)  /* 256 KBytes */
//#elif defined STM32F10X_HD
// #define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
// #define FLASH_SIZE                        (0x80000)  /* 512 KBytes */
//#elif defined STM32F10X_XL
// #define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
// #define FLASH_SIZE                        (0x100000) /* 1 MByte */
//#else 
// #error "Please select first the STM32 device"    
//#endif







//int __write_flash(u32 StartAddr,u16 *buf,u16 len);
//int __read_flash(u32 StartAddr,u16 *buf,u16 len);

//#endif





//-------------------------


  #ifndef __FLASH_H
#define __FLASH_H


#define ABS_RETURN(x,y)         (x < y) ? (y-x) : (x-y)

#define FLASH_PAGE_SIZE               0x100     /* 256 Bytes */
#define FLASH_HALFPAGE_SIZE						0x80    	/* 128 Bytes */

/* define the address from where user application will be loaded,
   the application address should be a start sector address */
#define APPLICATION_ADDRESS   (uint32_t)0x08000000 + 1024*100

/* define the address from where IAP will be loaded, 0x08000000:BANK1 or 
   0x08030000:BANK2 */
#define FLASH_START_ADDRESS   (uint32_t)0x08000000

/* Get the number of sectors from where the user program will be loaded */
#define FLASH_SECTOR_NUMBER  ((uint32_t)(ABS_RETURN(APPLICATION_ADDRESS,FLASH_START_ADDRESS))>>12)

#ifdef STM32L1XX_MD 
 //#define USER_FLASH_LAST_PAGE_ADDRESS  0x0801FF00
 #define USER_FLASH_LAST_PAGE_ADDRESS  0x0800aF00
  #define USER_FLASH_END_ADDRESS        0x0801FFFF
  /* Compute the mask to test if the Flash memory, where the user program will be
  loaded, is write protected */
  #define  FLASH_PROTECTED_SECTORS   ((uint32_t)~((1 << FLASH_SECTOR_NUMBER) - 1))
#elif defined STM32L1XX_HD
  #define USER_FLASH_LAST_PAGE_ADDRESS  0x0805FF00
  /* define the address from where user application will be loaded,
   the application address should be a start sector address */
  #define USER_FLASH_END_ADDRESS        0x08080000
  
#else
 #error "Please select first the STM32 device to be used (in stm32l1xx.h)"    
#endif 










#define    SrcMACID           0x06
#define	   DestMACID          0x07
#define	   CMDORSID           0x08 


#define	   CanSendTimeOut     216000    //3ms
#define	   FirstFrame               0

#define		INFO_LENGTH	       15
#define		ERASE_FLASH        0xc0
#define		DOWNLOAD           0xc1
#define		LED_ON             0xc2
#define		GET_HARDWARE_INFO  0xc3
#define		PROGRAM_RUN        0xc4
#define		ACK_NORMAL         0x06
#define		ACK_SUSPEND        0x07
#define		ACK_GOON           0x08
#define		ACK_ABNORMAL       0x15
#define		SEND_HARDWARE_INFO 0x9d
#define		SEND_TOTAL_CHECK   0x9e

 
/**
  * @brief  Jump to user application 
  * @param ApplicationAddress:	the ApplicationAddress is where user code start
  * @retval None.
  */
void Jump_ToUserCode(uint32_t UserAddress);
uint8_t   Program32Bytes_Flash(uint32_t Address,uint8_t *buf);
/* Exported functions ------------------------------------------------------- */
void FLASH_If_Init(void);
uint32_t FLASH_If_Erase(uint32_t StartSector);
uint32_t FLASH_If_DisableWriteProtection(void);
uint32_t FLASH_If_GetWriteProtectionStatus(void);
FLASH_Status FLASH_If_WriteProtectionConfig( FunctionalState able_disable );
uint32_t FLASH_If_Write(__IO uint32_t* FlashAddress, uint32_t* Data ,uint16_t DataLength);
uint32_t FLASH_If_WriteProtection(void);
uint32_t FLASH_If_ErasePage(uint32_t page);


typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

void FLASH_ProgramStart(u32 addr , u32 size);
void FLASH_AppendOneByte(u8 Data);
void FLASH_AppendBuffer(u8 *Data , u32 size);
void FLASH_AppendEnd(void);
void FLASH_ProgramDone(void);


#endif 


