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

	xGpio.GPIO_Speed = GPIO_Speed_50MHz;
	xGpio.GPIO_Mode = GPIO_Mode_AF_PP;

	switch (adrBase)
	{
	case FSMC_BASEADR_NORSRAM1:
		xGpio.GPIO_Pin = GPIO_Pin_7;
		GPIO_Init(GPIOD, &xGpio);
		return FSMC_Bank1_NORSRAM1;

	case FSMC_BASEADR_NORSRAM2:
		xGpio.GPIO_Pin = GPIO_Pin_9;
		GPIO_Init(GPIOG, &xGpio);
		return FSMC_Bank1_NORSRAM2;

	case FSMC_BASEADR_NORSRAM3:
		xGpio.GPIO_Pin = GPIO_Pin_10;
		GPIO_Init(GPIOG, &xGpio);
		return FSMC_Bank1_NORSRAM3;

	case FSMC_BASEADR_NORSRAM4:
		xGpio.GPIO_Pin = GPIO_Pin_12;
		GPIO_Init(GPIOG, &xGpio);
		return FSMC_Bank1_NORSRAM4;

	case FSMC_BASEADR_NAND2:
		xGpio.GPIO_Pin = GPIO_Pin_7;
		GPIO_Init(GPIOD, &xGpio);
		return FSMC_Bank2_NAND;

	case FSMC_BASEADR_NAND3:
		xGpio.GPIO_Pin = GPIO_Pin_9;
		GPIO_Init(GPIOG, &xGpio);
		return FSMC_Bank3_NAND;

	default:
		return 0;
	}
}


/*******************************************************************************
* Function Name  : FSMC_NOR_Init
* Description    : Configures the FSMC and GPIOs to interface with the NOR memory.
*                  This function must be called before any write/read operation
*                  on the NOR.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
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

	/*-- GPIO Configuration ------------------------------------------------------*/
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | 
							RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG, ENABLE);

	xGpio.GPIO_Speed =	GPIO_Speed_50MHz;
	xGpio.GPIO_Mode =	GPIO_Mode_AF_PP;

	/* NOE and NWE configuration */
	xGpio.GPIO_Pin =	GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOD, &xGpio);
	
	/* D0-D15 lines configuration */
	xGpio.GPIO_Pin = 	GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 |
						GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOD, &xGpio);

	xGpio.GPIO_Pin =	GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
						GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 |
						GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOE, &xGpio);

	/* Address lines configuration */
	xGpio.GPIO_Pin =	GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
						GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_13 |
						GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOF, &xGpio);

	xGpio.GPIO_Pin =	GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | 
						GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_Init(GPIOG, &xGpio);

	xGpio.GPIO_Pin =	GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_Init(GPIOD, &xGpio);

	xGpio.GPIO_Pin =	GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_Init(GPIOE, &xGpio);

#if EXTSRAM_ENABLE
	xGpio.GPIO_Pin =	GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_Init(GPIOE, &xGpio);
#endif

#if NANDFLASH_ENABLE
	xGpio.GPIO_Mode = GPIO_Mode_IPU;
	/* NWAIT NAND pin configuration */
	xGpio.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOD, &xGpio); 
#endif

	/* FSMC Clock Enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

#if NORFLASH_ENABLE
	nBank = stm32_Bank(NORFLASH_BASE_ADR);

	/*-- FSMC Configuration ----------------------------------------------------*/
	xNorReadTiming.FSMC_AddressSetupTime = 1;
	xNorReadTiming.FSMC_AddressHoldTime = 0;
	xNorReadTiming.FSMC_DataSetupTime = 5;
	xNorReadTiming.FSMC_BusTurnAroundDuration = 0;
	xNorReadTiming.FSMC_CLKDivision = 0;
	xNorReadTiming.FSMC_DataLatency = 0;
 	xNorReadTiming.FSMC_AccessMode = FSMC_AccessMode_B;
	xNorWriteTiming.FSMC_AddressSetupTime = 1;
	xNorWriteTiming.FSMC_AddressHoldTime = 0;
	xNorWriteTiming.FSMC_DataSetupTime = 5;
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

