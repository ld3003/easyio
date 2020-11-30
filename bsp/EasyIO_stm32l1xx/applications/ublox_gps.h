#ifndef __ublox_gps__
#define __ublox_gps__
#include <rtthread.h>

void enable_gps(void);
void disable_gps(void);

//标记gps是否开启
extern unsigned char gps_enable_flag;

extern unsigned char gps_process_flag;

rt_err_t open_gps_uart(char *name);


typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

#define HDR_CHKSM_LENGTH 8 //(includes "sync1 sync2 classid msgid length checksum")
#define UBX_SYNC_BYTE_1 0xB5
#define UBX_SYNC_BYTE_2 0x62

//! UBX Protocol Class/Message ID's
#define MSG_CLASS_ACK 0X05
    #define MSG_ID_ACK_ACK 0x01
    #define MSG_ID_ACK_NAK 0x00
#define MSG_CLASS_CFG 0x06
    #define MSG_ID_CFG_ANT 0X13
    #define MSG_ID_CFG_CFG 0x09
    #define MSG_ID_CFG_DAT 0x06
    #define MSG_ID_CFG_DOSC 0x61
    #define MSG_ID_CFG_ESRC 0x60
    #define MSG_ID_CFG_GNSS 0x3E
    #define MSG_ID_CFG_INF 0x02
    #define MSG_ID_CFG_ITFM 0x39
    #define MSG_ID_CFG_LOGFILTER 0x47
    #define MSG_ID_CFG_MSG 0x01
    #define MSG_ID_CFG_NAV5 0x24
    #define MSG_ID_CFG_NAVX5 0x23
    #define MSG_ID_CFG_NMEA 0x17
    #define MSG_ID_CFG_ODO 0x1E
    #define MSG_ID_CFG_PM2 0x3B
    #define MSG_ID_CFG_PRT 0x00
    #define MSG_ID_CFG_PWR 0x57
    #define MSG_ID_CFG_RATE 0x08
    #define MSG_ID_CFG_RINV 0x34
    #define MSG_ID_CFG_RST 0x04
    #define MSG_ID_CFG_RXM 0x11
    #define MSG_ID_CFG_SBAS 0x16
    #define MSG_ID_CFG_SMGR 0x62
    #define MSG_ID_CFG_TMODE2 0x3D
    #define MSG_ID_CFG_TP5 0x31
    #define MSG_ID_CFG_TXSLOT 0x53
    #define MSG_ID_CFG_USB 0x1B
#define MSG_CLASS_INF 0x04
    #define MSG_ID_INF_DEBUG 0x04
    #define MSG_ID_INF_ERROR 0x00
    #define MSG_ID_INF_NOTICE 0x02
    #define MSG_ID_INF_TEST 0x03
    #define MSG_ID_INF_WARNING 0x01
#define MSG_CLASS_LOG 0x21
    #define MSG_ID_LOG_CREATE 0x07
    #define MSG_ID_LOG_ERASE 0x03
    #define MSG_ID_LOG_FINDTIME 0x0E
    #define MSG_ID_LOG_INFO 0x08
    #define MSG_ID_LOG_RETRIEVEPOSEXTRA 0x0F
    #define MSG_ID_LOG_RETRIEVEPOS 0x0B
    #define MSG_ID_LOG_RETRIEVESTRING 0x0d
    #define MSG_ID_LOG_RETRIEVE 0x09
    #define MSG_ID_LOG_STRING 0x04
#define MSG_CLASS_MGA 0x13
    #define MSG_ID_MGA_ACK 0x60
    #define MSG_ID_MGA_ANO 0x20
    #define MSG_ID_MGA_DBD 0x80
    #define MSG_ID_MGA_DBD 0x80
    #define MSG_ID_MGA_FLASH 0x21
    #define MSG_ID_MGA_GLO 0x06
    #define MSG_ID_MGA_GPS 0x00
    #define MSG_ID_MGA_INI 0x40

    #define MSG_ID_MGA_QZSS 0x05
#define MSG_CLASS_MON 0x0A
    #define MSG_ID_MON_GNSS 0x28
    #define MSG_ID_MON_HW2 0x0B
    #define MSG_ID_MON_HW 0x09
    #define MSG_ID_MON_IO 0x02
    #define MSG_ID_MON_MSGPP 0x06
    #define MSG_ID_MON_PATCH 0x27
    #define MSG_ID_MON_RXBUF 0x07
    #define MSG_ID_MON_RXR 0x21
    #define MSG_ID_MON_SMGR 0x2E
    #define MSG_ID_MON_TXBUF 0X08
    #define MSG_ID_MON_VER 0x04
#define MSG_CLASS_NAV 0x01
    #define MSG_ID_NAV_AOPSTATUS 0x60
    #define MSG_ID_NAV_CLOCK 0x22
    #define MSG_ID_NAV_DGPS 0x31
    #define MSG_ID_NAV_DOP 0x04
    #define MSG_ID_NAV_ODO 0x09
    #define MSG_ID_NAV_ORB 0x34
    #define MSG_ID_NAV_POSECEF 0x01
    #define MSG_ID_NAV_POSLLH 0x02
    #define MSG_ID_NAV_PVT 0x07
    #define MSG_ID_NAV_RESETODO 0x10
    #define MSG_ID_NAV_SAT 0x35
    #define MSG_ID_NAV_SBAS 0x32
    #define MSG_ID_NAV_SOL 0x06
    #define MSG_ID_NAV_STATUS 0x03
    #define MSG_ID_NAV_SVINFO 0x30
    #define MSG_ID_NAV_TIMEBDS 0x24
    #define MSG_ID_NAV_TIMEGLO 0x23
    #define MSG_ID_NAV_TIMEGPS 0x20
    #define MSG_ID_NAV_TIMEUTC 0x21
    #define MSG_ID_NAV_VELECEF 0x11
    #define MSG_ID_NAV_VELNED 0x12
#define MSG_CLASS_RXM 0x02
    #define MSG_ID_RXM_PMREQ 0x41
    #define MSG_ID_RXM_RAWX 0x15
    #define MSG_ID_RXM_SFRBX 0x13
    #define MSG_ID_RXM_SVSI 0x20
#define MSG_CLASS_TIM 0x0D
    #define MSG_ID_TIM_DOSC 0x11
    #define MSG_ID_TIM_FCHG 0x16
    #define MSG_ID_TIM_HOC 0x17
    #define MSG_ID_TIM_SMEAS 0x13
    #define MSG_ID_TIM_SVIN 0x04
    #define MSG_ID_TIM_TM2 0x03
    #define MSG_ID_TIM_TOS 0x12
    #define MSG_ID_TIM_TP 0x01
    #define MSG_ID_TIM_VCOCAL 0x15
    #define MSG_ID_TIM_VRFY 0x06
#define MSG_CLASS_UPD 0x09
    #define MSG_ID_UPD_SOS 0x14
		


#pragma pack(push)
#pragma  pack(1)

typedef struct _UbloxHeader {
        uint8_t sync1;   //!< start of packet first byte (0xB5)
        uint8_t sync2;   //!< start of packet second byte (0x62)
        uint8_t message_class; //!< Class that defines basic subset of message (NAV, RXM, etc.)
        uint8_t message_id;             //!< Message ID
        uint16_t payload_length; //!< length of the payload data, excluding header and checksum
}UbloxHeader;
 

struct CfgRst {
        UbloxHeader header;     //!< Ublox header
        uint16_t nav_bbr_mask;  //!< Nav data to clear: 0x0000 = hot start, 0x0001 = warm start, 0xFFFF=cold start
        uint8_t  reset_mode;     //!< Reset mode
        uint8_t  reserved;       //!< reserved
        uint8_t checksum[2];
};


#pragma pack(pop)

void init_ublox_hw(void);

void calculateCheckSum(uint8_t* in, rt_size_t length, uint8_t* out);

void __send_restart_cmd(unsigned short nav_bbr_mask , unsigned char reset_mode);
//Coldstart

#define GPS_COLDSTART __send_restart_cmd(0xFFFF,0x02);
#define TEST_GPS	__send_gps_cmd();


#endif