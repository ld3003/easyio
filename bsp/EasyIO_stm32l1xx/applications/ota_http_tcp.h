#ifndef __ota_http_tcp__
#define __ota_http_tcp__


void ota_init_http_cli(void);
void ota_http_tcp_routing(void *p);
char ota_start_http_download(char *from , char *body);
void ota_restart_http_download(void);
void init_ota_http_cli(void);


#define OTA_FLAG_VERSIONNUM_MODE_CFG		0			//���汾�ź��ֶ�ģʽ
#define OTA_FLAG_VERSIONNUM_CFG					1			//ֻ���汾��
#define OTA_FLAG_NULL_CFG								2

extern char test_ota_flag; //OTA �������


#define STATUS_UPGRADE_OK										0
#define STATUS_TIME_OUT											1
#define STATUS_WAIT_CHECK										2
#define STATUS_DOWNLOAD_CONF								3
#define STATUS_DOWNLOAD_CONF_FAIL						4
#define STATUS_PARSE_CONF										5
#define STATUS_PARSE_CONF_ERROR							6
#define STATUS_HAVE_NEW_VERSON							7
#define STATUS_UPGRADE_MANUALLY							8
#define STATUS_WAIT_DOWNLOAD								9
#define STATUS_START_DOWNLOAD								10
#define STATUS_ALREADY_DOWNLOAD							11
#define STATUS_DOWNLOAD_OTA_FAIL						12
#define STATUS_CHECKSUM_FAIL								13
#define STATUS_UNZIP_FAIL										14
#define STATUS_OTA_FORMAT_ERROR							15
#define STATUS_START_BACKUP									16
#define STATUS_BACKUP_FAIL									17
#define STATUS_START_UPGRADE								18
#define STATUS_UPGRADE_FAIL									19
#define STATUS_UPGRADE_STOP									20


#if 0
//OTA�������³ɹ�
moduleApi.STATUS_UPGRADE_OK = 0
--ָ����r
moduleApi.STATUS_TIME_OUT = 1
--�ȴ���һ�εĸ��r�g���_
moduleApi.STATUS_WAIT_CHECK = 2
--���dconfigure�n
moduleApi.STATUS_DOWNLOAD_CONF = 3
--Configure���dʧ��
moduleApi.STATUS_DOWNLOAD_CONF_FAIL = 4
--����Configure��
moduleApi.STATUS_PARSE_CONF = 5
--����Configureʧ��
moduleApi.STATUS_PARSE_CONF_ERROR = 6
--��B�a,����Firmware�ɸ���
moduleApi.STATUS_HAVE_NEW_VERSON = 7
--�˞��քӸ��°�OTA,�ȴ��Ñ��ք����_����ָ��
moduleApi.STATUS_UPGRADE_MANUALLY = 8
--���������_�_ʼ���dָ��,�ȴ����d�����_ʼ�M��
moduleApi.STATUS_WAIT_DOWNLOAD = 9
--�����_ʼ�ں�Server���d�n��
moduleApi.STATUS_START_DOWNLOAD = 10
--���n�������d���
moduleApi.STATUS_ALREADY_DOWNLOAD = 11
--���n���dʧ��
moduleApi.STATUS_DOWNLOAD_OTA_FAIL = 12
--Ӌ��n��checksum�e�`
moduleApi.STATUS_CHECKSUM_FAIL = 13
--�≺�s�n���e�`
moduleApi.STATUS_UNZIP_FAIL = 14
--�n����ʽ�e�`
moduleApi.STATUS_OTA_FORMAT_ERROR = 15
--���ڂ��ϵ�y�Y����
moduleApi.STATUS_START_BACKUP = 16
--���ʧ��
moduleApi.STATUS_BACKUP_FAIL = 17
--���ڸ���ϵ�y��
moduleApi.STATUS_START_UPGRADE = 18
--����ʧ��
moduleApi.STATUS_UPGRADE_FAIL = 19
--����ֹͣ
moduleApi.STATUS_UPGRADE_STOP = 20
#endif



#endif
