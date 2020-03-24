#if WDG_ENABLE

void wdg_Reload(int nIntEnable)
{

#if WDT_INT_ENABLE
	arch_WdgReload(nIntEnable);
#endif
#if WDG_EXT_ENABLE
	sys_GpioSet(tbl_WdgCtrl[0], 0);
	sys_GpioSet(tbl_WdgCtrl[0], 1);
#endif
}

void wdg_Init()
{

#if WDT_INT_ENABLE
	arch_WdgInit();
#endif
#if WDG_EXT_ENABLE
	sys_GpioConf(tbl_WdgCtrl[0]);
#endif
	wdg_Reload(1);
}


#endif

