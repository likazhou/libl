#if IRQ_ENABLE
/* Includes ------------------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint const lm3s_tblGpioInt[] = {
	INT_GPIOA,
	INT_GPIOB,
	INT_GPIOC,
	INT_GPIOD,
	INT_GPIOE,
	INT_GPIOF,
	INT_GPIOG,
	INT_GPIOH,
	INT_GPIOJ,
};


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
#if 0
static void lm3s_ExtIrqISR(uint nPort)
{
	uint i, nFlag;

	nFlag = MAP_GPIOPinIntStatus(arch_GpioPortBase(nPort), true);
	for (i = 0; i < 8; i++)
	{
		if (nFlag & BITMASK(i))
			irq_ExtISR(nPort * 8 + i);
	}
}
#else
static void lm3s_ExtIrqISR(uint nPort)
{

	MAP_GPIOPinIntClear(arch_GpioPortBase(nPort), 0xFF);	// FIXME
	irq_ExtISR(nPort);
}
#endif

static void lm3s_TimerISR(uint nId)
{

	arch_TimerIntClear(nId);
	irq_TimerISR(nId);
}



//External Functions
int arch_ExtIrqRegister(uint nPort, uint nPin, uint nTriggerMode)
{
	uint nMode;

	MAP_IntEnable(lm3s_tblGpioInt[nPort]);
	if (nTriggerMode == IRQ_TRIGGER_FALLING)
		nMode = GPIO_FALLING_EDGE;
	else
		nMode = GPIO_RISING_EDGE;
	MAP_GPIOIntTypeSet(arch_GpioPortBase(nPort), BITMASK(nPin), nMode);
	return nPort;
}

void arch_ExtIrqEnable(uint nPort, uint nPin, uint nMode)
{
	uint nBase;

	nBase = arch_GpioPortBase(nPort);
	nPin = BITMASK(nPin);
	MAP_GPIOPinIntClear(nBase, nPin);
	MAP_GPIOPinIntEnable(nBase, nPin);
}

void arch_ExtIrqDisable(uint nPort, uint nPin)
{

	MAP_GPIOPinIntDisable(arch_GpioPortBase(nPort), BITMASK(nPin));
}


//Interrupt Functions
void lm3s_isr_porta()
{

#if IRQ_ENABLE
	os_irq_Enter();

	lm3s_ExtIrqISR(0);

	os_irq_Leave();
#endif
}

void lm3s_isr_portb()
{

#if IRQ_ENABLE
	os_irq_Enter();

	lm3s_ExtIrqISR(1);

	os_irq_Leave();
#endif
}

void lm3s_isr_portc()
{

#if IRQ_ENABLE
	os_irq_Enter();

	lm3s_ExtIrqISR(2);

	os_irq_Leave();
#endif
}

void lm3s_isr_portd()
{

#if IRQ_ENABLE
	os_irq_Enter();

	lm3s_ExtIrqISR(3);

	os_irq_Leave();
#endif
}

void lm3s_isr_porte()
{

#if IRQ_ENABLE
    os_irq_Enter();

	lm3s_ExtIrqISR(4);

	os_irq_Leave();

#endif
}

void lm3s_isr_uart0()
{

#if UART_ENABLE
	os_irq_Enter();

#ifdef RT_USING_FINSH
	rt_hw_uart_isr(&serial1);
#else
	arch_UartISR(0);
#endif

	os_irq_Leave();
#endif
}

void lm3s_isr_uart1()
{

#if UART_ENABLE
	os_irq_Enter();
	
	arch_UartISR(1);
	
	os_irq_Leave();
#endif
}

void lm3s_isr_ssi0()
{
}

void lm3s_isr_i2c0()
{
}

void lm3s_isr_pwmfault()
{
}

void lm3s_isr_pwm0()
{
}

void lm3s_isr_pwm1()
{
}

void lm3s_isr_pwm2()
{
}

void lm3s_isr_qei0()
{
}

void lm3s_isr_adc0()
{
}

void lm3s_isr_adc1()
{
}

void lm3s_isr_adc2()
{
}

void lm3s_isr_adc3()
{
}

void lm3s_isr_wdg()
{
}

void lm3s_isr_timer0a()
{

#if IRQ_ENABLE
	os_irq_Enter();

	lm3s_TimerISR(0);

	os_irq_Leave();
#endif
}

void lm3s_isr_timer0b()
{

#if IRQ_ENABLE
	os_irq_Enter();

	lm3s_TimerISR(1);

	os_irq_Leave();
#endif
}

void lm3s_isr_timer1a()
{

#if IRQ_ENABLE
	os_irq_Enter();

	lm3s_TimerISR(2);

	os_irq_Leave();
#endif
}

void lm3s_isr_timer1b()
{

#if IRQ_ENABLE
	os_irq_Enter();

	lm3s_TimerISR(3);

	os_irq_Leave();
#endif
}

void lm3s_isr_timer2a()
{

#if IRQ_ENABLE
	os_irq_Enter();

	lm3s_TimerISR(4);

	os_irq_Leave();
#endif
}

void lm3s_isr_timer2b()
{

#if IRQ_ENABLE
	os_irq_Enter();

	lm3s_TimerISR(5);

	os_irq_Leave();
#endif
}

void lm3s_isr_comp0()
{
}

void lm3s_isr_comp1()
{
}

void lm3s_isr_comp2()
{
}

void lm3s_isr_sysctrl()
{
}

void lm3s_isr_flashctrl()
{
}

void lm3s_isr_portf()
{

#if IRQ_ENABLE
	os_irq_Enter();

	lm3s_ExtIrqISR(5);

	os_irq_Leave();
#endif
}

void lm3s_isr_portg()
{

#if IRQ_ENABLE
	os_irq_Enter();

	lm3s_ExtIrqISR(6);

	os_irq_Leave();
#endif
}

void lm3s_isr_porth()
{

#if IRQ_ENABLE
	os_irq_Enter();

	lm3s_ExtIrqISR(7);

	os_irq_Leave();
#endif
}

void lm3s_isr_uart2()
{

#if UART_ENABLE
	os_irq_Enter();
	
	arch_UartISR(2);
	
	os_irq_Leave();
#endif
}

void lm3s_isr_ssi1()
{
}

void lm3s_isr_timer3a()
{

#if IRQ_ENABLE
	os_irq_Enter();

	lm3s_TimerISR(6);

	os_irq_Leave();
#endif
}

void lm3s_isr_timer3b()
{

#if IRQ_ENABLE
	os_irq_Enter();

	lm3s_TimerISR(7);

	os_irq_Leave();
#endif
}

void lm3s_isr_i2c1()
{
}

void lm3s_isr_qei1()
{
}

void lm3s_isr_can0()
{
}

void lm3s_isr_can1()
{
}

void lm3s_isr_can2()
{
}

void lm3s_isr_eth()
{
	extern void arch_EmacIsr(void);

#if ETH_INT_ENABLE
	os_irq_Enter();

	arch_EmacIsr();

	os_irq_Leave();
#endif
}

void lm3s_isr_hibernate0()
{
}

void lm3s_isr_usb0()
{
	extern void USB0HostIntHandler(void);

#if USB_ENABLE
	os_irq_Enter();

	USB0HostIntHandler();

	os_irq_Leave();
#endif
}

void lm3s_isr_pwm3()
{
}

void lm3s_isr_udma()
{
}

void lm3s_isr_udma_error()
{
}

#endif

