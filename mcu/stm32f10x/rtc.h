#ifndef __ARCH_RTC_H__
#define __ARCH_RTC_H__


#ifdef __cplusplus
extern "C" {
#endif













//External Functions
void arch_RtcInit(void);

#define arch_RtcGet(t)			RTC_GetCounter(t)
#define arch_RtcSet(t)			RTC_SetCounter(t)



#ifdef __cplusplus
}
#endif

#endif

