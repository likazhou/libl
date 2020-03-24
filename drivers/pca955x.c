#if PCA955X_ENABLE
#include <drivers/pca955x.h>


//Private Defines
#define PCA955X_SLAVEADR		0x70


//Private Macros
#define pca955x_Irq()			sys_GpioGet(&tbl_bspPca955x.tbl[0])


sys_res pca955x_Init(i2c_t *p)
{
	sys_res res;

	sys_GpioConf(&tbl_bspPca955x.tbl[0]);

	res = i2c_WriteByte(p, PCA955X_SLAVEADR, 1, 0xFF); //所有输出置为高电平
	res = i2c_WriteByte(p, PCA955X_SLAVEADR, 2, 0x00); //所有输入均不反转
	res = i2c_WriteByte(p, PCA955X_SLAVEADR, 3, PCA955X_CFG_IN); //0/1/2/3/4输入,5/6/7输出	
	return res;
}

int pca955x_GpioRead(i2c_t *p, int n)
{
	int nData = 0;

	i2c_WriteAdr(p, PCA955X_SLAVEADR, 0);
	if (i2c_Read(p, PCA955X_SLAVEADR, &nData, 1) != SYS_R_OK)
		pca955x_Init(p);
	
	return GETBIT(nData, n);
}

void pca955x_GpioSet(i2c_t *p, int nPin, int nHL)
{
	sys_res res;
	int nData = 0;

	i2c_WriteAdr(p, PCA955X_SLAVEADR, 1);
	if ((res = i2c_Read(p, PCA955X_SLAVEADR, &nData, 1)) == SYS_R_OK)
	{
		if (nHL)
			SETBIT(nData, nPin);
		else
			CLRBIT(nData, nPin);
		res = i2c_WriteByte(p, PCA955X_SLAVEADR, 1, nData);
	}
	
	if (res != SYS_R_OK)
		pca955x_Init(p);
}

#endif

