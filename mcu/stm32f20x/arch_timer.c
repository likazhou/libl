//Private Const
static TIM_TypeDef * const stm32_tblTimBase[ARCH_TIMER_QTY] = {
	TIM2,
	TIM4,
	TIM5,
	TIM6,
	TIM7,
	//PWM
	TIM3,
};

static u32 const stm32_tblTimAPB[ARCH_TIMER_QTY] = {
	RCC_APB1Periph_TIM2,
	RCC_APB1Periph_TIM4,
	RCC_APB1Periph_TIM5,
	RCC_APB1Periph_TIM6,
	RCC_APB1Periph_TIM7,
	RCC_APB1Periph_TIM3,
};

static IRQn_Type const stm32_tblTimIRQn[ARCH_TIMER_QTY] = {
	TIM2_IRQn,
	TIM4_IRQn,
	TIM5_IRQn,
	TIM6_DAC_IRQn,
	TIM7_IRQn,
};


static int arch_PwmTimId(int nPort, int nPin)
{

	switch (nPort)
	{
	case GPIO_P2:
		return 5;
	default:
		return -1;
	}
}

static void arch_PwmAFConf(int nPort, int nPin)
{

	switch (nPort)
	{
	case GPIO_P2:
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM3);
		break;
	default:
		break;
	}
}

void arch_PwmConf(int nPort, int nPin, int nMode, int nFreq)
{
	GPIO_InitTypeDef xGpio;
	TIM_TimeBaseInitTypeDef xTIM_TimeBase;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_TypeDef *pTim;
	int nId;

	nId = arch_PwmTimId(nPort, nPin);
	if (nId < 0)
		return;
	
	pTim = stm32_tblTimBase[nId];
	xGpio.GPIO_Speed = GPIO_Speed_50MHz;
	xGpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	xGpio.GPIO_Pin = BITMASK(nPin);
	xGpio.GPIO_Mode = GPIO_Mode_AF;
	if (nMode == GPIO_M_OUT_OD)
		xGpio.GPIO_OType = GPIO_OType_OD;
	else
		xGpio.GPIO_OType = GPIO_OType_PP;
	
	arch_GpioClockEnable(nPort);
	GPIO_Init(arch_GpioPortBase(nPort), &xGpio);
	arch_PwmAFConf(nPort, nPin);

	//Set PWM Register
	arch_TimerInit(nId);
	
	nFreq = TIMER_CLOCK / nFreq;
    xTIM_TimeBase.TIM_Period = nFreq;       //ARR
    xTIM_TimeBase.TIM_Prescaler = 0;			//PSC
    xTIM_TimeBase.TIM_ClockDivision = TIM_CKD_DIV1; 
    xTIM_TimeBase.TIM_CounterMode = TIM_CounterMode_Up; //计数方式
	TIM_TimeBaseInit(pTim, &xTIM_TimeBase); 

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; 				   //设置为pwm1输出模式
	TIM_OCInitStructure.TIM_Pulse = nFreq / 2;							//设置占空比时间
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;			 //设置输出极性
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 	   //使能该通道输出
	TIM_OC3Init(pTim, &TIM_OCInitStructure); 			//按照指定参数初始化

	TIM_OC3PreloadConfig(pTim, TIM_OCPreload_Enable);  //使能TIMx在CCR1上的预装载寄存器
	TIM_ARRPreloadConfig(pTim, ENABLE); 			   //使能TIMx在ARR上的预装载寄存器
}

void arch_PwmStart(int nPort, int nPin)
{

	TIM_Cmd(stm32_tblTimBase[arch_PwmTimId(nPort, nPin)], ENABLE); //使能
}

void arch_PwmStop(int nPort, int nPin)
{

	TIM_Cmd(stm32_tblTimBase[arch_PwmTimId(nPort, nPin)], DISABLE); //
}


void arch_TimerInit(int nId)
{
	NVIC_InitTypeDef xNVIC;

	if (nId < 5)
	{
		xNVIC.NVIC_IRQChannel = stm32_tblTimIRQn[nId];
		xNVIC.NVIC_IRQChannelPreemptionPriority = 0;
		xNVIC.NVIC_IRQChannelSubPriority = 0;
		xNVIC.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&xNVIC);
	}
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


