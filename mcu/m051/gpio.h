#ifndef __ARCH_GPIO_H__
#define __ARCH_GPIO_H__


#ifdef __cplusplus
extern "C" {
#endif









//External Functions
u32 arch_GpioPortBase(uint nPort);
void arch_GpioConf(uint nPort, uint nPin, uint nMode, uint nInit);
void arch_GpioSet(uint nPort, uint nPin, uint nHL);
int arch_GpioRead(uint nPort, uint nPin);





#ifdef __cplusplus
}
#endif

#endif


