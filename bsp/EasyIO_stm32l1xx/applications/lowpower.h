#ifndef __lowpower__
#define __lowpower__

enum LOWPOWER_STATE {
	GPS_INIT,
	GPS_ON,
	GPS_OFF,
	GPS_NOPOS,
};

extern int GPS_LOWPOWER_STATUS;

void lowpower_routing(void *p);
void open_gps_2min(void);
void open_gps_5min(void);

void config_fq_1(void);
void config_fq_2(void);

#endif