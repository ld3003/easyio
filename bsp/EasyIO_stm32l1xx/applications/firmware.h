#ifndef __firmware__
#define __firmware__


#pragma pack(push)
#pragma  pack(1)
struct FIRMWARE_HEADER {
    unsigned int size;
    unsigned char version[3];
		unsigned char MD5[16];
};
#pragma  pack(pop)


#endif