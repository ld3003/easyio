#ifndef __sim900__
#define __sim900__



extern void init_modem_hw(void);
extern void init_sim900_modem(void);
extern void init_sim900_modem_chk(void);
extern void enable_dtr(void);
extern void enable_dtr_always(char on);
extern void disable_dtr(void);
extern void disable_dtr2(void);
extern void close_max9860(void);
extern void open_max9860(void);
extern void config_ring_interrupt(void);
extern void config_simdet(void);
extern void check_3gmodem_active(void);

#define IS_SARAU2_ACTIVE GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)

void open_x9860(void);
#endif