#ifndef __ota_http_tcp__
#define __ota_http_tcp__


void ota_init_http_cli(void);
void ota_http_tcp_routing(void *p);
char ota_start_http_download(char *from , char *body);
void ota_restart_http_download(void);
void init_ota_http_cli(void);


#define OTA_FLAG_VERSIONNUM_MODE_CFG		0			//检查版本号和手动模式
#define OTA_FLAG_VERSIONNUM_CFG					1			//只检查版本号
#define OTA_FLAG_NULL_CFG								2

extern char test_ota_flag; //OTA 操作标记


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
//OTA完整更新成功
moduleApi.STATUS_UPGRADE_OK = 0
--指令逾時
moduleApi.STATUS_TIME_OUT = 1
--等待下一次的更新時間到達
moduleApi.STATUS_WAIT_CHECK = 2
--下載configure檔
moduleApi.STATUS_DOWNLOAD_CONF = 3
--Configure下載失敗
moduleApi.STATUS_DOWNLOAD_CONF_FAIL = 4
--解析Configure中
moduleApi.STATUS_PARSE_CONF = 5
--解析Configure失敗
moduleApi.STATUS_PARSE_CONF_ERROR = 6
--狀態碼,有新Firmware可更新
moduleApi.STATUS_HAVE_NEW_VERSON = 7
--此為手動更新版OTA,等待用戶手動下達更新指令
moduleApi.STATUS_UPGRADE_MANUALLY = 8
--己接收下達開始下載指令,等待下載動作開始進行
moduleApi.STATUS_WAIT_DOWNLOAD = 9
--己經開始在和Server下載檔案
moduleApi.STATUS_START_DOWNLOAD = 10
--更新檔己經下載完成
moduleApi.STATUS_ALREADY_DOWNLOAD = 11
--更新檔下載失敗
moduleApi.STATUS_DOWNLOAD_OTA_FAIL = 12
--計算檔案checksum錯誤
moduleApi.STATUS_CHECKSUM_FAIL = 13
--解壓縮檔案錯誤
moduleApi.STATUS_UNZIP_FAIL = 14
--檔案格式錯誤
moduleApi.STATUS_OTA_FORMAT_ERROR = 15
--正在備份系統資料中
moduleApi.STATUS_START_BACKUP = 16
--備份失敗
moduleApi.STATUS_BACKUP_FAIL = 17
--正在更新系統中
moduleApi.STATUS_START_UPGRADE = 18
--更新失敗
moduleApi.STATUS_UPGRADE_FAIL = 19
--更新停止
moduleApi.STATUS_UPGRADE_STOP = 20
#endif



#endif
