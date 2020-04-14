
//Private Defines


//Private Variables
static void (*sys_hook_init)(void) = NULL;

//In application
extern void app_Entry(void);
extern void app_Daemon(size_t nCnt);


#if OS_TYPE
void sys_IOHandle(void *args)
{
	static size_t nCnt;

#if IRQ_HALF_ENABLE
	//底半中断处理
	if (irq_Wait() == SYS_R_OK)
		irq_BottomHandler();
#endif

	if (nCnt++ > (500 / OS_TICK_MS))
	{
		nCnt = 0;

#if UART_ENABLE
		//串口维护
		uart_Maintian();
#endif

#if HC165_ENABLE
		hc165_Maintian();
#endif

#if HC595_ENABLE
		hc595_Maintian();
#endif
	}

#if USB_ENABLE
	usb_HostHandler();
#endif

#if TCPPS_ENABLE
	net_Handler();
#endif

#if ATSVR_ENABLE
	atsvr_Run();
#endif
}


#if OS_TYPE != OS_T_CHNIL
os_thd_declare(SysIo, 2048);
#endif
void tsk_SysIo(void *args)
{

	for (; ; )
	{
		sys_IOHandle(NULL);
		os_thd_slp1tick();
	}
}

#if DQUEUE_ENABLE
os_thd_declare(DqueIo, 2048);
void tsk_DqueIo(void *args)
{

	for (; ;)
	{
		dque_Wait();
	}
}
#endif

void sys_Maintian()
{
	size_t nCnt;

	for (nCnt = 0; ; nCnt++)
	{
		os_thd_sleep(1000);
		
#if MODEM_ENABLE
		modem_Run();
#endif
#if RDSS_ENABLE
		rdss_Run();
#endif
#if RNSS_ENABLE
		rnss_Run();
#endif
#if RDRN_ENABLE
		rdrn_Run();
#endif

#if BAT_VOL_ENABLE
		bat_VolGet();
#endif

#if DEBUG_MEMORY_ENABLE
		if ((nCnt & 0x03) == 0)
			list_memdebug(0, 0);
#endif

		if ((nCnt & 0x1F) == 0)
		{
#if RTC_ENABLE
			rtc_Maintian();
#endif

#if FLASH_ENABLE
			flash_Flush(60);
#endif
		}
		
#if UART_ENABLE
		if ((nCnt & 0x0FFF) == 0)
			uart_Reopen();
#endif

#if BATTERY_ENABLE
		if (nCnt == 2)
			bat_On();
#endif
	}
}


void sys_Daemon(void *args)
{
	static size_t nCnt;

#if WDG_ENABLE
	if ((nCnt & 0x03) == 0)
		wdg_Reload(1);
#endif
	
	if ((nCnt % 10) == 0)
	{
#if RTC_ENABLE
		rtc_OsTick();
#endif
#if BATTERY_ENABLE
		bat_Maintian();
#endif
	}

	app_Daemon(nCnt);

	nCnt += 1;
}
#if OS_TYPE == OS_T_RTTHREAD
static struct rt_timer timer_daemon;
#elif OS_TYPE != OS_T_CHNIL
os_thd_declare(Daemon, 512);
void tsk_Daemon(void *args)
{

	for (; ; )
	{
		sys_Daemon(NULL);
		os_thd_sleep(100);
	}
}
#endif
#endif




void sys_Init()
{
	int i;
#if GPIO_ENABLE
	t_gpio_def *p;
#endif
#if I2C_ENABLE
	i2c_t *pI2c;
#endif
#if SPI_ENABLE
	spi_t *pSpi;
#endif
#if UART_ENABLE
	uart_t *pUart;
#endif

	SYS_HOOK_CALL(sys_hook_init, ());

#if LIB_ZIP_ENABLE
	zip_Init();
#endif

#if IRQ_ENABLE
	irq_VectorInit();
#endif

#if GPIO_ENABLE
	for (p = tbl_bspGpio[0]; p < tbl_bspGpio[1]; p++)
	{
		sys_GpioConf(p);
	}
#endif

#if HC138_ENABLE
	for (p = tbl_bspHC138[0]; p < tbl_bspHC138[1]; p++)
	{
		sys_GpioConf(p);
	}
#endif

#if HC165_ENABLE
	for (p = tbl_bspHC165[0]; p < tbl_bspHC165[1]; p++)
	{
		sys_GpioConf(p);
	}
#endif

#if HC595_ENABLE
	for (p = tbl_bspHC595[0]; p < tbl_bspHC595[1]; p++)
	{
		sys_GpioConf(p);
	}
#endif

#if BATTERY_ENABLE
	for (p = tbl_bspBattery[0]; p < tbl_bspBattery[1]; p++)
	{
		sys_GpioConf(p);
	}
#if BAT_VOL_ENABLE
	arch_AdcInit();
#endif
#endif

#if WDG_ENABLE
	wdg_Init();
#endif

#if BKP_ENABLE
	bkp_Init();
#endif

#if KEY_ENABLE
	key_Init();
#endif

#if DQUEUE_ENABLE
	dque_SystemInit();
#endif

#if EPI_SOFTWARE
	for (p = tbl_bspEpiData[0]; p < tbl_bspEpiData[1]; p++)
	{
		sys_GpioConf(p);
	}
#endif

#if I2C_ENABLE
	memset(dev_I2c, 0, sizeof(dev_I2c));
	for (pI2c = dev_I2c, i = 0; pI2c < ARR_ENDADR(dev_I2c); pI2c++, i++)
	{
		pI2c->parent.id = i;
#if I2C_SOFTWARE
		i2cbus_Init(pI2c);
#else
		pI2c->def = &tbl_bspI2cDef[i];
		arch_I2cInit(pI2c);
#endif
	}
#endif

#if SPI_ENABLE
	memset(dev_Spi, 0, sizeof(dev_Spi));
	for (pSpi = dev_Spi, i = 0; pSpi < ARR_ENDADR(dev_Spi); pSpi++, i++)
	{
		pSpi->parent.id = i;
#if SPI_SOFTWARE
		spibus_Init(pSpi);
#else
		pSpi->def = &tbl_bspSpiDef[i];
		arch_SpiInit(pSpi);
#endif
	}
#endif

#if UART_ENABLE
	memset(dev_Uart, 0, sizeof(dev_Uart));
	for (pUart = dev_Uart, i = 0; pUart < ARR_ENDADR(dev_Uart); pUart++, i++)
	{
		pUart->parent.id = i;
		pUart->def = &tbl_bspUartDef[i];
		uart_Init(pUart);
	}
#endif	
	
#if OS_TYPE
	buf_Init();
#endif

#if IRQ_ENABLE
	irq_Init();
#endif

//-------------------------------------------------------------------------
//GPIO Functions
//-------------------------------------------------------------------------
#if PULSE_COL_ENABLE
	pulse_Init();
#endif
#if HT1621_ENABLE
	ht1621_Init();
#endif

//-------------------------------------------------------------------------
//External Parallel Interface Functions
//-------------------------------------------------------------------------
#if NANDFLASH_ENABLE
	nand_Init();
#endif

//-------------------------------------------------------------------------
//I2C Interface Functions
//-------------------------------------------------------------------------
#if PCA955X_ENABLE
	pca955x_Init();
#endif

//-------------------------------------------------------------------------
//SPI Interface Functions
//-------------------------------------------------------------------------
#if SC16IS7X_ENABLE
	sc16is7x_Init();
#endif
#if ATT7022_ENABLE
	att7022_Init();
#endif
#if RN8302_ENABLE
	rn8302_Init();
#endif
#if NRSEC3000_ENABLE
	nrsec3000_Init();
#endif
#if VOICE_ENABLE
	wtv_Init();
#endif
#if MIFARE_ENABLE
	mf_InitGpio();
#endif

//-------------------------------------------------------------------------
//UART Interface Functions
//-------------------------------------------------------------------------
#if VK321X_ENABLE
	vk321x_Init();
#endif
#if TDK6515_ENABLE
	tdk6515_Init();
#endif
#if MODEM_ENABLE
	modem_Init();
#endif
#if ATSVR_ENABLE
	atsvr_Init();
#endif
#if RDSS_ENABLE
	rdss_Init();
#endif
#if RNSS_ENABLE
	rnss_Init();
#endif
#if RDRN_ENABLE
	rdrn_Init();
#endif

//-------------------------------------------------------------------------
//Real Time Clock Functions
//-------------------------------------------------------------------------
#if RTC_ENABLE
	rtc_Init();
#endif

//-------------------------------------------------------------------------
//Backup Functions
//-------------------------------------------------------------------------
#if BKP_ENABLE
	bkp_Init();
#endif

//-------------------------------------------------------------------------
//Flash System Functions
//-------------------------------------------------------------------------
#if INTFLASH_ENABLE
	intf_Init();
#endif
#if NORFLASH_ENABLE
	norf_Init();
#endif
#if SPIFLASH_ENABLE
	spif_Init();
#endif
#if FLASH_ENABLE
	flash_Init();
#endif

//-------------------------------------------------------------------------
//File System Functions
//-------------------------------------------------------------------------
#if SFS_ENABLE
	sfs_SystemInit();
#endif
#if FS_ENABLE
	fs_init();
#endif

//-------------------------------------------------------------------------
//USB Interface Functions
//-------------------------------------------------------------------------
#if USB_ENABLE
	usb_Init();
#endif

//-------------------------------------------------------------------------
//Network Subsystem Functions
//-------------------------------------------------------------------------
#if TCPPS_ENABLE

#if DM9000_ENABLE
	dm9000_Init();
#endif

	net_Init();
#endif

//-------------------------------------------------------------------------
//In Application Programming Functions
//-------------------------------------------------------------------------
#if GDFTS_ENABLE
	gdfts_Init();
#endif

#if GUI_ENABLE
	gui_Init();
#endif

#if OS_TYPE

//创建系统IO处理线程
#if OS_TYPE != OS_T_CHNIL
	os_thd_init(SysIo, OS_THDPRI_HIGHEST);
#if DQUEUE_ENABLE
	os_thd_init(DqueIo, OS_THDPRI_HIGHEST);
#endif
#endif
	
	//创建系统守护定时(1S)
#if OS_TYPE == OS_T_RTTHREAD
	rt_timer_init(&timer_daemon, "daemon", sys_Daemon, NULL, 100 / OS_TICK_MS, RT_TIMER_FLAG_PERIODIC);
	rt_timer_start(&timer_daemon);
#elif OS_TYPE != OS_T_CHNIL
	os_thd_init(Daemon, OS_THDPRI_HIGHEST);
#endif

//-------------------------------------------------------------------------
//Debug Log Functions
//-------------------------------------------------------------------------
#if DEBUG_LOG_ENABLE
	log_Init();
#endif

	//创建应用层线程
	app_Entry();

	//系统维护线程
	sys_Maintian();

#endif
}


void sys_Start()
{
	
	arch_Init();

#if OS_TYPE
	os_Start();
#else
	sys_Init();
#endif
}

void sys_Reset()
{

#if FLASH_ENABLE
	flash_Flush(0);
#endif
	arch_Reset();
}

void sys_Delay(volatile size_t n)
{

	while (n--);
}

void sys_HookSet(void (*pHook)(void))
{

	sys_hook_init = pHook;
}



