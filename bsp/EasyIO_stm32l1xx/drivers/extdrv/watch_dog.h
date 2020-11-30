#ifndef __watch_dog__
#define __watch_dog__

#include <rtthread.h>
#include <stm32l1xx_iwdg.h>

void watch_dog_config(void);
void enable_wg();

void feed_watchdog(void *p);

#endif