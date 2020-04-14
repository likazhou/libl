

void arch_WdgReload(int nIntEnable)
{

	/* Reload IWDG counter */
	LL_IWDG_ReloadCounter(IWDG);
}




/*******************************************************************************
* Function Name  : stm32_WdgInit
* Description    : 片内独立看门狗配置
* Input 		 : None
* Output		 : None
* Return		 : None
*******************************************************************************/
void arch_WdgInit()
{

	LL_IWDG_EnableWriteAccess(IWDG);
	/* IWDG counter clock: 30-60KHz(LSI) / 64 = 469-937.5Hz */
	LL_IWDG_SetPrescaler(IWDG, LL_IWDG_PRESCALER_64);
	/* Set counter reload value to 1800 */
	LL_IWDG_SetReloadCounter(IWDG, 1800);
	/* Enable IWDG (the LSI oscillator will be enabled by hardware) */
	LL_IWDG_Enable(IWDG);
}

