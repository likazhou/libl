#ifndef __ARCH_TIMER_H__
#define __ARCH_TIMER_H__


#ifdef __cplusplus
extern "C" {
#endif













//External Functions
void arch_TimerInit(uint nId);
void arch_TimerIntClear(uint nId);
void arch_TimerStart(uint nId, uint nValue);
void arch_TimerStop(uint nId);
uint arch_TimerClockGet(void);


#ifdef __cplusplus
}
#endif

#endif

