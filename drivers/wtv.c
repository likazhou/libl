#if VOICE_ENABLE
#include <drivers/wtv.h>

#define WTV_AUDIO_MAX			20000

#define wtv_Reset(x)			sys_GpioSet(&tbl_bspVoice.tbl[0], x)
#define wtv_Nss(x)				sys_GpioSet(&tbl_bspVoice.tbl[1], x)
#define wtv_Busy()				sys_GpioGet(&tbl_bspVoice.tbl[2])





//-------------------------------------------------------------------------
//External Functions
//-------------------------------------------------------------------------
void wtv_Init()
{
	int i;

	for (i = 0; i < tbl_bspVoice.qty; i++)
	{
		sys_GpioConf(&tbl_bspVoice.tbl[i]);
	}
}

sys_res wtv_Cmd(spi_t *p, int nCmd, int nIsBusy)
{
	size_t nTmo;
	sys_res res;

	wtv_Reset(0);
	os_thd_slp1tick();
	wtv_Reset(1);
	os_thd_slp1tick();
	wtv_Nss(0);
	os_thd_slp1tick();
	nCmd = invert_bits(nCmd, 8);
	res = spi_Send(p, &nCmd, 1);
	wtv_Nss(1);
	
	if (res != SYS_R_OK)
		return res;
	
	if (nIsBusy == 0)
		return SYS_R_OK;
	
	for (nTmo = 100 / OS_TICK_MS; nTmo; nTmo--)
	{
		if (wtv_Busy() == 0)
			break;
		os_thd_slp1tick();
	}
	
	if (nTmo)
	{
		for (nTmo = WTV_AUDIO_MAX / OS_TICK_MS; nTmo; nTmo--)
		{
			if (wtv_Busy())
				return SYS_R_OK;
			os_thd_slp1tick();
		}
	}
	
	return SYS_R_TMO;
}


#endif

