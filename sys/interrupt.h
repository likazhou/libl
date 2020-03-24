#ifndef __SYS_INTERRUPT_H__
#define __SYS_INTERRUPT_H__


#ifdef __cplusplus
extern "C" {
#endif





//Public Defines
#define IRQ_TRIGGER_FALLING		0
#define IRQ_TRIGGER_RISING		1

#define IRQ_MODE_NORMAL			0
#define IRQ_MODE_HALF			1


//Public Typedefs
typedef struct {
	u8 mode;
#if IRQ_HALF_ENABLE
	u8 bottom;
#endif
	u8 port;
	u8 pin;
	void (*handler)(void *);
	void *args;
} t_irq_vector;





//External Functions
void irq_Init(void);
void irq_VectorInit(void);
sys_res irq_Wait(void);
int irq_ExtRegister(int nPort, int nPin, int nTriggerMode, void (*pHandler)(void *), 
							void *args, int nHandlerMode);
void irq_ExtEnable(int nId);
void irq_ExtDisable(int nId);
void irq_ExtISR(int nId);
sys_res irq_TimerRegister(int nId, void (*pHandler)(void *), void *args);
void irq_TimerStart(int nId, u32 nValue);
void irq_TimerStop(int nId);
void irq_TimerISR(int nId);
void irq_BottomHandler(void);



#ifdef __cplusplus
}
#endif

#endif

