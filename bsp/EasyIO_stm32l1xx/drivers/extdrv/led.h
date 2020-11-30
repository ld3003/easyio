#ifndef __led__
#define __led__

#include <stm32l1xx_gpio.h>

void init_led_hw(void);
void led_on(unsigned char index);
void led_off(unsigned char index);
void led_off_all(void);

void __enable_AMP_EN(void);
void __disable_AMP_EN(void);

void init_flicker_led_tmr(void);
void __flicker_led(unsigned char index , unsigned int ms);


#define LED_R		0
#define LED_G		2
#define LED_B		1

struct LED_FLICKER{
	
	//public
	unsigned char mode;
	unsigned short busy_ms_max;
	unsigned short idle_ms_max;
	
	//private:
	unsigned short busy_ms;
	unsigned short idle_ms;

};

//led 控制变量
extern struct LED_FLICKER led_flicker[];

enum LEDSTATUS{
	
	RGB_LED_OFF,							//所有LED关闭
	
	RED_STATUS_ON,
	RED_STATUS_CHARGE,
	
	GREEN_STATUS_GPSOFF,
	GREEN_STATUS_GPSV,
	GREEN_STATUS_GPSA,
	
	BLUE_STATUS_NOSIMCARD,
	BLUE_STATUS_REGISTER,
	BLUE_STATUS_LOGIN,
	BLUE_STATUS_SUCCESS,
	
	LED_STATUS_FAST,
	LED_STATUS_ON,
	
};

extern void update_led_mod(void);

extern char ledstatus[3];

#define SET_LED_MOD(EX1,EX2)		ledstatus[EX1]=EX2;update_led_mod();


void sos_key_press(void *p);
void sos_key_release(void *p);
void sos_long_press(void *p);
void sos_long_release(void *p);


extern unsigned int pwr_press_time;
extern unsigned int func_press_time;
extern unsigned int charge_disable_tim;

void pwr_key_press(void *p);
void pwr_key_release(void *p);
void pwr_key_long_press(void *p);
void pwr_key_long_release(void *p);

void func_key_press(void *p);
void func_key_release(void *p);
void func_key_long_press(void *p);
void func_key_long_release(void *p);

uint8_t __is_charge(void);
#define IS_CHARGE __is_charge()
#define POWER_KEY GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7)
#define FUNC_KEY !GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)

extern unsigned char disable_RGB_led;



#endif