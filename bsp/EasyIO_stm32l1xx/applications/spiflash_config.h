#ifndef __spi_flash_zone__
#define __spi_flash_zone__

#include "bootloader.h"

#define SPIFLASH_BASE_ADDR							4096 * 256

#define CONFIG_ADDRESS									SPIFLASH_BASE_ADDR
#define CONFIG_SIZE											4096

#define SPI_FLASH_FW_BCKP_ADDRESS				SPIFLASH_BASE_ADDR + CONFIG_SIZE
#define SPI_FLASH_FW_BCKP_SIZE					EASYIO_APP_SIZE

#define SPI_FLASH_FW_ADDRESS						SPIFLASH_BASE_ADDR + CONFIG_SIZE + SPI_FLASH_FW_BCKP_SIZE
#define SPI_FLASH_FW_SIZE								EASYIO_APP_SIZE

#define USER_APP_CONFIG_ADDRESS							SPIFLASH_BASE_ADDR + CONFIG_SIZE + SPI_FLASH_FW_BCKP_SIZE + SPI_FLASH_FW_SIZE
#define USER_APP_CONFIG_SIZE								4096

#pragma pack(push)
#pragma  pack(1)


enum {
	OTA_STATUS_MACHINE_NONE,
	OTA_STATUS_MACHINE_DOWNLOAD_FINISH,
	OTA_STATUS_MACHINE_BACKUP_FINISH,
	OTA_STATUS_MACHINE_UPDATE_FINISH,
	
	OTA_STATUS_MACHINE_WAIT_FW_SUCCESS_RUN,
	
	OTA_STATUS_MACHINE_FW_RUN1,
	OTA_STATUS_MACHINE_FW_RUN2,
	
	OTA_STATUS_MACHINE_FW_ERROR,
};

//下面这个结构体有4K大小，后面可以扩充
struct SPIFLASH_CONFIG {
	unsigned char		HARDWARE_VERSION_H;		//硬件版本，也可以算做bootloader版本
	unsigned char		HARDWARE_VERSION_L;	
	
	unsigned char		CURRENT_FW_VERSION_H;	//当前固件版本
	unsigned char		CURRENT_FW_VERSION_L;
	
	unsigned int		CURRENT_FW_SIZE;				//当前固件SIZE
	unsigned char		CURRENT_FW_MD5[16];			//当前固件的MD5校验码
	
	unsigned char		ota_status_machine;			//OTA流程的状态机
	
	unsigned char		TARGET_FW_VERSION_H;		//目标固件版本
	unsigned char		TARGET_FW_VERSION_L;
	unsigned int		TARGET_FW_SIZE;					//目标固件SIZE
	unsigned char		TARGET_FW_MD5[16];			//目标固件MD5
	
	
	unsigned short	CONFIG_CRC;					//当前配置信息的CRC，如果发现CRC不对说明固件没有被初始化过
};

struct SIMCARD_APN {
	char CCID[28];
	char APN[32];
	char USERNAME[32];
	char PASSWORD[32];
};

struct USER_APP_CONFIG {
	
	/*
	每次更改user config 除了此处，还需要修改三个地方
	1，set_default 中的deflault值
	2, make_json
	3, parser_json
	*/
	
	unsigned char		xSYSTEM_STATUS;					//系统状态，标记开关机等信息,现在不用了
	
	//user config
	unsigned int		PRESENCE_UPLOAD_PREIOD;	//更新周期
	unsigned int		PRESENCE_BC_COUNT;			//累計多少筆才會上傳
	char						HOST_ADDRESS[128];
	short						HOST_PORT;
	char						ACCOUNT[32];
	unsigned char		MANUALLY_OTA;				//是否有手动需要OTA
	unsigned char 	movcall_index;
	char						phoneBook[4][16];
	struct SIMCARD_APN	APNINFO;				//APN信息記錄
	unsigned short	CONFIG_CRC;					//当前配置信息的CRC，如果发现CRC不对说明固件没有被初始化过
};




#pragma  pack(pop)

extern struct SPIFLASH_CONFIG spiflash_conf;
extern struct USER_APP_CONFIG user_app_conf;

void readconfig(void);
void writeconfig(void);

void userapp_readconfig(void);
void userapp_writeconfig(void);
void set_default_userconfig(char writeflash);

void json_userapp_readconfig(void);
void json_userapp_writeconfig(void);
void json_set_default_userconfig(void);

char * makeJson(void);
void parseJson(char * pMsg);
void debug_user_config(void);





#endif
