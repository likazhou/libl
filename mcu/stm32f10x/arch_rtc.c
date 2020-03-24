#if RTC_TYPE == RTC_T_INT

//Private Defines
#define RTC_FLAG				0xA5A5


void arch_RtcInit()
{
    
	//需先调用stm32_BkpInit();
	if (BKP_ReadBackupRegister(BKP_DR9) != RTC_FLAG)
	{
		/* Backup data register value is not correct or not yet programmed (when
		the first time the program is executed) */

		/* Reset Backup Domain */
		BKP_DeInit();

		/* RTC Configuration */
		/* Enable LSE */
		RCC_LSEConfig(RCC_LSE_ON);
		
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET); /* Wait till LSE is ready */
		
		/* Select LSE as RTC Clock Source */
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		
		/* Enable RTC Clock */
		RCC_RTCCLKCmd(ENABLE);
		
		/* Wait for RTC registers synchronization */
		RTC_WaitForSynchro();
		
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
		
		/* Disable the RTC Second Interrupt */
		RTC_ITConfig(RTC_IT_SEC, DISABLE);
		
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
		
		/* Set RTC prescaler: set RTC period to 1sec */
		RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
		
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();

		BKP_WriteBackupRegister(BKP_DR9, RTC_FLAG);
	}
	else
	{
		/* Check if the Power On Reset flag is set */
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
		{
		}
		/* Check if the Pin Reset flag is set */
		else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
		{
		}

		/* Wait for RTC registers synchronization */
		RTC_WaitForSynchro();

		/* Disable the RTC Second Interrupt */
		RTC_ITConfig(RTC_IT_SEC, DISABLE);

		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
	}
}


#endif
