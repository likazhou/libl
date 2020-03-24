#ifndef __ARCH_GPIO_H__
#define __ARCH_GPIO_H__


#ifdef __cplusplus
extern "C" {
#endif









//External Functions
GPIO_TypeDef *arch_GpioPortBase(int nPort);
void arch_GpioConf(int nPort, int nPin, int nMode, int nInit);
void arch_GpioSet(int nPort, int nPin, int nHL);
int arch_GpioRead(int nPort, int nPin);



#ifdef __cplusplus
}
#endif

#endif

