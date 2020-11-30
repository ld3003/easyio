#include <rtthread.h>
#include <components.h>

#include "common.h"

#include "my_stdc_func/debugl.h"
#include "w25q16.h"
#include "spiflash_config.h"
#include "bootloader.h"
#include "firmware.h" //
#include "MD5/md5sum.h"
#include "led.h"
#include "watch_dog.h"
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
			feed_watchdog(0);
			FLASH_AppendBuffer(buff,BUFFER_SIZE_RFLASH);
			//md5_append(&state,buff,BUFFER_SIZE_RFLASH);
			total -= BUFFER_SIZE_RFLASH;
			
			
			
		}else {
			SPI_Flash_Read(buff,address + (rcnt*BUFFER_SIZE_RFLASH),total);
			//debugbuffer("READ",buff,total);
			feed_watchdog(0);
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


#define HARDWARE_VERSION_H_VAL		1
#define HARDWARE_VERSION_L_VAL		0


void bootloader_readconfig(void){
	unsigned short crc;
	char read_cnt = 0;
	
	
	spiflash_release_powerdown();
	

	REREAD:
	read_cnt ++;
	SPI_Flash_Read((unsigned char*)&spiflash_conf,CONFIG_ADDRESS,sizeof(struct SPIFLASH_CONFIG));
	
	crc = sdk_stream_crc16_calc((unsigned char *)&spiflash_conf,((unsigned int)(&spiflash_conf.CONFIG_CRC) - (unsigned int)(&spiflash_conf))); //¶àÉÙ
	
	if (spiflash_conf.CONFIG_CRC == crc)
	{
		return ;
	}

	
	//set default value
	memset((unsigned char*)&spiflash_conf,0x0,sizeof(spiflash_conf));
	spiflash_conf.HARDWARE_VERSION_H = HARDWARE_VERSION_H_VAL;
	spiflash_conf.HARDWARE_VERSION_L = HARDWARE_VERSION_L_VAL;
	
	writeconfig();
	
	spiflash_powerdown();
	//goto REREAD;
	
	//set default
	//
}

void write_hardware_version(void)
{
	//Èç¹ûÓ²¼þÏàÍ¬£¬Ôò¡£¡£¡£
	if ((spiflash_conf.HARDWARE_VERSION_H == HARDWARE_VERSION_H_VAL) && (spiflash_conf.HARDWARE_VERSION_L == HARDWARE_VERSION_L_VAL))
	{
		return ;
	}
	
	spiflash_conf.HARDWARE_VERSION_H = HARDWARE_VERSION_H_VAL;
	spiflash_conf.HARDWARE_VERSION_L = HARDWARE_VERSION_L_VAL;
	writeconfig();
	return;
	
}



extern unsigned int SystemCoreClock;
extern void rt_hw_board_init(void);
extern unsigned int SystemCoreClockConfigure_1M(void);
extern unsigned int SystemCoreClockConfigure_2M(void);
extern unsigned int SystemCoreClockConfigure_4M(void);
extern unsigned int SystemCoreClockConfigure_32M(void);
extern unsigned int SystemCoreClockConfigure_12M(void);
extern unsigned int SystemCoreClockConfigure_65K(void);

extern void reinit_board(void);

#ifdef FRQL
static unsigned char fqtype = 1;
#endif

#ifdef FRQH
static unsigned char fqtype = 2;
#endif

void config_fq_0(void)
{

	if (fqtype == 0)
		return ;
	fqtype = 0;
	SystemCoreClock = SystemCoreClockConfigure_1M();
	USART_DeInit(USART2);
	reinit_board();
	reinit_system_serial();
}

void config_fq_1(void)
{

	unsigned short spi_flash_id = 0x0;
	
	//±£Ö¤Ö»»á
	if (fqtype == 1)
		return ;
	fqtype = 1;

	SystemInit();
	SystemCoreClock = SystemCoreClockConfigure_2M();
	reinit_board();
	reinit_system_serial();
	
		//³õÊ¼»¯SPIFlash
	SPI_Flash_Init();
	spi_flash_id = SPI_Flash_ReadID();
	DEBUGL->debug("SPI Flash ID %X \r\n",spi_flash_id);
}



void config_fq_2(void)
{
	unsigned short spi_flash_id = 0x0;
	
	if (fqtype == 2)
		return ;
	fqtype = 2;


	SystemCoreClock = SystemCoreClockConfigure_32M();
	reinit_board();
	reinit_system_serial();
	
	//³õÊ¼»¯SPIFlash
	SPI_Flash_Init();
	spi_flash_id = SPI_Flash_ReadID();
	DEBUGL->debug("SPI Flash ID %X \r\n",spi_flash_id);
	
	
}



#include "../fw.h"

void mainloop(void *p)
{
	
	unsigned int ota_fw_size;
	uint16_t spi_flash_id;
	int at_cmd_ret_code;
	GPIO_InitTypeDef GPIO_InitStructure;
	struct FIRMWARE_HEADER firmware_header;
	
	DEBUGL->debug("@@@@@@@@@@@@@@@@@@@@@@@@ BUILD DATE %s TIME %s [%02x]\r\n",__DATE__,__TIME__,0x0);
	
	enable_wg();
	
	init_led_hw();

	//ÐèÒªÁ½´Î³õÊ¼»¯
	SPI_Flash_Init();
	spi_flash_id = SPI_Flash_ReadID();
	DEBUGL->debug("SPI Flash ID %X \r\n",spi_flash_id);
	
	SPI_Flash_Init();
	spi_flash_id = SPI_Flash_ReadID();
	DEBUGL->debug("SPI Flash ID %X \r\n",spi_flash_id);
	
	
	
//	led_on(0);
//	led_on(1);
//	led_on(2);
	
	bootloader_readconfig();
	write_hardware_version();
	
	//spiflash_2_chipflash(SPI_FLASH_FW_ADDRESS + sizeof(firmware_header),EASYIO_ADDRESS,firmware_header.size);		//¿ªÊ¼¸üÐ
	SPI_Flash_Read((unsigned char*)&firmware_header,SPI_FLASH_FW_ADDRESS,sizeof(firmware_header));							//¶ÁÈ¡¹Ì¼þÍ·ÐÅÏ¢
	
	for(;;)
	{
		switch(spiflash_conf.ota_status_machine)
		{
			case OTA_STATUS_MACHINE_NONE:
			{
//				RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1, DISABLE);
//				USART_DeInit(USART1);
				DEBUGL->debug("GO !!!!!!\r\n");
				
				
				//ÅäÖÃ´®¿Ú1
				{
					/* USART1 */
					#define UART1_GPIO_TX		GPIO_Pin_9
					#define UART1_GPIO_RX		GPIO_Pin_10
					#define UART1_GPIO			GPIOA

			
					GPIO_InitTypeDef GPIO_InitStructure;
				
					GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
					GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
					GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
				
					/* Connect PXx to USARTx_Tx */
					GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,GPIO_AF_USART1);//PA9-TX 
					/* Connect PXx to USARTx_Rx */
					GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);//PA10-RX
				
					/* Configure USART Rx/tx PIN */
					GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
					GPIO_InitStructure.GPIO_Pin = UART1_GPIO_RX;
					GPIO_Init(UART1_GPIO, &GPIO_InitStructure);

					GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
					GPIO_InitStructure.GPIO_Pin = UART1_GPIO_TX;
					GPIO_Init(UART1_GPIO, &GPIO_InitStructure);
					
					//disable UART
					USART_DeInit(USART1);
					USART_DeInit(USART2);
					USART_DeInit(USART3);
				
				}
				

				
				//disable UART

				JumpToApp(EASYIO_ADDRESS);
				break;
			}
			case OTA_STATUS_MACHINE_DOWNLOAD_FINISH:
			{
				char str[64];
				int di = 0;
				
				//no backup ²»ÐèÒª ±¸·Ý
				spiflash_conf.ota_status_machine = OTA_STATUS_MACHINE_BACKUP_FINISH;
				writeconfig();
				break;
				
				//±¸·Ýµ±Ç°¹Ì¼þ
				spiflash_conf.CURRENT_FW_SIZE = EASYIO_APP_SIZE - sizeof(firmware_header);								//±¸·Ý×î´ó
				firmware_header.size = spiflash_conf.CURRENT_FW_SIZE;
				memcpy(firmware_header.MD5,spiflash_conf.CURRENT_FW_MD5,16);
				init_write_flash(SPI_FLASH_FW_BCKP_ADDRESS,0x0);																	//³õÊ¼»¯µØÖ·
				
				append_w25x_append_buffer((unsigned char*)&firmware_header,sizeof(firmware_header));	//Ð´Èë¹Ì¼þÍ·ÐÅÏ¢µ½flash
				append_w25x_append_buffer((unsigned char*)EASYIO_ADDRESS,firmware_header.size);	//½«¹Ì¼þÐ´Èë ±¸·Ýµ½ SPI Flash
				w25x_append_end();
				
				
				//¼ì²éµ±Ç°FWµÄMD5
				md5_init(&state);
				md5_append(&state,(unsigned char*)EASYIO_ADDRESS,spiflash_conf.CURRENT_FW_SIZE);
				md5_finish(&state, digest);
				snprintf(str,sizeof(str),"Current FW MD5:");
				for (di = 0; di < 16; ++di)
				{
					char md5str[4];
					snprintf(md5str,sizeof(md5str),"%02x",digest[di]);
					strcat(str,md5str);
				}
				DEBUGL->debug("%s\r\n",str);

				
				//¼ì²éµ±Ç°±¸·ÝµÄ¹Ì¼þMD5
				DEBUGL->debug("Current BACKUP SPIFLASH FW");
				check_flash(SPI_FLASH_FW_BCKP_ADDRESS + sizeof(firmware_header),firmware_header.size);
				
				//±ê¼Ç±¸·ÝÍê³É£¬¿ÉÒÔ½øÐÐ¹Ì¼þ¸üÐÂ
				spiflash_conf.ota_status_machine = OTA_STATUS_MACHINE_BACKUP_FINISH;
				writeconfig();
				break;
			}
			
			
			case OTA_STATUS_MACHINE_BACKUP_FINISH:
			{
				//ËµÃ÷±¸·Ý£¬µ«ÊÇ¸üÐÂÃ»ÓÐÍê³É,Òò´Ë¼±Ðè¸üÐÂ
				//´ÓSPIflash¸üÐÂ¹Ì¼þ
				SPI_Flash_Read((unsigned char*)&firmware_header,SPI_FLASH_FW_ADDRESS,sizeof(firmware_header));							//¶ÁÈ¡¹Ì¼þÍ·ÐÅÏ¢
				
				//¼ì²é¹Ì¼þ³¤¶ÈÊÇ·ñºÏ·¨£¬Èç¹û³¬¹ýÁË£¬ÄÇÃ´½«ÈÏÎª²»ºÏ·¨
				if (firmware_header.size > EASYIO_APP_SIZE - sizeof(firmware_header))
				{
					//ÈÏÎªÔËÐÐÊ§°Ü£¬»Ö¸´ÉÏÒ»°æ±¸·ÝµÄ¹Ì¼þ,ÓÉÓÚ»¹Ã»ÓÐ½øÐÐ¶ÁÐ´ËùÒÔ£¬Ö±½ÓÔËÐÐ¾Í¿ÉÒÔ£¬²»ÓÃ¸üÐÂFlash
					spiflash_conf.ota_status_machine = OTA_STATUS_MACHINE_NONE;
					writeconfig();
					break;
				}
				
				spiflash_2_chipflash(SPI_FLASH_FW_ADDRESS + sizeof(firmware_header),EASYIO_ADDRESS,firmware_header.size);		//¿ªÊ¼¸üÐÂ
				//Ä¿Ç°Ö»Ð§Ñé£¬²»¿½±´
				check_flash(SPI_FLASH_FW_ADDRESS + sizeof(firmware_header),firmware_header.size);
				
				//¸Ä±ä×´Ì¬»ú
				spiflash_conf.ota_status_machine = OTA_STATUS_MACHINE_UPDATE_FINISH;
				writeconfig();
				DEBUGL->debug("GOTO INSTALL ...\r\n");
				JumpToApp(EASYIO_ADDRESS);
				
				break;
			}
			
			case OTA_STATUS_MACHINE_UPDATE_FINISH:
			{
				spiflash_conf.ota_status_machine = OTA_STATUS_MACHINE_WAIT_FW_SUCCESS_RUN;
				DEBUGL->debug("GOTO INSTALL ...\r\n");
				JumpToApp(EASYIO_ADDRESS);
				break;
			}
			
			case OTA_STATUS_MACHINE_WAIT_FW_SUCCESS_RUN:
			{
				//ÔËÐÐÊ§°ÜÁËà¸£¬»Ö¸´ÉÏÒ»°æ±¾µÄ¹Ì¼þ
				DEBUGL->debug("FW RUNNING ERROR ...\r\n");
				SPI_Flash_Read((unsigned char*)&firmware_header,SPI_FLASH_FW_BCKP_ADDRESS,sizeof(firmware_header));							//¶ÁÈ¡¹Ì¼þÍ·ÐÅÏ¢
				if (firmware_header.size > EASYIO_APP_SIZE - sizeof(firmware_header))
				{
					DEBUGL->debug("GAME OVER !!!\n");
					for(;;){};
					break;
				}
				
				led_on(0);
				led_on(1);
				led_on(2);
				
				
				spiflash_2_chipflash(SPI_FLASH_FW_BCKP_ADDRESS + sizeof(firmware_header),EASYIO_ADDRESS,firmware_header.size);
				
				spiflash_conf.ota_status_machine = OTA_STATUS_MACHINE_NONE;
				writeconfig();
				break;
			}
			
		}
	
	}
	
	

	
	return ;
}

void disable_gps (void){};
void sys_power_off(void){};

