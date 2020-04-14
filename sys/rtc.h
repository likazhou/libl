#ifndef __SYS_RTC_H__
#define __SYS_RTC_H__


#ifdef __cplusplus
extern "C" {
#endif
#include <time.h>






//Public Defines
#define RTC_T_OSTICK			0
#define RTC_T_INT				1
#define RTC_T_RNSS				2
	
#define RTC_T_I2C				BITMASK(7)
#define RTC_T_ISL1202X			(RTC_T_I2C | 0)
#define RTC_T_DS323X			(RTC_T_I2C | 1)
#define RTC_T_R202X				(RTC_T_I2C | 2)
#define RTC_T_RX802X			(RTC_T_I2C | 3)
#define RTC_T_M41T8X			(RTC_T_I2C | 4)
#define RTC_T_PCF8526X			(RTC_T_I2C | 5)





//External Functions
sys_res rtc_Init(void);
void rtc_OsTick(void);
time_t rtc_GetTimet(void);
struct tm *rtc_pTm(void);
sys_res rtc_SetTimet(time_t tTime);


#ifdef __cplusplus
}
#endif

#endif

