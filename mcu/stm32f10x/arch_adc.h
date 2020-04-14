#ifndef __ARCH_ADC_H__
#define __ARCH_ADC_H__



#ifdef __cplusplus
extern "C" {
#endif








//External Functions
void arch_AdcInit(void);
u32 arch_AdcData(int nPort, int nPin);


#ifdef __cplusplus
}
#endif

#endif

