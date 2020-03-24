#if HC165_ENABLE

//Private Defines
#define hc165_Le(x)				sys_GpioSet(gpio_node(tbl_bspHC165, 0), x)
#define hc165_Clk(x)	        sys_GpioSet(gpio_node(tbl_bspHC165, 1), x)
#define hc165_Din()				sys_GpioGet(gpio_node(tbl_bspHC165, 2))


//Private Variables
static u8 hc165_nCache;



//External Functions
void hc165_Maintian()
{
	int i, nData = 0;

	hc165_Le(1);	//锁存串行输入

	for (i = 0; i < 8; i++)
	{
		nData <<= 1;
		if (hc165_Din())
			nData |= 1;
		hc165_Clk(0);
		hc165_Clk(1);
	}

	hc165_Le(0);    //串行输入刷新
	hc165_nCache = nData;
}

u8 hc165_Read()
{

	return hc165_nCache;
}

#endif

