
//Private Const
static TIM_TypeDef * const stm32_tblTimBase[ARCH_TIMER_QTY] = {
	TIM2,
	TIM3,
	TIM4,
	TIM5,
	TIM6,
	TIM7,
};

static const u32 stm32_tblTimAPB[ARCH_TIMER_QTY] = {
	RCC_APB1Periph_TIM2,
	RCC_APB1Periph_TIM3,
	RCC_APB1Periph_TIM4,
	RCC_APB1Periph_TIM5,
	RCC_APB1Periph_TIM6,
	RCC_APB1Periph_TIM7,
};

static const IRQn_Type stm32_tblTimIRQn[ARCH_TIMER_QTY] = {
	TIM2_IRQn,
	TIM3_IRQn,
	TIM4_IRQn,
#if ARCH_TYPE != ARCH_T_STM32F10X_MD
	TIM5_IRQn,
	TIM6_IRQn,
	TIM7_IRQn,
#endif
};

void arch_TimerInit(int nId)
{
	NVIC_InitTypeDef xNVIC;

	xNVIC.NVIC_IRQChannel = stm32_tblTimIRQn[nId];
	xNVIC.NVIC_IRQChannelPreemptionPriority = 0;
	xNVIC.NVIC_IRQChannelSubPriority = 0;
	xNVIC.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&xNVIC);

	RCC_APB1PeriphClockCmd(stm32_tblTimAPB[nId], ENABLE);
	TIM_DeInit(stm32_tblTimBase[nId]);
}

void arch_TimerIntClear(int nId)
{

	TIM_ClearITPendingBit(stm32_tblTimBase[nId], TIM_IT_Update);
}

void arch_TimerStart(int nId, u32 nValue)
{
	TIM_TimeBaseInitTypeDef xTIM_TimeBase;
	TIM_TypeDef * const	pTim = stm32_tblTimBase[nId];

	TIM_DeInit(pTim);
	xTIM_TimeBase.TIM_Period = nValue;          //ARR
	xTIM_TimeBase.TIM_Prescaler = 0;			//PSC
	xTIM_TimeBase.TIM_ClockDivision = TIM_CKD_DIV1;
	xTIM_TimeBase.TIM_CounterMode = TIM_CounterMode_Up; //计数方式
	TIM_TimeBaseInit(pTim, &xTIM_TimeBase);
	TIM_ClearFlag(pTim, TIM_FLAG_Update);
	TIM_Cmd(pTim, ENABLE); //使能
	TIM_ITConfig(pTim, TIM_IT_Update, ENABLE);
}

void arch_TimerStop(int nId)
{
	TIM_TypeDef * const	pTim = stm32_tblTimBase[nId];

	TIM_ClearFlag(pTim, TIM_FLAG_Update);  
	TIM_Cmd(pTim, DISABLE); //
}



