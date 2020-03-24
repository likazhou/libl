#include <drivers/i2cbus.h>







//Private Defines
#define I2CBUS_LOCK_ENABLE		0


//Private Macros
#if I2CBUS_LOCK_ENABLE
#define i2cbus_Lock()			os_thd_lock()
#define i2cbus_Unlock()			os_thd_unlock()
#else
#define i2cbus_Lock()
#define i2cbus_Unlock()
#endif




//-------------------------------------------------------------------
//Internal Functions
//-------------------------------------------------------------------
static int i2cbus_Ste(i2c_t *p)
{
	t_gpio_def *pIO = tbl_bspI2cDef[p->parent.id] + 1;

	return arch_GpioRead(pIO->port, pIO->pin);
}

static void i2cbus_Scl(i2c_t *p, int nHL)
{
	t_gpio_def *pIO = tbl_bspI2cDef[p->parent.id];

	arch_GpioSet(pIO->port, pIO->pin, nHL);
	sys_Delay(I2CBUS_DELAY);
}

static void i2cbus_Sda(i2c_t *p, int nHL)
{
	t_gpio_def *pIO = tbl_bspI2cDef[p->parent.id] + 1;

	arch_GpioSet(pIO->port, pIO->pin, nHL);
}

static void i2cbus_IoInput(i2c_t *p)
{
	t_gpio_def *pIO = tbl_bspI2cDef[p->parent.id] + 1;

	arch_GpioConf(pIO->port, pIO->pin, GPIO_M_IN_FLOAT, GPIO_INIT_HIGH);
}

static void i2cbus_IoConf(i2c_t *p)
{
	t_gpio_def *pIO = tbl_bspI2cDef[p->parent.id] + 1;

	arch_GpioConf(pIO->port, pIO->pin, pIO->mode, pIO->init);
}

static sys_res i2cbus_Start(i2c_t *p)
{

	i2cbus_Sda(p, 1);
	i2cbus_Scl(p, 1);
	
	if (i2cbus_Ste(p) == 0)
		return SYS_R_ERR;
	
	i2cbus_Sda(p, 0);
	
	return SYS_R_OK;
}

static void i2cbus_Stop(i2c_t *p)
{

	i2cbus_Scl(p, 0);
	i2cbus_Sda(p, 0);
	i2cbus_Scl(p, 1);
	i2cbus_Sda(p, 1);
	i2cbus_Scl(p, 1);
}

static sys_res i2cbus_WaitAck(i2c_t *p)
{
	int nSda;

	i2cbus_Scl(p, 0);
#if I2CBUS_IOMODE_CHANGE
	i2cbus_IoInput(p);
#else
	i2cbus_Sda(p, 1);
#endif
	i2cbus_Scl(p, 1);
	nSda = i2cbus_Ste(p);
#if I2CBUS_IOMODE_CHANGE
	i2cbus_IoConf(p);
#endif

	if (nSda)
		return SYS_R_ERR;
	
	return SYS_R_OK;
}

static void i2cbus_Ack(i2c_t *p)
{

	i2cbus_Scl(p, 0);
	i2cbus_Sda(p, 0);
	i2cbus_Scl(p, 1);
	i2cbus_Scl(p, 0);
	i2cbus_Sda(p, 1);
}

static void i2cbus_Nack(i2c_t *p)
{

	i2cbus_Scl(p, 0);
	i2cbus_Sda(p, 1);
	i2cbus_Scl(p, 1);
}

static sys_res i2cbus_Send(i2c_t *p, int nData)
{
	int i;

	for (i = 0; i < 8; i++, nData <<= 1)
	{
		i2cbus_Scl(p, 0);
		i2cbus_Sda(p, nData & 0x80);
		i2cbus_Scl(p, 1);
	}

	return i2cbus_WaitAck(p);
}

static u8 i2cbus_Recv(i2c_t *p)
{
	int i, nData = 0;

#if I2CBUS_IOMODE_CHANGE
	i2cbus_IoInput(p);
#endif

	for (i = 0; i < 8; i++)
	{
		nData <<= 1;
		i2cbus_Scl(p, 0);
		i2cbus_Scl(p, 1);
		if (i2cbus_Ste(p))
			SETBIT(nData, 0);
	}
	
#if I2CBUS_IOMODE_CHANGE
	i2cbus_IoConf(p);
#endif

	return nData;
}

static sys_res _i2cbus_Write(i2c_t *p, int nDev, const u8 *pData, size_t nLen)
{

	if (i2cbus_Start(p) != SYS_R_OK)
		return SYS_R_ERR;
	
	if (i2cbus_Send(p, nDev & BITANTI(0)) != SYS_R_OK)
		return SYS_R_ERR;
	
	for (; nLen; nLen--)
	{
		if (i2cbus_Send(p, *pData++) != SYS_R_OK)
			return SYS_R_ERR;
	}
	
	i2cbus_Stop(p);
	
	return SYS_R_OK;
}

static sys_res _i2cbus_Read(i2c_t *p, int nDev, u8 *pBuf, size_t nLen)
{

	if (i2cbus_Start(p) != SYS_R_OK)
		return SYS_R_ERR;
	
	if (i2cbus_Send(p, nDev | BITMASK(0)) != SYS_R_OK)
		return SYS_R_ERR;
	
	for (; nLen; nLen--)
	{
		*pBuf++ = i2cbus_Recv(p);
		if (nLen == 1)
			i2cbus_Nack(p);
		else
			i2cbus_Ack(p);
	}
	
	i2cbus_Stop(p);
	
	return SYS_R_OK;
}



//-------------------------------------------------------------------
//External Functions
//-------------------------------------------------------------------
sys_res i2cbus_Init(i2c_t *p)
{
	tbl_gpio_def pScl = tbl_bspI2cDef[p->parent.id];
	int i;

	sys_GpioConf(pScl);
	sys_GpioConf(pScl + 1);
	
	for (i = 0; i < 10; i++)
	{
		i2cbus_Scl(p, 0);
		sys_Delay(0x200);
		i2cbus_Scl(p, 1);
		sys_Delay(0x200);
		
		if (i2cbus_Ste(p))
			break;
	}
	
	if (i == 0)
		return SYS_R_BUSY;
	
	return SYS_R_OK;
}

sys_res i2cbus_Write(i2c_t *p, int nDev, const void *pData, size_t nLen)
{
	int i;

	i2cbus_Lock();
	
	for (i = 0; i < 2; i++)
	{
		if (_i2cbus_Write(p, nDev, pData, nLen) == SYS_R_OK)
			break;
		
		i2cbus_Init(p);
	}

	i2cbus_Unlock();
	
	if (i >= 2)
		return SYS_R_ERR;
	
	return SYS_R_OK;
}


sys_res i2cbus_Read(i2c_t *p, int nDev, void *pData, size_t nLen)
{
	int i;

	i2cbus_Lock();
	
	for (i = 0; i < 2; i++)
	{
		if (_i2cbus_Read(p, nDev, pData, nLen) == SYS_R_OK)
			break;
		
		i2cbus_Init(p);
	}

	i2cbus_Unlock();

	if (i >= 2)
		return SYS_R_ERR;
	
	return SYS_R_OK;
}




