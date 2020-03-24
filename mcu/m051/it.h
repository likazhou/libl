#ifndef __ARCH_IT_H
#define __ARCH_IT_H


#ifdef __cplusplus
extern "C" {
#endif





/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define ARCH_EXTIRQ_QTY			8
#define ARCH_TIMER_QTY			8


/* Exported functions ------------------------------------------------------- */
int arch_ExtIrqRegister(uint nPort, uint nPin, uint nTriggerMode);
void arch_ExtIrqEnable(uint nPort, uint nPin, uint nMode);
void arch_ExtIrqDisable(uint nPort, uint nPin);



void lm3s_isr_porta(void);
void lm3s_isr_portb(void);
void lm3s_isr_portc(void);
void lm3s_isr_portd(void);
void lm3s_isr_porte(void);
void lm3s_isr_uart_0(void);
void lm3s_isr_uart_1(void);
void lm3s_isr_ssi0(void);
void lm3s_isr_i2c0(void);
void lm3s_isr_pwmfault(void);
void lm3s_isr_pwm0(void);
void lm3s_isr_pwm1(void);
void lm3s_isr_pwm2(void);
void lm3s_isr_qei0(void);
void lm3s_isr_adc0(void);
void lm3s_isr_adc1(void);
void lm3s_isr_adc2(void);
void lm3s_isr_adc3(void);
void lm3s_isr_wdg(void);
void lm3s_isr_timer0a(void);
void lm3s_isr_timer0b(void);
void lm3s_isr_timer1a(void);
void lm3s_isr_timer1b(void);
void lm3s_isr_timer2a(void);
void lm3s_isr_timer2b(void);
void lm3s_isr_comp0(void);
void lm3s_isr_comp1(void);
void lm3s_isr_comp2(void);
void lm3s_isr_sysctrl(void);
void lm3s_isr_flashctrl(void);
void lm3s_isr_portf(void);
void lm3s_isr_portg(void);
void lm3s_isr_porth(void);
void lm3s_isr_uart2(void);
void lm3s_isr_ssi1(void);
void lm3s_isr_timer3a(void);
void lm3s_isr_timer3b(void);
void lm3s_isr_i2c1(void);
void lm3s_isr_qei1(void);
void lm3s_isr_can0(void);
void lm3s_isr_can1(void);
void lm3s_isr_can2(void);
void lm3s_isr_eth(void);
void lm3s_isr_hibernate0(void);
void lm3s_isr_usb0(void);
void lm3s_isr_pwm3(void);
void lm3s_isr_udma(void);
void lm3s_isr_udma_error(void);
		


#ifdef __cplusplus
}
#endif

#endif


