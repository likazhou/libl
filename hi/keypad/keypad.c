#include <hi/keypad.h>

void key_Init()
{
	t_gpio_def *p;
	
	for (p = tbl_bspKeypad[0]; p < tbl_bspKeypad[1]; p++)
	{
		sys_GpioConf(p);
	}
}

int key_Read()
{
	t_gpio_def *p;
	int i = 0, n = 0;
	static int nSaved = 0;
	
	for (p = tbl_bspKeypad[0]; p < tbl_bspKeypad[1]; p++, i++)
	{
		if (sys_GpioGet(p))
		{
			//有按键被按下
			if (nSaved & BITMASK(i))
			{
				CLRBIT(nSaved, i);
				SETBIT(n, i);
			}
		}
		else
		{
			SETBIT(nSaved, i);
		}
	}
	
	return n;
}


