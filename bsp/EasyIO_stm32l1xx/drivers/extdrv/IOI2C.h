#ifndef __IOI2C_H
#define __IOI2C_H




extern void IOI2C_Init(void);
extern unsigned char IICwriteBytes(unsigned char dev, unsigned char reg, unsigned char length, unsigned char* data);
extern unsigned char IICreadBytes(unsigned char dev, unsigned char reg, unsigned char length, unsigned char *data);
extern unsigned short I2C_Erorr_Count;

typedef unsigned char u8;

void I2C_GPIO_Config(void);
void I2C1_Init(void);
void I2C_Write(u8 addr, u8 data);
u8 I2C_Read(u8 nAddr);



#endif