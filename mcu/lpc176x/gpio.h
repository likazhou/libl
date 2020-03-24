#ifndef __ARCH_GPIO_H__
#define __ARCH_GPIO_H__


#ifdef __cplusplus
extern "C" {
#endif






//External Functions
void arch_GpioClockEnable(uint nPort);
void arch_GpioSel(uint nPort, uint nPin, uint nSel);
void arch_GpioConf(uint nPort, uint nPin, uint nMode, uint nInit);
void arch_GpioSet(uint nPort, uint nPin, uint nHL);
int arch_GpioRead(uint nPort, uint nPin);





#ifdef __cplusplus
}
#endif

#endif


