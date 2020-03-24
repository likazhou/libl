#if EPI_ENABLE


//Private Defines
#define FSMC_BASEADR_NORSRAM1			0x60000000
#define FSMC_BASEADR_NORSRAM2			0x64000000
#define FSMC_BASEADR_NORSRAM3			0x68000000
#define FSMC_BASEADR_NORSRAM4			0x6C000000

#define FSMC_BASEADR_NAND2				0x70000000
#define FSMC_BASEADR_NAND3				0x80000000



//Internal Functions
static u32 stm32_Bank(adr_t adrBase)
{
	GPIO_InitTypeDef xGpio;

	xGpio.GPIO_Mode = GPIO_Mode_AF;
	xGpio.GPIO_Speed = GPIO_Speed_100MHz;
	xGpio.GPIO_OType = GPIO_OType_PP;
	xGpio.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	switch (adrBase)
	{
	case FSMC_BASEADR_NORSRAM1:
		xGpio.GPIO_Pin = GPIO_Pin_7;
		GPIO_Init(GPIOD, &xGpio);
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC);
		return FSMC_Bank1_NORSRAM1;
		
	case FSMC_BASEADR_NORSRAM2:
		xGpio.GPIO_Pin = GPIO_Pin_9;
		GPIO_Init(GPIOG, &xGpio);
		GPIO_PinAFConfig(GPIOG, GPIO_PinSource9, GPIO_AF_FSMC);
		return FSMC_Bank1_NORSRAM2;
		
	case FSMC_BASEADR_NORSRAM3:
		xGpio.GPIO_Pin = GPIO_Pin_10;
		GPIO_Init(GPIOG, &xGpio);
		GPIO_PinAFConfig(GPIOG, GPIO_PinSource10, GPIO_AF_FSMC);
		return FSMC_Bank1_NORSRAM3;
		
	case FSMC_BASEADR_NORSRAM4:
		xGpio.GPIO_Pin = GPIO_Pin_12;
		GPIO_Init(GPIOG, &xGpio);
		GPIO_PinAFConfig(GPIOG, GPIO_PinSource12, GPIO_AF_FSMC);
		return FSMC_Bank1_NORSRAM4;
		
	case FSMC_BASEADR_NAND2:
		xGpio.GPIO_Pin = GPIO_Pin_7;
		GPIO_Init(GPIOD, &xGpio);
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC);
		return FSMC_Bank2_NAND;
		
	case FSMC_BASEADR_NAND3:
		xGpio.GPIO_Pin = GPIO_Pin_9;
		GPIO_Init(GPIOG, &xGpio);
		GPIO_PinAFConfig(GPIOG, GPIO_PinSource9, GPIO_AF_FSMC);
		return FSMC_Bank3_NAND;
		
	default:
		return 0;
	}
}

void stm32_FsmcInit()
{
	FSMC_NORSRAMInitTypeDef xNor;
	FSMC_NORSRAMTimingInitTypeDef xNorWriteTiming, xNorReadTiming;
#if NANDFLASH_ENABLE
	FSMC_NANDInitTypeDef xNand;
	FSMC_NAND_PCCARDTimingInitTypeDef xNandWriteTiming, xNandReadTiming;
#endif
	GPIO_InitTypeDef xGpio;
	u32 nBank;
	
	/* Enable GPIOs clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE |
							RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG, ENABLE);
	/*-- GPIO Configuration ------------------------------------------------------*/
	/*
	 +-------------------+--------------------+------------------+------------------+
	 +                       SRAM pins assignment                                  +
	  +-------------------+--------------------+------------------+------------------+
	 | PD0  <-> FSMC_D2  | PE0  <-> FSMC_NBL0 | PF0  <-> FSMC_A0 | PG0 <-> FSMC_A10 |
	 | PD1  <-> FSMC_D3  | PE1  <-> FSMC_NBL1 | PF1  <-> FSMC_A1 | PG1 <-> FSMC_A11 |
	 | PD4  <-> FSMC_NOE | PE2  <-> FSMC_A23  | PF2  <-> FSMC_A2 | PG2 <-> FSMC_A12 |
	 | PD5  <-> FSMC_NWE | PE3  <-> FSMC_A19  | PF3  <-> FSMC_A3 | PG3 <-> FSMC_A13 |
	 | PD8  <-> FSMC_D13 | PE4  <-> FSMC_A20  | PF4  <-> FSMC_A4 | PG4 <-> FSMC_A14 |
	 | PD9  <-> FSMC_D14 | PE5  <-> FSMC_A21  | PF5  <-> FSMC_A5 | PG5 <-> FSMC_A15 | 
	 | PD10 <-> FSMC_D15 | PE6  <-> FSMC_A22  | PF12 <-> FSMC_A6 | PG9 <-> FSMC_NE2 | 
	 | PD11 <-> FSMC_A16 | PE7  <-> FSMC_D4   | PF13 <-> FSMC_A7 |------------------+
	 | PD12 <-> FSMC_A17 | PE8  <-> FSMC_D5	  | PF14 <-> FSMC_A8 |------------------+
     | PD13 <-> FSMC_A18 | PE9  <-> FSMC_D6   | PF15 <-> FSMC_A9 |------------------+ 
	 | PD14 <-> FSMC_D0  | PE10 <-> FSMC_D7   	------------------+
	 | PD15 <-> FSMC_D1  | PE11 <-> FSMC_D8   	------------------+   
	 +------------------ | PE12 <-> FSMC_D9   	------------------+   
	 +------------------ | PE13 <-> FSMC_D10    ------------------+   
 	 +------------------ | PE14 <-> FSMC_D11    ------------------+   
	 +------------------ | PE15 <-> FSMC_D12  	------------------+
	 +-------------------+--------------------+
	*/
	 
	/*-- GPIO ≈‰÷√ -----------------------------------------------------------*/
	xGpio.GPIO_Mode = GPIO_Mode_AF;
	xGpio.GPIO_Speed = GPIO_Speed_100MHz;
	xGpio.GPIO_OType = GPIO_OType_PP;
	xGpio.GPIO_PuPd  = GPIO_PuPd_NOPULL;

	xGpio.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_4  | GPIO_Pin_5 |  
					GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 | GPIO_Pin_11 |
					GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
				 
	/* GPIOD configuration */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);  //FSMC_NOE
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);  //FSMC_NWE
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC); 
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);
	GPIO_Init(GPIOD, &xGpio);

	/* GPIOE configuration */
	xGpio.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 |
					GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9  | GPIO_Pin_10 | GPIO_Pin_11 |
					GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
								
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource2 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource3 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource4 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource5 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource6 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC);

	GPIO_Init(GPIOE, &xGpio);

	/* GPIOF configuration */
	xGpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 |
					GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
								
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource0 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource1 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource2 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource3 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource4 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource5 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource12 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource13 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource14 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource15 , GPIO_AF_FSMC);

	GPIO_Init(GPIOF, &xGpio);

	/* GPIOG configuration */
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource0 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource1 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource2 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource3 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource4 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource5 , GPIO_AF_FSMC);

	xGpio.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | 
						GPIO_Pin_4 | GPIO_Pin_5;

	GPIO_Init(GPIOG, &xGpio);

	/* FSMC_NWAIT */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_FSMC);
	xGpio.GPIO_Pin = GPIO_Pin_6; 
	xGpio.GPIO_Mode = GPIO_Mode_IN;
	xGpio.GPIO_Speed = GPIO_Speed_100MHz;
	xGpio.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &xGpio);

#if EXTSRAM_ENABLE
	
	
#endif

#if NANDFLASH_ENABLE
	
	
#endif

	/* FSMC Clock Enable */
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

#if NORFLASH_ENABLE
	nBank = stm32_Bank(NORFLASH_BASE_ADR);

  	/*-- FSMC Configuration ----------------------------------------------------*/
	xNorReadTiming.FSMC_AddressSetupTime = 5;
	xNorReadTiming.FSMC_AddressHoldTime = 0;
	xNorReadTiming.FSMC_DataSetupTime = 12;
	xNorReadTiming.FSMC_BusTurnAroundDuration = 0;
	xNorReadTiming.FSMC_CLKDivision = 0;
	xNorReadTiming.FSMC_DataLatency = 0;
 	xNorReadTiming.FSMC_AccessMode = FSMC_AccessMode_B;
	xNorWriteTiming.FSMC_AddressSetupTime = 5;
	xNorWriteTiming.FSMC_AddressHoldTime = 0;
	xNorWriteTiming.FSMC_DataSetupTime = 12;
	xNorWriteTiming.FSMC_BusTurnAroundDuration = 0;
	xNorWriteTiming.FSMC_CLKDivision = 0;
	xNorWriteTiming.FSMC_DataLatency = 0;
 	xNorWriteTiming.FSMC_AccessMode = FSMC_AccessMode_B;
	
	xNor.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	xNor.FSMC_MemoryType = FSMC_MemoryType_NOR;
	xNor.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	xNor.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	xNor.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	xNor.FSMC_WrapMode = FSMC_WrapMode_Disable;
	xNor.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	xNor.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	xNor.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	xNor.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	xNor.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	xNor.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	xNor.FSMC_ReadWriteTimingStruct = &xNorReadTiming;
	xNor.FSMC_WriteTimingStruct = &xNorWriteTiming;

	xNor.FSMC_Bank = nBank;
	FSMC_NORSRAMInit(&xNor);
	FSMC_NORSRAMCmd(nBank, ENABLE);
#endif

#if EXTSRAM_ENABLE
	nBank = stm32_Bank(EXTSRAM_BASE_ADR);

  	/*-- FSMC Configuration ----------------------------------------------------*/
	xNorReadTiming.FSMC_AddressSetupTime = 0;
	xNorReadTiming.FSMC_AddressHoldTime = 0;
	xNorReadTiming.FSMC_DataSetupTime = 2;
	xNorReadTiming.FSMC_BusTurnAroundDuration = 0;
	xNorReadTiming.FSMC_CLKDivision = 0;
	xNorReadTiming.FSMC_DataLatency = 0;
 	xNorReadTiming.FSMC_AccessMode = FSMC_AccessMode_A;
	xNorWriteTiming.FSMC_AddressSetupTime = 0;
	xNorWriteTiming.FSMC_AddressHoldTime = 0;
	xNorWriteTiming.FSMC_DataSetupTime = 2;
	xNorWriteTiming.FSMC_BusTurnAroundDuration = 0;
	xNorWriteTiming.FSMC_CLKDivision = 0;
	xNorWriteTiming.FSMC_DataLatency = 0;
 	xNorWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;

	xNor.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	xNor.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	xNor.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	xNor.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	xNor.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	xNor.FSMC_WrapMode = FSMC_WrapMode_Disable;
	xNor.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	xNor.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	xNor.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	xNor.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	xNor.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	xNor.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	xNor.FSMC_ReadWriteTimingStruct = &xNorReadTiming;
	xNor.FSMC_WriteTimingStruct = &xNorWriteTiming;

	xNor.FSMC_Bank = nBank;
	FSMC_NORSRAMInit(&xNor);
	FSMC_NORSRAMCmd(nBank, ENABLE);
#endif

#if GUI_ENABLE
	nBank = stm32_Bank(GUI_LCD_BASE_ADR);

  	/*-- FSMC Configuration ----------------------------------------------------*/
	xNorReadTiming.FSMC_AddressSetupTime = 1;
	xNorReadTiming.FSMC_AddressHoldTime = 0;
	xNorReadTiming.FSMC_DataSetupTime = 9;
	xNorReadTiming.FSMC_BusTurnAroundDuration = 0;
	xNorReadTiming.FSMC_CLKDivision = 0;
	xNorReadTiming.FSMC_DataLatency = 0;
 	xNorReadTiming.FSMC_AccessMode = FSMC_AccessMode_A;
	xNorWriteTiming.FSMC_AddressSetupTime = 1;
	xNorWriteTiming.FSMC_AddressHoldTime = 0;
	xNorWriteTiming.FSMC_DataSetupTime = 9;
	xNorWriteTiming.FSMC_BusTurnAroundDuration = 0;
	xNorWriteTiming.FSMC_CLKDivision = 0;
	xNorWriteTiming.FSMC_DataLatency = 0;
 	xNorWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;

	xNor.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	xNor.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	xNor.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;
	xNor.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	xNor.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	xNor.FSMC_WrapMode = FSMC_WrapMode_Disable;
	xNor.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	xNor.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	xNor.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	xNor.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;
	xNor.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	xNor.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	xNor.FSMC_ReadWriteTimingStruct = &xNorReadTiming;
	xNor.FSMC_WriteTimingStruct = &xNorWriteTiming;

	xNor.FSMC_Bank = nBank;
	FSMC_NORSRAMInit(&xNor);
	FSMC_NORSRAMCmd(nBank, ENABLE);
#endif

#if DM9000_ENABLE
	nBank = stm32_Bank(DM9000_BASE_ADR);

	/*-- FSMC Configuration ----------------------------------------------------*/
	xNorReadTiming.FSMC_AddressSetupTime = 0;
	xNorReadTiming.FSMC_AddressHoldTime = 0;
	xNorReadTiming.FSMC_DataSetupTime = 2;
	xNorReadTiming.FSMC_BusTurnAroundDuration = 0;
	xNorReadTiming.FSMC_CLKDivision = 0;
	xNorReadTiming.FSMC_DataLatency = 0;
	xNorReadTiming.FSMC_AccessMode = FSMC_AccessMode_A;
	xNorWriteTiming.FSMC_AddressSetupTime = 0;
	xNorWriteTiming.FSMC_AddressHoldTime = 0;
	xNorWriteTiming.FSMC_DataSetupTime = 2;
	xNorWriteTiming.FSMC_BusTurnAroundDuration = 0;
	xNorWriteTiming.FSMC_CLKDivision = 0;
	xNorWriteTiming.FSMC_DataLatency = 0;
	xNorWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;

	xNor.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	xNor.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	xNor.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	xNor.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	xNor.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	xNor.FSMC_WrapMode = FSMC_WrapMode_Disable;
	xNor.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	xNor.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	xNor.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	xNor.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	xNor.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	xNor.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	xNor.FSMC_ReadWriteTimingStruct = &xNorReadTiming;
	xNor.FSMC_WriteTimingStruct = &xNorWriteTiming;

	xNor.FSMC_Bank = nBank;
	FSMC_NORSRAMInit(&xNor);
	FSMC_NORSRAMCmd(nBank, ENABLE);
#endif

#if NANDFLASH_ENABLE
	nBank = stm32_Bank(NAND_BASE_ADR);

	/*-- FSMC Configuration ------------------------------------------------------*/
	xNandReadTiming.FSMC_SetupTime = 1;
	xNandReadTiming.FSMC_WaitSetupTime = 3;
	xNandReadTiming.FSMC_HoldSetupTime = 2;
	xNandReadTiming.FSMC_HiZSetupTime = 1;
	xNandWriteTiming.FSMC_SetupTime = 1;
	xNandWriteTiming.FSMC_WaitSetupTime = 3;
	xNandWriteTiming.FSMC_HoldSetupTime = 2;
	xNandWriteTiming.FSMC_HiZSetupTime = 1;

	xNand.FSMC_Waitfeature = FSMC_Waitfeature_Enable;
#if NAND_DATA_WIDTH == 16
	xNand.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
#else
	xNand.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;
#endif
	xNand.FSMC_ECC = FSMC_ECC_Disable;
#if NAND_PAGE_DATA == 2048
	xNand.FSMC_ECCPageSize = FSMC_ECCPageSize_2048Bytes;
#else
	xNand.FSMC_ECCPageSize = FSMC_ECCPageSize_512Bytes;
#endif
	xNand.FSMC_AddressLowMapping = FSMC_AddressLowMapping_Direct;
	xNand.FSMC_TCLRSetupTime = 0x00;
	xNand.FSMC_TARSetupTime = 0x00;
	xNand.FSMC_CommonSpaceTimingStruct = &xNandReadTiming;
	xNand.FSMC_AttributeSpaceTimingStruct = &xNandWriteTiming;

	xNand.FSMC_Bank = nBank;
	FSMC_NANDInit(&xNand);
	FSMC_NANDCmd(nBank, ENABLE);
#endif
}


#endif

