#ifndef __ARCH_TIMER_H__
#define __ARCH_TIMER_H__


#ifdef __cplusplus
extern "C" {
#endif





//Public Defines
#define TIMER_CLOCK			(MCU_CLOCK / 2)







//External Functions
void arch_PwmConf(int nPort, int nPin, int nMode, int nFreq);
void arch_PwmStart(int nPort, int nPin);
void arch_PwmStop(int nPort, int nPin);

void arch_TimerInit(int nId);
void arch_TimerIntClear(int nId);
void arch_TimerStart(int nId, u32 nValue);
void arch_TimerStop(int nId);



#ifdef __cplusplus
}
#endif

#endif


