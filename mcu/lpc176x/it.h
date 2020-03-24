#ifndef __ARCH_IT_H
#define __ARCH_IT_H


#ifdef __cplusplus
extern "C" {
#endif





/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define ARCH_EXTIRQ_QTY			32
#define ARCH_TIMER_QTY			4

#define arch_ExtIrqRxConf(...)


/* Exported functions ------------------------------------------------------- */
int arch_ExtIrqRegister(uint nPort, uint nPin, uint nTriggerMode);
void arch_ExtIrqEnable(uint nPort, uint nPin, uint nMode);
void arch_ExtIrqDisable(uint nPort, uint nPin);



		


#ifdef __cplusplus
}
#endif

#endif

