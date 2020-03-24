#include <libl.h>

//Private Variables
static int bat_nPowerOff = 0;
#if BAT_VOL_ENABLE
static float bat_fV;
#endif


//External Functions
void bat_On()
{

#if BAT_CTRL_HIGH_EN
	sys_GpioSet(gpio_node(tbl_bspBattery, 0), 1);
#else
	sys_GpioSet(gpio_node(tbl_bspBattery, 0), 0);
#endif
}

void bat_Off()
{

#if BAT_CTRL_HIGH_EN
	sys_GpioSet(gpio_node(tbl_bspBattery, 0), 0);
#else
	sys_GpioSet(gpio_node(tbl_bspBattery, 0), 1);
#endif
}

int bat_IsPowerOn()
{

	return sys_GpioGet(gpio_node(tbl_bspBattery, 1));
}

#if BAT_CHECK_ENABLE
int bat_IsBatteryOff()
{

	return sys_GpioGet(gpio_node(tbl_bspBattery, 2));
}
#endif

#if BAT_VOL_ENABLE
float bat_Voltage()
{

	return bat_fV;
}

void bat_VolGet()
{
#if BAT_CHECK_ENABLE
	tbl_gpio_def p = gpio_node(tbl_bspBattery, 3);
#else
	tbl_gpio_def p = gpio_node(tbl_bspBattery, 2);
#endif
	
	bat_fV = arch_AdcData(p->port, p->pin) * (3.3f / 4096.0f * 1.42f);
}
#endif

int bat_GetPowerOffCnt()
{

	return bat_nPowerOff;
}

void bat_Maintian()
{

	if (bat_IsPowerOn())
		bat_nPowerOff = 0;
	else
		bat_nPowerOff += 1;
}

