/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : stm32f10x_it.c
* Author             : MCD Application Team
* Version            : V2.0.1
* Date               : 06/13/2008
* Description        : Main Interrupt Service Routines.
*                      This file provides template for all exceptions handler
*                      and peripherals interrupt service routine.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



int arch_ExtIrqRegister(uint nPort, uint nPin, uint nTriggerMode)
{
	EXTI_InitTypeDef xEXTI;
	uint nPortSource;

	switch (nPort)
	{
	case GPIO_P0:
		nPortSource = GPIO_PortSourceGPIOA;
		break;
	case GPIO_P1:
		nPortSource = GPIO_PortSourceGPIOB;
		break;
	case GPIO_P2:
		nPortSource = GPIO_PortSourceGPIOC;
		break;
	case GPIO_P3:
		nPortSource = GPIO_PortSourceGPIOD;
		break;
	case GPIO_P4:
		nPortSource = GPIO_PortSourceGPIOE;
		break;
	case GPIO_P5:
		nPortSource = GPIO_PortSourceGPIOF;
		break;
	case GPIO_P6:
		nPortSource = GPIO_PortSourceGPIOG;
		break;	
	default:
		return -1;
	}
	GPIO_EXTILineConfig(nPortSource, nPin);
	
	xEXTI.EXTI_Line = BITMASK(nPin);
	xEXTI.EXTI_Mode = EXTI_Mode_Interrupt;
	switch (nTriggerMode)
	{
	case IRQ_TRIGGER_RISING:
		xEXTI.EXTI_Trigger = EXTI_Trigger_Rising;
		break;
	default:
		xEXTI.EXTI_Trigger = EXTI_Trigger_Falling;
		break;
	}
	xEXTI.EXTI_LineCmd = ENABLE;
	EXTI_Init(&xEXTI);
	
	return nPin;
}


void arch_ExtIrqEnable(uint nPort, uint nPin, uint nMode)
{ 
	EXTI_InitTypeDef xEXTI;

	xEXTI.EXTI_Line = BITMASK(nPin);
	xEXTI.EXTI_Mode = EXTI_Mode_Interrupt;
	
	switch (nMode)
	{
	case IRQ_TRIGGER_RISING:
		xEXTI.EXTI_Trigger = EXTI_Trigger_Rising;
		break;
	default:
		xEXTI.EXTI_Trigger = EXTI_Trigger_Falling;
		break;
	}
	
	xEXTI.EXTI_LineCmd = ENABLE;
	EXTI_Init(&xEXTI);
}

void arch_ExtIrqDisable(uint nPort, uint nPin)
{
	EXTI_InitTypeDef xEXTI;

	xEXTI.EXTI_Line = BITMASK(nPin);
	xEXTI.EXTI_LineCmd = DISABLE;
	EXTI_Init(&xEXTI);
}

/*******************************************************************************
* Function Name  : NMIException
* Description    : This function handles NMI exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void NMIException(void)
{
}

/*******************************************************************************
* Function Name  : HardFaultException
* Description    : This function handles Hard Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void HardFaultException(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : MemManageException
* Description    : This function handles Memory Manage exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void MemManageException(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : BusFaultException
* Description    : This function handles Bus Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void BusFaultException(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : UsageFaultException
* Description    : This function handles Usage Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void UsageFaultException(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : DebugMonitor
* Description    : This function handles Debug Monitor exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void DebugMonitor(void)
{
}

/*******************************************************************************
* Function Name  : SVCHandler
* Description    : This function handles SVCall exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void SVCHandler(void)
{
}

/*******************************************************************************
* Function Name  : PendSVC
* Description    : This function handles PendSVC exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void PendSVC(void)
{
}

/*******************************************************************************
* Function Name  : SysTickHandler
* Description    : This function handles SysTick Handler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void SysTickHandler(void)
{
}

/*******************************************************************************
* Function Name  : WWDG_IRQHandler
* Description    : This function handles WWDG interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void WWDG_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : PVD_IRQHandler
* Description    : This function handles PVD interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void PVD_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TAMPER_IRQHandler
* Description    : This function handles Tamper interrupt request. 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void TAMPER_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : RTC_IRQHandler
* Description    : This function handles RTC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void RTC_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : FLASH_IRQHandler
* Description    : This function handles Flash interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void FLASH_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : RCC_IRQHandler
* Description    : This function handles RCC interrupt request. 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void RCC_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : EXTI0_IRQHandler
* Description    : This function handles External interrupt Line 0 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void EXTI0_IRQHandler(void)
{

#if IRQ_ENABLE
	os_irq_Enter();

	EXTI->PR = BITMASK(0);
	irq_ExtISR(0);

	os_irq_Leave();
#endif
}

/*******************************************************************************
* Function Name  : EXTI1_IRQHandler
* Description    : This function handles External interrupt Line 1 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void EXTI1_IRQHandler(void)
{

#if IRQ_ENABLE
	os_irq_Enter();

	EXTI->PR = BITMASK(1);
	irq_ExtISR(1);

	os_irq_Leave();
#endif
}

/*******************************************************************************
* Function Name  : EXTI2_IRQHandler
* Description    : This function handles External interrupt Line 2 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void EXTI2_IRQHandler(void)
{

#if IRQ_ENABLE
	os_irq_Enter();

	EXTI->PR = BITMASK(2);
	irq_ExtISR(2);

	os_irq_Leave();
#endif
}

/*******************************************************************************
* Function Name  : EXTI3_IRQHandler
* Description    : This function handles External interrupt Line 3 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void EXTI3_IRQHandler(void)
{

#if IRQ_ENABLE
	os_irq_Enter();

	EXTI->PR = BITMASK(3);
	irq_ExtISR(3);

	os_irq_Leave();
#endif
}

/*******************************************************************************
* Function Name  : EXTI4_IRQHandler
* Description    : This function handles External interrupt Line 4 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void EXTI4_IRQHandler(void)
{

#if IRQ_ENABLE
	os_irq_Enter();

	EXTI->PR = BITMASK(4);
	irq_ExtISR(4);

	os_irq_Leave();
#endif
}

/*******************************************************************************
* Function Name  : DMA1_Channel1_IRQHandler
* Description    : This function handles DMA1 Channel 1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void DMA1_Channel1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA1_Channel2_IRQHandler
* Description    : This function handles DMA1 Channel 2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void DMA1_Channel2_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA1_Channel3_IRQHandler
* Description    : This function handles DMA1 Channel 3 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void DMA1_Channel3_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA1_Channel4_IRQHandler
* Description    : This function handles DMA1 Channel 4 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void DMA1_Channel4_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA1_Channel5_IRQHandler
* Description    : This function handles DMA1 Channel 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void DMA1_Channel5_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA1_Channel6_IRQHandler
* Description    : This function handles DMA1 Channel 6 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void DMA1_Channel6_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA1_Channel7_IRQHandler
* Description    : This function handles DMA1 Channel 7 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void DMA1_Channel7_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : ADC1_2_IRQHandler
* Description    : This function handles ADC1 and ADC2 global interrupts requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void ADC1_2_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : USB_HP_CAN_TX_IRQHandler
* Description    : This function handles USB High Priority or CAN TX interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void CAN1_TX_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : USB_LP_CAN_RX0_IRQHandler
* Description    : This function handles USB Low Priority or CAN RX0 interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void CAN1_RX0_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : CAN_RX1_IRQHandler
* Description    : This function handles CAN RX1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void CAN1_RX1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : CAN_SCE_IRQHandler
* Description    : This function handles CAN SCE interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void CAN1_SCE_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : EXTI9_5_IRQHandler
* Description    : This function handles External lines 9 to 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void EXTI9_5_IRQHandler(void)
{
#if IRQ_ENABLE
	uint i, nIS;

	os_irq_Enter();

	while ((nIS = EXTI->PR & 0x03E0) != 0)
	{
		for (i = 5; i < 10; i++)
		{
			if (nIS & BITMASK(i))
			{
				EXTI->PR = BITMASK(i);
				irq_ExtISR(i);
			}
		}
	}

	os_irq_Leave();
#endif
}

/*******************************************************************************
* Function Name  : TIM1_BRK_IRQHandler
* Description    : This function handles TIM1 Break interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void TIM1_BRK_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM1_UP_IRQHandler
* Description    : This function handles TIM1 overflow and update interrupt 
*                  request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void TIM1_UP_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM1_TRG_COM_IRQHandler
* Description    : This function handles TIM1 Trigger and commutation interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void TIM1_TRG_COM_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM1_CC_IRQHandler
* Description    : This function handles TIM1 capture compare interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void TIM1_CC_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : This function handles TIM2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void TIM2_IRQHandler(void)
{

#if IRQ_TIMER_ENABLE
	os_irq_Enter();

	irq_TimerISR(0);
	arch_TimerIntClear(0);

	os_irq_Leave();
#endif
}

/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles TIM3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void TIM3_IRQHandler(void)
{

#if IRQ_TIMER_ENABLE
	os_irq_Enter();

	irq_TimerISR(1);
	arch_TimerIntClear(1);

	os_irq_Leave();
#endif
}

/*******************************************************************************
* Function Name  : TIM4_IRQHandler
* Description    : This function handles TIM4 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void TIM4_IRQHandler(void)
{

#if IRQ_TIMER_ENABLE
	os_irq_Enter();

	irq_TimerISR(2);
	arch_TimerIntClear(2);

	os_irq_Leave();
#endif
}

/*******************************************************************************
* Function Name  : I2C1_EV_IRQHandler
* Description    : This function handles I2C1 Event interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void I2C1_EV_IRQHandler(void)
{

#if I2C_ENABLE
	os_irq_Enter();

#if I2C_IRQ_ENABLE
	arch_I2cIoHandler(i2c_Dev(0));
#endif

	os_irq_Leave();
#endif
}

/*******************************************************************************
* Function Name  : I2C1_ER_IRQHandler
* Description    : This function handles I2C1 Error interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void I2C1_ER_IRQHandler(void)
{

#if I2C_ENABLE
	os_irq_Enter();

#if I2C_IRQ_ENABLE
	arch_I2cErrorHandler(i2c_Dev(0));
#endif

	os_irq_Leave();
#endif
}

/*******************************************************************************
* Function Name  : I2C2_EV_IRQHandler
* Description    : This function handles I2C2 Event interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void I2C2_EV_IRQHandler(void)
{

#if I2C_ENABLE
	os_irq_Enter();

#if I2C_IRQ_ENABLE
	arch_I2cIoHandler(i2c_Dev(1));
#endif

	os_irq_Leave();
#endif
}

/*******************************************************************************
* Function Name  : I2C2_ER_IRQHandler
* Description    : This function handles I2C2 Error interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void I2C2_ER_IRQHandler(void)
{

#if I2C_ENABLE
	os_irq_Enter();

#if I2C_IRQ_ENABLE
	arch_I2cErrorHandler(i2c_Dev(1));
#endif

	os_irq_Leave();
#endif
}

/*******************************************************************************
* Function Name  : SPI1_IRQHandler
* Description    : This function handles SPI1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void SPI1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : SPI2_IRQHandler
* Description    : This function handles SPI2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void SPI2_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void USART1_IRQHandler(void)
{

#if UART_ENABLE
	os_irq_Enter();

#ifdef RT_USING_FINSH
	rt_hw_serial_isr(&uart1_device);
#else
	arch_UartISR(0);
#endif

	os_irq_Leave();
#endif
}

/*******************************************************************************
* Function Name  : USART2_IRQHandler
* Description    : This function handles USART2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void USART2_IRQHandler(void)
{

#if UART_ENABLE
	os_irq_Enter();

	arch_UartISR(1);

	os_irq_Leave();
#endif
}

/*******************************************************************************
* Function Name  : USART3_IRQHandler
* Description    : This function handles USART3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void USART3_IRQHandler(void)
{

#if UART_ENABLE
	os_irq_Enter();

	arch_UartISR(2);

	os_irq_Leave();
#endif
}

/*******************************************************************************
* Function Name  : EXTI15_10_IRQHandler
* Description    : This function handles External lines 15 to 10 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void EXTI15_10_IRQHandler(void)
{
#if IRQ_ENABLE
	uint i, nIS;

	os_irq_Enter();

	while ((nIS = EXTI->PR & 0xFC00) != 0)
	{
		for (i = 10; i < 16; i++)
		{
			if (nIS & BITMASK(i))
			{
				EXTI->PR = BITMASK(i);
				irq_ExtISR(i);
			}
		}
	}
			
	os_irq_Leave();
#endif
}

/*******************************************************************************
* Function Name  : RTCAlarm_IRQHandler
* Description    : This function handles RTC Alarm interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void RTCAlarm_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : USBWakeUp_IRQHandler
* Description    : This function handles USB WakeUp interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void OTG_FS_WKUP_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM8_BRK_IRQHandler
* Description    : This function handles TIM8 Break interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void TIM8_BRK_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM8_UP_IRQHandler
* Description    : This function handles TIM8 overflow and update interrupt 
*                  request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void TIM8_UP_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM8_TRG_COM_IRQHandler
* Description    : This function handles TIM8 Trigger and commutation interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void TIM8_TRG_COM_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM8_CC_IRQHandler
* Description    : This function handles TIM8 capture compare interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void TIM8_CC_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : ADC3_IRQHandler
* Description    : This function handles ADC3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void ADC3_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : FSMC_IRQHandler
* Description    : This function handles FSMC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void FSMC_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : SDIO_IRQHandler
* Description    : This function handles SDIO global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void SDIO_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM5_IRQHandler
* Description    : This function handles TIM5 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void TIM5_IRQHandler(void)
{

#if IRQ_TIMER_ENABLE
	os_irq_Enter();

	irq_TimerISR(3);
	arch_TimerIntClear(3);

	os_irq_Leave();
#endif
}

/*******************************************************************************
* Function Name  : SPI3_IRQHandler
* Description    : This function handles SPI3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void SPI3_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : UART4_IRQHandler
* Description    : This function handles UART4 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void UART4_IRQHandler(void)
{

#if UART_ENABLE
	os_irq_Enter();

	arch_UartISR(3);

	os_irq_Leave();
#endif
}

/*******************************************************************************
* Function Name  : UART5_IRQHandler
* Description    : This function handles UART5 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void UART5_IRQHandler(void)
{

#if UART_ENABLE
	os_irq_Enter();

	arch_UartISR(4);

	os_irq_Leave();
#endif
}

/*******************************************************************************
* Function Name  : TIM6_IRQHandler
* Description    : This function handles TIM6 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void TIM6_IRQHandler(void)
{

#if IRQ_TIMER_ENABLE
	os_irq_Enter();

	irq_TimerISR(4);
	arch_TimerIntClear(4);

	os_irq_Leave();
#endif
}

/*******************************************************************************
* Function Name  : TIM7_IRQHandler
* Description    : This function handles TIM7 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void TIM7_IRQHandler(void)
{

#if IRQ_TIMER_ENABLE
	os_irq_Enter();

	irq_TimerISR(5);
	arch_TimerIntClear(5);

	os_irq_Leave();
#endif
}

/*******************************************************************************
* Function Name  : DMA2_Channel1_IRQHandler
* Description    : This function handles DMA2 Channel 1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void DMA2_Channel1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA2_Channel2_IRQHandler
* Description    : This function handles DMA2 Channel 2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void DMA2_Channel2_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA2_Channel3_IRQHandler
* Description    : This function handles DMA2 Channel 3 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void DMA2_Channel3_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA2_Channel4_5_IRQHandler
* Description    : This function handles DMA2 Channel 4 and DMA2 Channel 5
*                  interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__weak void DMA2_Channel4_IRQHandler(void)
{
}

__weak void DMA2_Channel5_IRQHandler(void)
{
}

__weak void ETH_IRQHandler(void)
{

#if ETH_INT_ENABLE
	os_irq_Enter();

	arch_EmacIsr();

	os_irq_Leave();
#endif
}

//Ethernet Wakeup through EXTI line
__weak void ETH_WKUP_IRQHandler(void)
{

}

__weak void CAN2_TX_IRQHandler(void)
{

}

__weak void CAN2_RX0_IRQHandler(void)
{

}

__weak void CAN2_RX1_IRQHandler(void)
{

}

__weak void CAN2_SCE_IRQHandler(void)
{

}

__weak void OTG_FS_IRQHandler(void)
{
	extern void usb_HostIRQ(void);

#if USB_ENABLE
#if OS_TYPE
	os_irq_enter();
#endif

	usb_HostIRQ();

#if OS_TYPE
	os_irq_Leave();
#endif
#endif
}


/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/


