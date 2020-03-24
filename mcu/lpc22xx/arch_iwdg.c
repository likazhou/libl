#if WDT_INT_ENABLE










//External Functions
void arch_WdgReload(uint nIntEnable)
{

#if OS_TYPE
	if (nIntEnable)
		os_interrupt_Disable();
#endif
	LPC_WDT->WDFEED = 0xAA;
	LPC_WDT->WDFEED = 0x55;
#if OS_TYPE
	if (nIntEnable)
		os_interrupt_Enable();
#endif
}

void arch_WdgInit()
{

	LPC_WDT->WDTC = MCU_CLOCK / 16;
	LPC_WDT->WDMOD = 3;
	arch_WdgReload(1);
}

#endif


