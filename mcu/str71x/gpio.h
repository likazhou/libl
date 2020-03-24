#ifndef __ARCH_GPIO_H__
#define __ARCH_GPIO_H__






//External Functions
GPIO_TypeDef *str7_GpioPortDef(int nPort);
void arch_GpioConf(p_gpio_def p);
void str7_GpioIdleInit(void);
void str7_GpioClockEnable(int nPort);
void arch_GpioSet(p_gpio_def p, uint nHL);
int arch_GpioRead(p_gpio_def p);


#endif


