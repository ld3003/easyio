#ifndef __ADC__
#define __ADC__

#include "common.h"
#include "adc.h"
#include <rtthread.h>

void initADC(void);
void disableADC(void);
uint16_t get_bat_vol(void);
uint16_t get_bat_vol_fast(void);
unsigned char get_bat_pre(void);

#endif