#include <drivers/hc138.h>



#if 0
void hc138_Set(int nId)
{
	int i;

	for (i = 0; i < 4; i++)
	{
		if (nId & BITMASK(i))
			sys_GpioSet(gpio_node(tbl_bspHC138, i), 1);
		else
			sys_GpioSet(gpio_node(tbl_bspHC138, i), 0);
	}
}
#else
void hc138_Set(int nId)
{

	LPC_GPIO1->FIOCLR = 0x07 << 23;
	LPC_GPIO1->FIOSET = (nId & 0x07) << 23;
	
	if (nId & 0x08)
		LPC_GPIO0->FIOSET = BITMASK(21);
	else
		LPC_GPIO0->FIOCLR = BITMASK(21);
}
#endif

