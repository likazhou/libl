#ifndef __ARCH_RTC_H__
#define __ARCH_RTC_H__




//External Functions
void arch_RtcInit(void);

#define arch_RtcGet(t)			RTC_CounterValue(t)
#define arch_RtcSet(t)			RTC_CounterConfig(t)



#endif

