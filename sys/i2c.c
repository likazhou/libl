



//Public Variables
static i2c_t dev_I2c[BSP_I2C_QTY];






//-------------------------------------------------------------------------
//Function Name  : 
//Description    : 
//Input          : - GPIOx:
//                - GPIO_InitStruct: 
//Output         : None
//Return         : None
//-------------------------------------------------------------------------
i2c_t *i2c_Open(int nId, size_t nTmo)
{
	i2c_t *p = &dev_I2c[nId];

#if OS_TYPE
	if (dev_Open(&p->parent, nTmo) != SYS_R_OK)
		return NULL;
#endif
	
	return p;
}


//-------------------------------------------------------------------------
//Function Name  : 
//Description    : 
//Input          : - GPIOx:
//                - GPIO_InitStruct: 
//Output         : None
//Return         : None
//-------------------------------------------------------------------------
sys_res i2c_Close(i2c_t *p)
{

#if OS_TYPE
	
	return dev_Close(&p->parent);
#else
	
	return SYS_R_OK;
#endif
}


//-------------------------------------------------------------------------
//Function Name  : 
//Description    : 
//Input          : - GPIOx:
//                - GPIO_InitStruct: 
//Output         : None
//Return         : None
//-------------------------------------------------------------------------
sys_res i2c_Write(i2c_t *p, int nDev, const void *pData, size_t nLen)
{
	sys_res res;

	if (p->ste == I2C_S_IDLE)
	{
		p->adr = nDev;
		p->ste = I2C_S_W_SB;
#if I2C_SOFTWARE
		res = i2cbus_Write(p, p->adr, pData, nLen);
#else
#if I2C_IRQ_ENABLE
		dque_Push(p->parent.id | I2C_DQUE_TX_CHL, pData, nLen);
		p->wlen = nLen;
		arch_I2cStart(p);
#else
		res = arch_I2cWrite(p, p->adr, pData, nLen);
#endif
#endif
		p->ste = I2C_S_IDLE;

		return res;
		
	}
	
	return SYS_R_BUSY;
}



//-------------------------------------------------------------------------
//Function Name  : 
//Description    : 
//Input          : - GPIOx:
//                - GPIO_InitStruct: 
//Output         : None
//Return         : None
//-------------------------------------------------------------------------
sys_res i2c_Read(i2c_t *p, int nDev, void *pData, size_t nLen)
{
	sys_res res;

	if (p->ste == I2C_S_IDLE)
	{
		p->adr = nDev;
		p->ste = I2C_S_R_SB;
#if I2C_SOFTWARE
		res = i2cbus_Read(p, p->adr, pData, nLen);
#else
#if I2C_IRQ_ENABLE

#else
		res = arch_I2cRead(p, p->adr, pData, nLen);
#endif
#endif
		p->ste = I2C_S_IDLE;

		return res;
	}
	
	return SYS_R_BUSY;
}


sys_res i2c_WriteByte(i2c_t *p, int nDev, int nAdr, int nData)
{
	u8 aTemp[2];

	aTemp[0] = nAdr;
	aTemp[1] = nData;
	return i2c_Write(p, nDev, aTemp, 2);
}

sys_res i2c_WriteAdr(i2c_t *p, int nDev, int nAdr)
{

	return i2c_Write(p, nDev, &nAdr, 1);
}




