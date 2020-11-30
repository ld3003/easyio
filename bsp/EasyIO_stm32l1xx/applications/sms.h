#ifndef __sms__
#define __sms__

extern void init_sms_callback(void);
extern void send_sms_api(const char *phonenum , const char *smsbody);

#endif