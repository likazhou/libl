#ifndef __RX802X_H__
#define __RX802X_H__



#ifdef __cplusplus
extern "C" {
#endif








//External Functions
sys_res rtc_i2c_Init(i2c_t *p);
sys_res rtc_i2c_GetTime(i2c_t *p, time_t *pTime);
sys_res rtc_i2c_SetTime(i2c_t *p, time_t tTime);


#ifdef __cplusplus
}
#endif


#endif



