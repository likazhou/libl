#ifndef __ARCH_GPIO_H__
#define __ARCH_GPIO_H__


#ifdef __cplusplus
extern "C" {
#endif









//External Functions
GPIO_TypeDef *arch_GpioPortBase(int port);
void arch_GpioClockEnable(int port);
void arch_GpioConf(int port, int pin, int mode, int init);
void arch_GpioSet(int port, int pin, int HL);
int arch_GpioRead(int port, int pin);




#ifdef __cplusplus
}
#endif

#endif


