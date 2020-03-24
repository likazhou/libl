#if WDT_INT_ENABLE










//External Functions
void arch_WdgReload(uint nIntEnable)
{

	MAP_WatchdogIntClear(WATCHDOG0_BASE);
}

void arch_WdgInit()
{

	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_WDOG0);
	MAP_WatchdogReloadSet(WATCHDOG0_BASE, 40000000);
	MAP_WatchdogResetEnable(WATCHDOG0_BASE);
	MAP_WatchdogEnable(WATCHDOG0_BASE);
	MAP_WatchdogLock(WATCHDOG0_BASE);
}

#endif


