



sys_res dev_Open(struct dev *p, size_t nTmo)
{
	int nForever;

	if (nTmo == OS_TMO_FOREVER)
	{
		nForever = 1;
	}
	else
	{
		nForever = 0;
		nTmo /= OS_TICK_MS;
	}
	
	do
	{
		if (p->ste == DEV_S_IDLE)
		{
			p->ste = DEV_S_READY;
			return SYS_R_OK;
		}

		os_thd_slp1tick();
		
	} while (nForever || nTmo--);

	return SYS_R_TMO;
}

sys_res dev_Close(struct dev *p)
{

	p->ste = DEV_S_IDLE;

	return SYS_R_OK;
}

int dev_IsOpened(struct dev *p)
{

	return (p->ste != DEV_S_IDLE) ? 1 : 0;
}

