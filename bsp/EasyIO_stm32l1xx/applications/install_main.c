#include <rtthread.h>
#include <components.h>

#include "common.h"

#include "my_stdc_func/debugl.h"
#include "w25q16.h"
#include "spiflash_config.h"
#include "bootloader.h"


#include "MD5/md5sum.h"

static md5_state_t state;
static md5_byte_t digest[16];
static char hex_output[16*2 + 1];

void check_flash(unsigned int address , int size)
{
	int i=0;
	unsigned char *buff;
	int rcnt=0;
	int total = size;
	unsigned int di;
	
	//if (http_get_status == HTTP_GET_FW)
	{
		char str[64];
	
		#define BUFFER_SIZE_RFLASH 256
		buff = rt_malloc(BUFFER_SIZE_RFLASH);
		if (buff <= 0)
		{
			DEBUGL->debug("MALLOC ERR\r\n");
			for(;;);
		}
		
		//
		//Debug flash md5
		md5_init(&state);
		

		
		while(total >0)
		{

			if (total > BUFFER_SIZE_RFLASH)
			{
				SPI_Flash_Read(buff,address + (rcnt*BUFFER_SIZE_RFLASH),BUFFER_SIZE_RFLASH);
				
				//debugbuffer("READ",buff,BUFFER_SIZE_RFLASH);
				
				md5_append(&state,buff,BUFFER_SIZE_RFLASH);
				total -= BUFFER_SIZE_RFLASH;
				
				
				
			}else {
				SPI_Flash_Read(buff,address + (rcnt*BUFFER_SIZE_RFLASH),total);
				//debugbuffer("READ",buff,total);
				md5_append(&state,buff,total);
				total = 0;
				
			}
			
			rcnt ++;
		}
		
		md5_finish(&state, digest);
		snprintf(str,sizeof(str),"FLASH MD5:");
		for (di = 0; di < 16; ++di)
		{
			char md5str[4];
			snprintf(md5str,sizeof(md5str),"%02x",digest[di]);
			strcat(str,md5str);
		}
		DEBUGL->debug("%s\r\n",str);
		
	
	}
	
}
#include "flash_opt.h"
static void spiflash_2_chipflash(unsigned int spi_address , unsigned int chip_address , int size)
{
	int i=0;
	unsigned char *buff;
	int rcnt=0;
	int total = size;
	unsigned int di;
	unsigned int address = spi_address;
	
	FLASH_ProgramStart(chip_address,size);
	
	
	#define BUFFER_SIZE_RFLASH 256
	buff = rt_malloc(BUFFER_SIZE_RFLASH);
	if (buff <= 0)
	{
		DEBUGL->debug("MALLOC ERR\r\n");
		for(;;);
	}
		
	
	
	while(total >0)
	{

		if (total > BUFFER_SIZE_RFLASH)
		{
			SPI_Flash_Read(buff,address + (rcnt*BUFFER_SIZE_RFLASH),BUFFER_SIZE_RFLASH);
			
			//debugbuffer("READ",buff,BUFFER_SIZE_RFLASH);
			FLASH_AppendBuffer(buff,BUFFER_SIZE_RFLASH);
			//md5_append(&state,buff,BUFFER_SIZE_RFLASH);
			total -= BUFFER_SIZE_RFLASH;
			
			
			
		}else {
			SPI_Flash_Read(buff,address + (rcnt*BUFFER_SIZE_RFLASH),total);
			//debugbuffer("READ",buff,total);
			FLASH_AppendBuffer(buff,total);
			//md5_append(&state,buff,total);
			total = 0;
			
		}
		
		rcnt ++;
	}
	
	FLASH_AppendEnd();
	FLASH_ProgramDone();
	//
}



typedef  void (*pFunction)(void);
void JumpToApp(u32 appAddr)
{
	pFunction JumpToApplication;
	u32 JumpAddress;
	
	//appAddr = CPU2_FW_ADDRESS;
	
	JumpAddress = *(u32*) (appAddr + 4);
	JumpToApplication = (pFunction)JumpAddress;
  /* Initialize user application's Stack Pointer */
  __set_MSP(*(vu32*) appAddr);
  JumpToApplication();
}

#if 0
#include "bootloader.h"
#define NVIC_VectTab_RAM             ((uint32_t)0x20000000)
//#define NVIC_VectTab_FLASH           ((uint32_t)0x08000000)
#ifdef ENABLE_OTA
#define NVIC_VectTab_FLASH           ((uint32_t)EASYIO_ADDRESS)
#else
#define NVIC_VectTab_FLASH           ((uint32_t)0x08000000)
#endif
#define IS_NVIC_VECTTAB(VECTTAB) (((VECTTAB) == NVIC_VectTab_RAM) || \
                                  ((VECTTAB) == NVIC_VectTab_FLASH))
#endif


void mainloop(void *p)
{
	
	extern unsigned char enable_bianpin;
	
	uint16_t spi_flash_id;
	int at_cmd_ret_code;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	DEBUGL->debug("@@@@@@@@@@@@@@@@@@@@@@@@ BUILD DATE %s TIME %s \r\n",__DATE__,__TIME__);
	

	//需要两次初始化
	SPI_Flash_Init();
	spi_flash_id = SPI_Flash_ReadID();
	DEBUGL->debug("SPI Flash ID %X \r\n",spi_flash_id);
	
	SPI_Flash_Init();
	spi_flash_id = SPI_Flash_ReadID();
	DEBUGL->debug("SPI Flash ID %X \r\n",spi_flash_id);
	
	#ifdef TEST_CONFIG_RW
	readconfig();
	DEBUGL->debug("SPI Flash CONFIG A %d %d %d\r\n",spiflash_conf.VERSION[0],spiflash_conf.VERSION[1],spiflash_conf.VERSION[2]);
	spiflash_conf.VERSION[0] += 1;
	spiflash_conf.VERSION[1] += 2;
	spiflash_conf.VERSION[2] += 3;
	
	spiflash_conf.ota_flag = 0;
	writeconfig();
	readconfig();
	DEBUGL->debug("SPI Flash CONFIG B %d %d %d\r\n",spiflash_conf.VERSION[0],spiflash_conf.VERSION[1],spiflash_conf.VERSION[2]);
	#endif
	
	readconfig();
	
	if (/*spiflash_conf.ota_flag == 0*/0)
	{
		//如果OTA需要更新
		char str[64];
		int di = 0;
		
		//检查当前FW的MD5
		md5_init(&state);
		md5_append(&state,(unsigned char*)EASYIO_ADDRESS,EASYIO_SIZE);
		md5_finish(&state, digest);
		snprintf(str,sizeof(str),"FLASH 1 MD5:");
		for (di = 0; di < 16; ++di)
		{
			char md5str[4];
			snprintf(md5str,sizeof(md5str),"%02x",digest[di]);
			strcat(str,md5str);
		}
		DEBUGL->debug("%s\r\n",str);
		
		
		//备份当前FW到SPIFLASH
		init_write_flash(SPI_FLASH_FW_ADDRESS,0x0);																	//初始化地址
		append_w25x_append_buffer((unsigned char*)EASYIO_ADDRESS,EASYIO_SIZE);		//256*1024k
		w25x_append_end();
		
		//校验 SPI FLASH
		check_flash(SPI_FLASH_FW_ADDRESS,EASYIO_SIZE);
		
		//恢复
		spiflash_2_chipflash(SPI_FLASH_FW_ADDRESS,EASYIO_ADDRESS,EASYIO_SIZE);
		
		md5_init(&state);
		md5_append(&state,(unsigned char*)EASYIO_ADDRESS,EASYIO_SIZE);
		md5_finish(&state, digest);
		snprintf(str,sizeof(str),"FLASH 2 MD5:");
		for (di = 0; di < 16; ++di)
		{
			char md5str[4];
			snprintf(md5str,sizeof(md5str),"%02x",digest[di]);
			strcat(str,md5str);
		}
		DEBUGL->debug("%s\r\n",str);
		
		
	}else{
		//
	}
	
	
	DEBUGL->debug("GOTO FW ... \r\n");
	//JumpToApp(EASYIO_ADDRESS);
	
	for(;;)
	{
		DEBUGL->debug("GOTO FW ... \r\n");
		rt_thread_sleep(100);
	}
	
	
	return ;
}


