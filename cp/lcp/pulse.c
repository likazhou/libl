

#if PULSE_COL_ENABLE
void pulse_Init()
{
	t_gpio_def *p;

	for (p = tbl_bspPulse[0]; p < tbl_bspPulse[1]; p++)
	{
		sys_GpioConf(p);
	}
}

int pulse_Read()
{
	int i, n = 0;
	t_gpio_def *p;
	static int nSaved = ~0;

	for (i = 0, p = tbl_bspPulse[0]; p < tbl_bspPulse[1]; i++, p++)
	{
		if (sys_GpioGet(p))
		{
			if ((nSaved & BITMASK(i)) == 0)
			{
				SETBIT(nSaved, i);
				SETBIT(n, i);
			}
		}
		else
		{
			CLRBIT(nSaved, i);
		}
	}
	
	return n;
}
#endif

