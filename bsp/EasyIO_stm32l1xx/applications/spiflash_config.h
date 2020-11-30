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

//��������ṹ����4K��С�������������
struct SPIFLASH_CONFIG {
	unsigned char		HARDWARE_VERSION_H;		//Ӳ���汾��Ҳ��������bootloader�汾
	unsigned char		HARDWARE_VERSION_L;	
	
	unsigned char		CURRENT_FW_VERSION_H;	//��ǰ�̼��汾
	unsigned char		CURRENT_FW_VERSION_L;
	
	unsigned int		CURRENT_FW_SIZE;				//��ǰ�̼�SIZE
	unsigned char		CURRENT_FW_MD5[16];			//��ǰ�̼���MD5У����
	
	unsigned char		ota_status_machine;			//OTA���̵�״̬��
	
	unsigned char		TARGET_FW_VERSION_H;		//Ŀ��̼��汾
	unsigned char		TARGET_FW_VERSION_L;
	unsigned int		TARGET_FW_SIZE;					//Ŀ��̼�SIZE
	unsigned char		TARGET_FW_MD5[16];			//Ŀ��̼�MD5
	
	
	unsigned short	CONFIG_CRC;					//��ǰ������Ϣ��CRC���������CRC����˵���̼�û�б���ʼ����
};

struct SIMCARD_APN {
	char CCID[28];
	char APN[32];
	char USERNAME[32];
	char PASSWORD[32];
};

struct USER_APP_CONFIG {
	
	/*
	ÿ�θ���user config ���˴˴�������Ҫ�޸������ط�
	1��set_default �е�deflaultֵ
	2, make_json
	3, parser_json
	*/
	
	unsigned char		xSYSTEM_STATUS;					//ϵͳ״̬����ǿ��ػ�����Ϣ,���ڲ�����
	
	//user config
	unsigned int		PRESENCE_UPLOAD_PREIOD;	//��������
	unsigned int		PRESENCE_BC_COUNT;			//��Ӌ���ٹP�ŕ��ς�
	char						HOST_ADDRESS[128];
	short						HOST_PORT;
	char						ACCOUNT[32];
	unsigned char		MANUALLY_OTA;				//�Ƿ����ֶ���ҪOTA
	unsigned char 	movcall_index;
	char						phoneBook[4][16];
	struct SIMCARD_APN	APNINFO;				//APN��Ϣӛ�
	unsigned short	CONFIG_CRC;					//��ǰ������Ϣ��CRC���������CRC����˵���̼�û�б���ʼ����
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
