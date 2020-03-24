#if HC595_ENABLE


//Private Defines
#define hc595_Clk(x)			sys_GpioSet(gpio_node(tbl_bspHC595, 0), x)
#define hc595_RCK(x)			sys_GpioSet(gpio_node(tbl_bspHC595, 1), x)
#define hc595_SER(x)			sys_GpioSet(gpio_node(tbl_bspHC595, 2), x)
#if HC595_OE_ENABLE
#define hc595_OE(x)				sys_GpioSet(gpio_node(tbl_bspHC595, 3), x)
#endif


//Private Variables
static u8 hc595_nCache = 0;



//External Functions
void hc595_Maintian()
{
	int i, nTemp = hc595_nCache;
	
	hc595_RCK(0);
#if HC595_OE_ENABLE
	hc595_OE(0);
#endif
	
	for (i = 0; i < 8; i++)
	{
		hc595_Clk(0);

		if (nTemp & 0x80)
			hc595_SER(0);
		else
			hc595_SER(1);

		hc595_Clk(1);
		nTemp <<= 1;
	}

	hc595_RCK(1);
#if HC595_OE_ENABLE
	hc595_OE(0);
#endif
}




void hc595_Write(int nPin, int nHL)
{

	if (nHL)
		SETBIT(hc595_nCache, nPin);
	else
		CLRBIT(hc595_nCache, nPin);
}



#endif


