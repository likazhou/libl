#ifndef __ISL1202X_H__
#define __ISL1202X_H__








//External Functions
sys_res rtc_i2c_Init(i2c_t *p);
sys_res rtc_i2c_GetTime(i2c_t *p, time_t *pTime);
sys_res rtc_i2c_SetTime(i2c_t *p, time_t tTime);


#endif

