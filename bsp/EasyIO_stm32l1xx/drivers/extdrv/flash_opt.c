//#include "flash_opt.h"
//#include <stm32l1xx_flash.h>

//#define FALSE 0
//#define TRUE 1

//static u32 ApplicationAddress = 0x801C000;      //APP程序首地址
//static u32 ApplicationSize		= 1024*200;          //程序预留空间

//typedef unsigned int vu32;
//typedef unsigned short vu16;
////#if 0
////  [..] The FLASH Memory Programming functions, includes the following functions:
////    (+) void FLASH_Unlock(void);
////    (+) void FLASH_Lock(void);
////    (+) FLASH_Status FLASH_ErasePage(uint32_t Page_Address);
////    (+) FLASH_Status FLASH_FastProgramWord(uint32_t Address, uint32_t Data);
////   
////    [..] Any operation of erase or program should follow these steps:
////    (#) Call the FLASH_Unlock() function to enable the flash control register and 
////        program memory access.
////    (#) Call the desired function to erase page or program data.
////    (#) Call the FLASH_Lock() to disable the flash program memory access 
////       (recommended to protect the FLASH memory against possible unwanted operation).

////@endverbatim
//		
//		



///*
//********************************************************************************
//*                           
//*                              程序下载部分
//*
//* File          : Download.C
//* Version       : V1.0
//* By            : whq
//*
//* For           : Stm32f10x
//* Mode          : Thumb2
//* Toolchain     : 
//*                   RealView Microcontroller Development Kit (MDK)
//*                   Keil uVision
//* Description   : 
//*
//*
//* Date          : 2013.2.20
//*******************************************************************************/


//#include "stm32l1xx_flash.h"


////static u32 m_EraseCounter = 0;


///*******************************************************************************
//* Function Name :unsigned int FLASH_PagesMask(volatile unsigned int Size)
//* Description   :计算所要擦除 的页数
//* Input         :
//* Output        :
//* Other         :
//* Date          :2013.02.24
//*******************************************************************************/
//static u32 FLASH_PagesMask(unsigned int Size)
//{
//    u32 pagenumber = 0x0;
//    u32 size = Size;

//    if ((size % PAGE_SIZE) != 0)
//    {
//        pagenumber = (size / PAGE_SIZE) + 1;
//    }
//    else
//    {
//        pagenumber = size / PAGE_SIZE;
//    }
//    return pagenumber;
//}


///*******************************************************************************
//* Function Name :u32 FLASH_WriteBank(u32 *pData, u32 addr, u32 size)
//* Description   :写入一块数据
//* Input         :pData:数据；addr:数据的地址；size:长度
//* Output        :TRUE:成功，FALSE:失败。
//* Other         :
//* Date          :2013.02.24
//*******************************************************************************/
//u32 FLASH_WriteBank(u8 *pData, u32 addr, u16 size)
//{
//	
//		FLASH_Status status;
//    vu32 *pDataTemp = (vu32 *)pData;
//    vu32 temp = addr;
////    FLASH_Status FLASHStatus = FLASH_COMPLETE;
////    u32 NbrOfPage = 0;
////
////    NbrOfPage = FLASH_PagesMask(addr + size - ApplicationAddress);
////    for (; (m_EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); m_EraseCounter++)
////    {
////        FLASHStatus = FLASH_ErasePage(ApplicationAddress + (PAGE_SIZE * m_EraseCounter));
////    }

//    for (; temp < (addr + size); pDataTemp++, temp += 4)
//    {
//				//写数据数据
//			
//				status = FLASH_FastProgramWord(temp,*pDataTemp);
//			
//				status = 0;
//			
//        //DATA_EEPROM_ProgramHalfWord(temp, *pDataTemp);
//        if (*pDataTemp != *(vu32 *)temp)
//        {
//            return FALSE;
//        }
//    }

//    return TRUE;
//}


//static u8 buffer[4];
//static u8 buffer_index = 0;
//static u32 buffer_addr = 0;
//u32 FLASH_AppendOneByte(u8 Data)
//{
//	buffer[buffer_index++] = Data;
//	if (buffer_index >= sizeof(buffer))
//	{
//		FLASH_WriteBank(buffer,buffer_addr,sizeof(buffer));
//		buffer_addr += sizeof(buffer);
//		buffer_index = 0;
//	}
//	//
//}

//u32 FLASH_AppendBuffer(u8 *Data , u32 size)
//{
//	u32 i=0;
//	for(i=0;i<size;i++)
//	{
//		FLASH_AppendOneByte(Data[i]);
//	}
//	
//	//
//}

//void FLASH_AppendEnd(void)
//{
//	if (buffer_index > 0)
//	{
//		FLASH_WriteBank(buffer,buffer_addr,sizeof(buffer));
//		buffer_addr += sizeof(buffer);
//	}
//	//
//}
///*******************************************************************************
//* Function Name :void FLASH_ProgramDone(void)
//* Description   :烧写启动
//* Input         :
//* Output        :
//* Other         :
//* Date          :2013.02.24
//*******************************************************************************/


//void FLASH_If_Init(void)
//{ 
//  /* Unlock the Program memory */
//  FLASH_Unlock();

//  /* Clear all FLASH flags */  
//  FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
//                  | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);   
//}




//void FLASH_ProgramStart(u32 addr , u32 size)
//{
//    static FLASH_Status FLASHStatus = FLASH_COMPLETE;
//    u32 NbrOfPage = 0;
//    vu32 EraseCounter = 0;
//		
//    
//		FLASH_OB_Unlock();
//	
//		FLASHStatus = FLASH_OB_WRPConfig(OB_WRP_AllPages,DISABLE);
//		rt_kprintf("FLASHStatus %d\r\n",FLASHStatus);
//		FLASHStatus = FLASH_OB_WRP1Config(OB_WRP1_AllPages,DISABLE);
//		rt_kprintf("FLASHStatus %d\r\n",FLASHStatus);
//		FLASHStatus = FLASH_OB_WRP2Config(OB_WRP2_AllPages,DISABLE);
//		rt_kprintf("FLASHStatus %d\r\n",FLASHStatus);
//	
//		//FLASH_OB_Launch();
//		FLASH_OB_Lock();
//		FLASH_Unlock();
////		for(;;)
////		{
////			FLASHStatus = FLASH_GetStatus();
////		}
//	
////    m_EraseCounter = 0;
//	
//		ApplicationAddress = addr;
//		ApplicationSize = size;
//	
//		buffer_index = 0;
//		buffer_addr = ApplicationAddress;

//    NbrOfPage = FLASH_PagesMask(ApplicationSize);
//	
//		FLASHStatus = FLASH_ErasePage(ApplicationAddress);
//	
//		return ;
//    for (; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
//    {
//        FLASHStatus = FLASH_ErasePage(ApplicationAddress + (PAGE_SIZE * EraseCounter));
//				FLASHStatus = 0;
//    }
//}


///*******************************************************************************
//* Function Name :void FLASH_ProgramDone(void)
//* Description   :烧写结束
//* Input         :
//* Output        :
//* Other         :
//* Date          :2013.02.24
//*******************************************************************************/
//void FLASH_ProgramDone(void)
//{
//    FLASH_Lock();
//		FLASH_OB_Unlock();
//		FLASH_OB_WRPConfig(OB_WRP_AllPages,ENABLE);
//		FLASH_OB_WRP1Config(OB_WRP1_AllPages,ENABLE);
//		FLASH_OB_WRP2Config(OB_WRP2_AllPages,ENABLE);
//		FLASH_OB_Lock();
//}



//#define EEPROM_BASE_ADDR	0x08080000	
//#define EEPROM_BYTE_SIZE	0x0FFF

//#define PEKEY1	0x89ABCDEF		//FLASH_PEKEYR
//#define PEKEY2	0x02030405		//FLASH_PEKEYR

//#define EN_INT      	__enable_irq();		//系统开全局中断
//#define DIS_INT     	__disable_irq();	//系统关全局中断

//typedef unsigned int uint32;
//typedef unsigned short uint16;
//typedef unsigned char uint8;


///*------------------------------------------------------------
// Func: EEPROM数据按字节写入
// Note:
//-------------------------------------------------------------*/

//#if 0
//#define FLASH_PDKEY1               ((uint32_t)0x04152637) /*!< Flash power down key1 */
//#define FLASH_PDKEY2               ((uint32_t)0xFAFBFCFD) /*!< Flash power down key2: used with FLASH_PDKEY1 
//                                                              to unlock the RUN_PD bit in FLASH_ACR */

//#define FLASH_PEKEY1               ((uint32_t)0x89ABCDEF) /*!< Flash program erase key1 */
//#define FLASH_PEKEY2               ((uint32_t)0x02030405) /*!< Flash program erase key: used with FLASH_PEKEY2
//                                                               to unlock the write access to the FLASH_PECR register and
//                                                               data EEPROM */

//#define FLASH_PRGKEY1              ((uint32_t)0x8C9DAEBF) /*!< Flash program memory key1 */
//#define FLASH_PRGKEY2              ((uint32_t)0x13141516) /*!< Flash program memory key2: used with FLASH_PRGKEY2
//                                                               to unlock the program memory */

//#define FLASH_OPTKEY1              ((uint32_t)0xFBEAD9C8) /*!< Flash option key1 */
//#define FLASH_OPTKEY2              ((uint32_t)0x24252627) /*!< Flash option key2: used with FLASH_OPTKEY1 to
//                                                              unlock the write access to the option byte block */
//#endif
//void EEPROM_WriteBytes(uint32 Addr,uint8 *Buffer,uint16 Length)
//{
//	uint8 *wAddr;
//	wAddr=(uint8 *)(Addr);
//	DIS_INT
//	FLASH->PEKEYR=PEKEY1;				//unlock
//	FLASH->PEKEYR=PEKEY2;
//	while(FLASH->PECR&FLASH_PECR_PELOCK);
//	FLASH->PECR|=FLASH_PECR_FTDW;		//not fast write
//	while(Length--){
//		*wAddr++=*Buffer++;
//		while(FLASH->SR&FLASH_SR_BSY);
//	}
//	FLASH->PECR|=FLASH_PECR_PELOCK;
//	EN_INT
//}

///*------------------------------------------------------------
// Func: EEPROM数据按字写入
// Note: 字当半字用
//-------------------------------------------------------------*/
//void EEPROM_WriteWords(uint32 Addr,uint16 *Buffer,uint16 Length)
//{
//	uint32 *wAddr;
//	wAddr=(uint32 *)(Addr);
//	DIS_INT
//	FLASH->PEKEYR=PEKEY1;				//unlock
//	FLASH->PEKEYR=PEKEY2;
//	while(FLASH->PECR&FLASH_PECR_PELOCK);
//	FLASH->PECR|=FLASH_PECR_FTDW;		//not fast write
//	while(Length--){
//		*wAddr++=*Buffer++;
//		while(FLASH->SR&FLASH_SR_BSY);
//	}
//	FLASH->PECR|=FLASH_PECR_PELOCK;
//	EN_INT
//}


//void FLASH_ProgramStart(u32 addr , u32 size);
//u32 FLASH_AppendOneByte(u8 Data);
//u32 FLASH_AppendBuffer(u8 *Data , u32 size);
//void FLASH_AppendEnd(void);
//u32 FLASH_WriteBank(u8 *pData, u32 addr, u16 size);
//void FLASH_ProgramDone(void);

//#if 0
//FLASH_ProgramStart(CONFIG_ADDR,CONFIG_SIZE);	\
//		FLASH_AppendBuffer((u8*)(&fwconfig),sizeof(struct FLASH_CONFIG_DATA));	\
//		FLASH_AppendEnd();	\
//		FLASH_ProgramDone();
//#endif

//void TEST_STM32L_FLASH(void)
//{
//	static unsigned int tmp;
//	FLASH_Status status;
//	rt_kprintf("FLASH_BODY %08x\r\n",*(u32*)ApplicationAddress);
//	tmp = *(u32*)ApplicationAddress;
//	tmp++;
//	FLASH_ProgramStart(ApplicationAddress,4);
//	FLASH_AppendBuffer((unsigned char*)(&tmp),4);
//	FLASH_AppendEnd();
//	FLASH_ProgramDone();
//	tmp = *(u32*)ApplicationAddress;
//	rt_kprintf("FLAS2_BODY %08x\r\n",*(u32*)ApplicationAddress);
//	//
//}




///********************************* END ****************************************/




////--------------------------------------------------------------------------------

//#define printf(...)

//int __write_flash(u32 StartAddr,u16 *buf,u16 len)
//{
////        volatile FLASH_Status FLASHStatus;
////        u32 FlashAddr;
////        len=(len+1)/2;
////        FLASH_Unlock();
////        FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
////        FlashAddr=StartAddr;
////        FLASH_ErasePage(StartAddr);
////        while(len--){
////                FLASHStatus = FLASH_ProgramHalfWord(FlashAddr,*buf++);
////                if (FLASHStatus != FLASH_COMPLETE){
////                        printf("FLSH :Error %08X\n\r",FLASHStatus);
////                        return -1;
////                }
////                FlashAddr += 2;
////        }
////        FLASH_Lock();
//        return 0;
//}
//int __read_flash(u32 StartAddr,u16 *buf,u16 len)
//{
////        u16 *p=(u16 *)StartAddr;
////        len=(len+1)/2;
////        while(len--){
////                *buf++=*p++;
////        }
//        return 0;
//}






















//============================================



#include "stm32l1xx.h"
#include "flash_opt.h"
//#include "com.h"
//#include "bsp.h"





typedef  void (*pFunction)(void);
pFunction Jump_To_Application;

void hexTOascii(uint32_t val, uint8_t *ptr,uint8_t len) 
{
	uint8_t  i;
	for ( i =len-1; i > 0; i-- )  
    {		//val低位在前
		ptr[i-1] = val % 10+0x30;  
		val = val / 10;  
	}  
	ptr[len-1]=0;
}



   /**
  * @brief  Jump to user application 
  * @param ApplicationAddress:	the ApplicationAddress is where user code start
  * @retval None.
  */
 void Jump_ToUserCode(uint32_t UserAddress)
 {	 	uint32_t JumpAddress;
         pFunction Jump_To_Application; 
		 

		   if (((*(__IO uint32_t*)UserAddress) & 0x2FFE0000 ) == 0x20000000)
			{ 
			     // Jump to user application 
			      JumpAddress = *(__IO uint32_t*) (UserAddress + 4);
			      Jump_To_Application = (pFunction) JumpAddress;
			      // Initialize user application's Stack Pointer 
			      __set_MSP(*(__IO uint32_t*) UserAddress);
			      Jump_To_Application();
			   
			}
}
   /**
  * @brief  Unlocks Flash for write access
  * @param  None
  * @retval None
  */
void FLASH_If_Init(void)
{ 
  /* Unlock the Program memory */
  FLASH_Unlock();

  /* Clear all FLASH flags */  
  FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                  | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);   
}
/**
  * @brief  This function does an erase of all user flash area
  * @param  StartSector: start of user flash area
  * @retval 0: user flash area successfully erased
  *         1: error occurred
  */
uint32_t FLASH_If_Erase(uint32_t StartSector)
{
  uint32_t flashaddress;

  flashaddress = StartSector;
  
 // while (flashaddress <= (uint32_t) USER_FLASH_LAST_PAGE_ADDRESS)
  while (flashaddress <= (uint32_t) USER_FLASH_LAST_PAGE_ADDRESS)
  {
    if (FLASH_ErasePage(flashaddress) == FLASH_COMPLETE)
    {
      flashaddress += FLASH_PAGE_SIZE;
    }
    else
    {
      /* Error occurred while page erase */
      return (1);
    }
  }
  return (0);
}





FLASH_Status FLASH1_ErasePage(uint32_t Page_Address)
{
  FLASH_Status status = FLASH_COMPLETE;
static uint8_t c=0,b=0;
  uint8_t dispNUM[20];
  
  /* Check the parameters */
  assert_param(IS_FLASH_PROGRAM_ADDRESS(Page_Address));
 
  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT);
  if( status != FLASH_COMPLETE)
    { b++;
     //DispChina("1",2,0);
    }
  if(status == FLASH_COMPLETE)
  {
    /* If the previous operation is completed, proceed to erase the page */

    /* Set the ERASE bit */
    FLASH->PECR |= FLASH_PECR_ERASE;

    /* Set PROG bit */
    FLASH->PECR |= FLASH_PECR_PROG;
  
    /* Write 00000000h to the first word of the program page to erase */
    *(__IO uint32_t *)Page_Address = 0x00000000;
 
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT);
    if( status!= FLASH_COMPLETE)
    {c++;
     //DispChina("2",2,0);
    }
    /* If the erase operation is completed, disable the ERASE and PROG bits */
    FLASH->PECR &= (uint32_t)(~FLASH_PECR_PROG);
    FLASH->PECR &= (uint32_t)(~FLASH_PECR_ERASE);   
  }    
  
  /* Return the Erase Status */
       hexTOascii(c, dispNUM,3);
	   //DispChina(dispNUM,2,0);
       hexTOascii(b, dispNUM,4);
	   //DispChina(dispNUM,2,10);
  return status;
}
uint32_t FLASH_If_ErasePage(uint32_t page)
{
  
  
   if (FLASH1_ErasePage(APPLICATION_ADDRESS+(page<<8)) == FLASH_COMPLETE)
    {
      
      FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                      | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);
      return (0);
    }
    else
    {
      
      return (1);
    }
  
   
}

/**
  * @brief  This function writes a data buffer in flash (data are 32-bit aligned).
  * @note   After writing data buffer, the flash content is checked.
  * @param  FlashAddress: start address for writing data buffer
  * @param  Data: pointer on data buffer
  * @param  DataLength: length of data buffer (unit is 32-bit word)   
  * @retval 0: Data successfully written to Flash memory
  *         1: Error occurred while writing data in Flash memory
  *         2: Written Data in flash memory is different from expected one
  */
uint32_t FLASH_If_Write(__IO uint32_t* FlashAddress, uint32_t* Data ,uint16_t DataLength)
{
  FLASH_Status status = FLASH_BUSY;
  uint32_t* malPointer = (uint32_t *)Data;
  uint32_t memBuffer[32]; /* Temporary buffer holding data that will be written in a half-page space */
  uint32_t* mempBuffer = memBuffer;
  uint32_t i = 0;

 
    
  while (malPointer < (uint32_t*)(Data + DataLength))
  {
    /* Fill with the received buffer */
    while (mempBuffer < (memBuffer + 32))
    {
      /* If there are still data available in the received buffer */
      if (malPointer < ((uint32_t *)Data + DataLength))
      {
        *(uint32_t *)(mempBuffer++) = *(uint32_t *)(malPointer++);
      }
      else /* no more data available in the received buffer: fill remaining with dummy 0 */
      {
        *(uint32_t *)(mempBuffer++) = 0;
      }
    }

    /* Write the buffer to the memory */
    status = FLASH_ProgramHalfPage(*FlashAddress, (uint32_t *)(memBuffer));
  
    if (status != FLASH_COMPLETE)
    {
      /* Error occurred while writing data in Flash memory */
      return (1);
    }

    /* Check if flash content matches memBuffer */
    for (i = 0; i < 32; i++)
    {
      if ((*(uint32_t*)(*(uint32_t*)FlashAddress + 4 * i)) != memBuffer[i])
      {
        /* flash content doesn't match memBuffer */
        return(2);
      }
    }

    /* Increment the memory pointer */
    *FlashAddress += 128;

    /* Reinitialize the intermediate buffer pointer */
    mempBuffer = memBuffer;
  }

  return (0);
}

/**
  * @brief  This function writes 32 bytes to flash 
  * @note   After writing data buffer, the flash content is checked.
  * @param  Address: start address for writing data buffer
  * @param  Data: pointer on data buffer
  * @retval 0: Data successfully written to Flash memory
  *         1: Error occurred while writing data in Flash memory
  *         2: Written Data in flash memory is different from expected one
  */
uint8_t   Program32Bytes_Flash(uint32_t Address,uint8_t *buf)
{
     uint32_t  i=0,RamSource=0;
     FLASH_Status status = FLASH_COMPLETE;
     __IO	uint32_t  addr=Address;
	 
      RamSource = (uint32_t)buf;
			
      for (i = 0;i < 32;i += 4)//32bytes
      {
        // Program the data received into STM32L Flash 
            status=  FLASH_FastProgramWord(addr, *(uint32_t*)RamSource);
	        if (status != FLASH_COMPLETE)
            {
                    /* Error occurred while writing data in Flash memory */
                  return (1);
            }	
            if (*(uint32_t*)addr != *(uint32_t*)RamSource)
            {
         	   return(2);
          	}
            addr += 4;
            RamSource += 4;
     }
		  return(0);
			   	  
}

uint8_t   Program4Bytes_Flash(uint32_t Address,uint8_t *buf)
{
     uint32_t  i=0,RamSource=0;
     FLASH_Status status = FLASH_COMPLETE;
     __IO	uint32_t  addr=Address;
	 
      RamSource = (uint32_t)buf;
			
      for (i = 0;i < 4;i += 4)//32bytes
      {
        // Program the data received into STM32L Flash 
            status=  FLASH_FastProgramWord(addr, *(uint32_t*)RamSource);
	        if (status != FLASH_COMPLETE)
            {
                    /* Error occurred while writing data in Flash memory */
							for(;;){};
                  return (1);
            }	
            if (*(uint32_t*)addr != *(uint32_t*)RamSource)
            {
         	   return(2);
          	}
            addr += 4;
            RamSource += 4;
     }
		  return(0);
			   	  
}

    /**
  * @brief  Disables the write protection of user flash area.
  * @param  None
  * @retval 0: Write Protection successfully disabled
  *         1: Error: Flash write unprotection failed
  *         2: Flash memory is not write protected
  */
uint32_t FLASH_If_DisableWriteProtection(void)
{
  FLASH_Status status = FLASH_BUSY;

  /* Clear all FLASH flags */  
  FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                  | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);  
  
  /* Test if user memory is write protected */
  if (FLASH_If_GetWriteProtectionStatus() != 0x00)
  {
    /* Unlock the Option Bytes */  
    FLASH_OB_Unlock();

    /* Disable the write protection of user application pages */ 
    status = FLASH_If_WriteProtectionConfig(DISABLE);
    if (status == FLASH_COMPLETE)
    {
      /* Write Protection successfully disabled */
      return (0);
    }
    else
    {
      /* Error: Flash write unprotection failed */
      return (1);
    }
  }
  else
  {
     /* Flash memory is not write protected */
     return(2);
  }
}

 /**
  * @brief  ENables the write protection of user flash area.
  * @param  None
  * @retval 0: Write Protection successfully ENabled
  *         1: Error: Flash write unprotection failed
  */
uint32_t FLASH_If_WriteProtection(void)
{
  FLASH_Status status = FLASH_BUSY;

  /* Clear all FLASH flags */  
  FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                  | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);  
  
  /* Unlock the Option Bytes */  
    FLASH_OB_Unlock();

    /* Disable the write protection of user application pages */ 
    status = FLASH_If_WriteProtectionConfig(ENABLE);
	
    if (status == FLASH_COMPLETE)
    {
      /* Write Protection successfully enabled */
      return (0);
    }
    else
    {
      /* Error: Flash write protection failed */
      return (1);
    }
  
}

/**
  * @brief  Returns the write protection status of user flash area.
  * @param  None
  * @retval If the sector is write-protected, the corresponding bit in returned
  *         value is set.
  *         If the sector isn't write-protected, the corresponding bit in returned
  *         value is reset.
  *         e.g. if only sector 3 is write-protected, returned value is 0x00000008
  */
uint32_t FLASH_If_GetWriteProtectionStatus(void)
{
#ifdef STM32L1XX_MD 
  return(FLASH_OB_GetWRP() & FLASH_PROTECTED_SECTORS);
#elif defined STM32L1XX_HD
  uint32_t WRP_Status = 0, WRP1_Status = 0, WRP2_Status = 0, sectornumber = 0;
  
  sectornumber=FLASH_SECTOR_NUMBER;
  /* Test on the start address in Bank1 */
  if (FLASH_START_ADDRESS <= 0x0802FF00)
  {
    /* Test on the user application to be programmed in Bank1 */
    if (APPLICATION_ADDRESS <= 0x0802FF00)
    {
      WRP2_Status = FLASH_OB_GetWRP2();
      
      if(sectornumber < 32)
      {
        WRP_Status = FLASH_OB_GetWRP() & ((uint32_t)~((1<<sectornumber) - 1));
        WRP1_Status = FLASH_OB_GetWRP1();
      }
      else
      {
        WRP1_Status = (FLASH_OB_GetWRP1() & ((uint32_t)~((1<<(sectornumber-32)) - 1)));
      }
    }
    else if (APPLICATION_ADDRESS >= 0x08030000) /* Test on the user application to be programmed in Bank2 */
    {
      sectornumber = (uint32_t)((APPLICATION_ADDRESS - 0x08030000)>>12);
      if (sectornumber < 16)
      {
        WRP1_Status = FLASH_OB_GetWRP1() & (uint32_t)~((1 << (sectornumber +16) - 1));
        WRP2_Status = FLASH_OB_GetWRP2();
      }
      else
      {
        WRP2_Status = FLASH_OB_GetWRP2() & (uint32_t)~((1 << (sectornumber-16) - 1));
      }
    }
  }
  else if (FLASH_START_ADDRESS >= 0x08030000) /* Test on the start address in Bank2 */
  {  
    if (APPLICATION_ADDRESS <= 0x0802FF00)  /* Test on the user application to be programmed in Bank1 */
    {
      sectornumber = (uint32_t)(APPLICATION_ADDRESS - 0x08000000)>>12;
      if(sectornumber < 32)
      {
        WRP_Status = FLASH_OB_GetWRP() & (uint32_t)~((1<<sectornumber - 1));
        WRP1_Status = FLASH_OB_GetWRP1()&0x0000FFFF;
      }
      else
      {
        WRP1_Status = (FLASH_OB_GetWRP1() & (uint32_t)~((1 << (sectornumber-32) - 1)))&0x0000FFFF;    
      }
    }
    else  /* Test on the user application to be programmed in Bank2 */
    {
      if(sectornumber < 16)
      {
        WRP1_Status = FLASH_OB_GetWRP1() & ((uint32_t)~(1 << (sectornumber + 16) - 1));
        WRP2_Status = FLASH_OB_GetWRP2();
      }
      else
      {
        WRP2_Status = (FLASH_OB_GetWRP2() & (uint32_t)~(1 <<( sectornumber - 16) - 1));
      }      
    }
  }
  if ((WRP_Status!=0)||(WRP1_Status!=0)||(WRP2_Status!=0))
    return 1;
  else
    return 0;

#endif   
}

/**
  * @brief  Disable the write protection status of user flash area.
  * @param  None
  * @retval If the sector is write-protected, the corresponding bit in returned
  *         value is set.
  *         If the sector isn't write-protected, the corresponding bit in returned
  *         value is reset.
  *         e.g. if only sector 3 is write-protected, returned value is 0x00000008
  */

 
FLASH_Status FLASH_If_WriteProtectionConfig(FunctionalState able_disable )
{
  FLASH_Status state = FLASH_COMPLETE;
#ifdef STM32L1XX_MD 
  
  /* Disable the write protection of user application pages */
  state = FLASH_OB_WRPConfig(FLASH_PROTECTED_SECTORS,able_disable ); 
#elif defined STM32L1XX_HD
  FLASH_Status WRPstatus = FLASH_COMPLETE, WRP1status = FLASH_COMPLETE, WRP2status = FLASH_COMPLETE ;
  uint32_t sectornumber = 0;
  
  sectornumber=FLASH_SECTOR_NUMBER;
  
  /* Test on the start address on Bank1 */
  if (FLASH_START_ADDRESS <= 0x0802FF00)
  {
    if (sectornumber < 32)
    {
      WRPstatus = FLASH_OB_WRPConfig(((uint32_t)~((1 << sectornumber) - 1)), able_disable );
      WRP1status = FLASH_OB_WRP1Config(OB_WRP1_AllPages, able_disable );
      WRP2status = FLASH_OB_WRP2Config(OB_WRP2_AllPages, able_disable );
    }
    else if ((sectornumber >= 32)&& (sectornumber < 64))
    {
      WRP1status = FLASH_OB_WRP1Config(((uint32_t)~((1 << (sectornumber - 32)) - 1)), able_disable );
      WRP2status = FLASH_OB_WRP2Config(OB_WRP2_AllPages, able_disable );
    }
    else 
      WRP2status = FLASH_OB_WRP2Config(((uint32_t)~((1 << (sectornumber - 64)) - 1)),able_disable );
  }
  else  if (FLASH_START_ADDRESS >= 0x08030000)/* Test on the start address on Bank2 */
  {
    if( APPLICATION_ADDRESS <=0x0802FF00)/* User application in BANK1 */
    {
      sectornumber = (uint32_t)((APPLICATION_ADDRESS-0x08000000)>>12);
      if(sectornumber < 32)
      {
        WRPstatus = FLASH_OB_WRPConfig((uint32_t)~((1 << sectornumber) - 1), able_disable );
        WRP1status = FLASH_OB_WRP1Config(0x0000FFFF,able_disable );
      }
      else
      { 
        WRP1status = FLASH_OB_WRP1Config((uint32_t)~((1 << (sectornumber-32)) - 1), able_disable );
      }
    }
    else /* User application Bank2 */
    {
      if(sectornumber < 16)
      {
        WRP1status = FLASH_OB_WRP1Config((uint32_t)~((1 << (sectornumber+16)) - 1), able_disable ); 
        WRP2status = FLASH_OB_WRP2Config(OB_WRP2_AllPages, able_disable );
      }
      else
      {
        WRP2status = FLASH_OB_WRP2Config((uint32_t)~((1 << (sectornumber -16 )) - 1), able_disable );
      }
    }

    if ((WRPstatus!=FLASH_COMPLETE)||(WRP1status!=FLASH_COMPLETE)||(WRP2status!=FLASH_COMPLETE))
      state = FLASH_ERROR_WRP;
  }
#endif   
  return state;
}





uint32_t flash_erase(uint32_t startaddr , uint32_t endaddr)
{
  uint32_t flashaddress;

  flashaddress = startaddr;
  
 // while (flashaddress <= (uint32_t) USER_FLASH_LAST_PAGE_ADDRESS)
  while (flashaddress <= (uint32_t) endaddr)
  {
    if (FLASH_ErasePage(flashaddress) == FLASH_COMPLETE)
    {
      flashaddress += FLASH_PAGE_SIZE;
    }
    else
    {
      /* Error occurred while page erase */
      return (1);
    }
  }
  return (0);
}

static u32 prog_strat_address = 0;
static u16 prog_write_cnt = 0;
static u16 prog_size = 0;
static unsigned char __write_flash_buffer[4];
static unsigned char __write_flash_buffer_index = 0;
void FLASH_ProgramStart(u32 addr , u32 size)
{
	uint32_t status;
	rt_kprintf("init size %d \r\n",size);
	
	prog_strat_address = addr;
	prog_size = size;
	prog_write_cnt = 0;
	__write_flash_buffer_index = 0;
	FLASH_If_Init();
	status=FLASH_If_DisableWriteProtection() ;
	flash_erase(addr,addr+size);
	//
}



void FLASH_AppendOneByte(u8 Data)
{
	

	__write_flash_buffer[__write_flash_buffer_index++] = Data;
	if (__write_flash_buffer_index >= 4)
	{
		Program4Bytes_Flash(prog_strat_address+(prog_write_cnt*4),__write_flash_buffer);
		__write_flash_buffer_index = 0;
		prog_write_cnt ++;
	}
	//
}

void FLASH_AppendBuffer(u8 *Data , u32 size)
{
	u32 i=0;
	rt_kprintf("append size %d \r\n",size);
	for(i=0;i<size;i++)
	{
		FLASH_AppendOneByte(Data[i]);
	}
	
	//
}

void FLASH_AppendEnd(void)
{
	if (__write_flash_buffer_index > 0)
	{
		Program4Bytes_Flash(prog_strat_address+(prog_write_cnt*4),__write_flash_buffer);
		__write_flash_buffer_index = 0;
		prog_write_cnt ++;
	}
	//
}

void FLASH_ProgramDone(void)
{
	//FLASH_If_WriteProtection();
	//
	FLASH_Lock();
}


#include "MD5/md5sum.h"
//Program32Bytes_Flash
void TEST_STM32L_FLASH(void)
{
	char str[128];
	md5_state_t state;
	static char debugstr[] = "1234567890qwertyuioplkhgdfasdfasvxcvsdfgsdfgsvend@@";
	int di;
	unsigned char digest[16];
	md5_init(&state);
	md5_append(&state,(unsigned char*)(0x08000000),1024*100);
	snprintf(str,sizeof(str),"FW1 000 ---- 100 MD5:");	
	md5_finish(&state, digest);
	
	for (di = 0; di < 16; ++di)
	{
		char md5str[4];
		snprintf(md5str,sizeof(md5str),"%02x",digest[di]);
		strcat(str,md5str);
	}
	rt_kprintf("\r\n%s\r\n",str);
	
	//-----------------------------------------------------------------------------------
	
	md5_init(&state);
	md5_append(&state,(unsigned char*)(0x08000000 + (1024*100)),1024*100);
	snprintf(str,sizeof(str),"FW 100 --- 200 MD5:");	
	md5_finish(&state, digest);
	
	for (di = 0; di < 16; ++di)
	{
		char md5str[4];
		snprintf(md5str,sizeof(md5str),"%02x",digest[di]);
		strcat(str,md5str);
	}
	rt_kprintf("\r\n%s\r\n",str);
	
	
	
	
	FLASH_ProgramStart(0x08000000 + (1024*100),1024*100);
	FLASH_AppendBuffer((unsigned char*)0x08000000,1024*100);
	FLASH_AppendEnd();
	FLASH_ProgramDone();
	

	
	md5_init(&state);
	md5_append(&state,(unsigned char*)(0x08000000 + (1024*100)),1024*100);
	snprintf(str,sizeof(str),"FW 100k --- 200k MD5:");	
	md5_finish(&state, digest);
	
	for (di = 0; di < 16; ++di)
	{
		char md5str[4];
		snprintf(md5str,sizeof(md5str),"%02x",digest[di]);
		strcat(str,md5str);
	}
	rt_kprintf("\r\n%s\r\n",str);
	
	
	
	
	
//	unsigned int XXX[4] = {0x01,0x02,0x03,0x04};
//	int status;
//	FLASH_If_Init();
//	
//	status=FLASH_If_DisableWriteProtection() ;
//	rt_kprintf("Flash ear!!!  %d\r\n",status);
//  status  = FLASH_If_Erase(APPLICATION_ADDRESS);
//	rt_kprintf("Flash ear!!!  %d\r\n",status);
//	
//	status = Program32Bytes_Flash(APPLICATION_ADDRESS,(unsigned char*)XXX);
//	rt_kprintf("Flash ear!!!  %d\r\n",status);
//	memset(XXX,0x0,16);
//	
//	memcpy(XXX,(unsigned char*)APPLICATION_ADDRESS,16);
//	rt_kprintf("%d %d  %d %d \r\n",XXX[0],XXX[1],XXX[2],XXX[3]);
//	
//	
	
	
	for(;;){};
	//
}
