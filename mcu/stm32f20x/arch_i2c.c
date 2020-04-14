#if I2C_ENABLE && I2C_SOFTWARE == 0


static I2C_TypeDef * const stm32_tblI2cId[] = {I2C1, I2C2};

static void stm32_I2cApbClockCmd(I2C_TypeDef *pI2c, FunctionalState ste)
{

	switch ((u32)pI2c)
	{
	case I2C1_BASE:
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ste);
		break;
	case I2C2_BASE:
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ste);
		break;
	default:
		break;
	}
}

static sys_res stm32_I2cUnlock(i2c_t *p)
{
	GPIO_InitTypeDef xGpio;
	int i;

	xGpio.GPIO_Speed = GPIO_Speed_2MHz;
	//SCL ==> output
	xGpio.GPIO_Mode = GPIO_Mode_OUT;
	xGpio.GPIO_OType = GPIO_OType_OD;
	xGpio.GPIO_Pin = BITMASK(p->def->sclpin);
	arch_GpioClockEnable(p->def->sclport);
	GPIO_Init(arch_GpioPortBase(p->def->sclport), &xGpio);
	//SDA ==> input
	xGpio.GPIO_Mode = GPIO_Mode_IN;
	xGpio.GPIO_PuPd =GPIO_PuPd_NOPULL;
	xGpio.GPIO_Pin = BITMASK(p->def->sdapin);
	arch_GpioClockEnable(p->def->sdaport);
	GPIO_Init(arch_GpioPortBase(p->def->sdaport), &xGpio);

	for (i = 0; i < 10; i++)
	{
		GPIO_ResetBits(arch_GpioPortBase(p->def->sclport), BITMASK(p->def->sclpin));
		sys_Delay(0x2000);
		GPIO_SetBits(arch_GpioPortBase(p->def->sclport), BITMASK(p->def->sclpin));
		sys_Delay(0x2000);
		
		if (GPIO_ReadInputDataBit(arch_GpioPortBase(p->def->sdaport), BITMASK(p->def->sdapin)))
			break;
	}
	if (i < 10)
		return SYS_R_OK;
	
	return SYS_R_BUSY;
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
sys_res arch_I2cInit(i2c_t *p)
{
#if I2C_IRQ_ENABLE
	NVIC_InitTypeDef xNVIC;
#endif
	I2C_InitTypeDef  xI2C;
	GPIO_InitTypeDef xGpio;
	I2C_TypeDef *pI2c;

	pI2c = stm32_tblI2cId[p->parent.id];
	//Disable I2C Bus
#if 0
	stm32_I2cApbClockCmd(p->parent.id, ENABLE);
	I2C_Cmd(pI2c, DISABLE);
	stm32_I2cApbClockCmd(p->parent.id, DISABLE);
#else
	I2C_DeInit(pI2c);
#endif
	//Enable I2C Clock
	stm32_I2cApbClockCmd(pI2c, ENABLE);
	xGpio.GPIO_Speed = GPIO_Speed_2MHz;
	xGpio.GPIO_Mode = GPIO_Mode_AF;
	xGpio.GPIO_OType = GPIO_OType_OD;
	//SCL
	xGpio.GPIO_Pin = BITMASK(p->def->sclpin);
	arch_GpioClockEnable(p->def->sclport);
	GPIO_SetBits(arch_GpioPortBase(p->def->sclport), BITMASK(p->def->sclpin));
	GPIO_Init(arch_GpioPortBase(p->def->sclport), &xGpio);
	//SDA
	xGpio.GPIO_Pin = BITMASK(p->def->sdapin);
	arch_GpioClockEnable(p->def->sdaport);
	GPIO_SetBits(arch_GpioPortBase(p->def->sdaport), BITMASK(p->def->sdapin));
	GPIO_Init(arch_GpioPortBase(p->def->sdaport), &xGpio);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6 , GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7 , GPIO_AF_I2C1);
	
	xI2C.I2C_Mode = I2C_Mode_I2C;
	xI2C.I2C_DutyCycle = I2C_DutyCycle_2;
	xI2C.I2C_Ack = I2C_Ack_Disable;
	switch (p->def->adrmode)
	{
	default:
	case I2C_ADR_7BIT:
		xI2C.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
		break;
	case I2C_ADR_10BIT:
		xI2C.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_10bit;
		break;
	}
	
	xI2C.I2C_ClockSpeed = p->def->speed;
	I2C_Init(pI2c, &xI2C);
	I2C_Cmd(pI2c, ENABLE);

#if I2C_IRQ_ENABLE
	I2C_ITConfig(pI2c, I2C_IT_BUF | I2C_IT_EVT | I2C_IT_ERR, ENABLE);
	xNVIC.NVIC_IRQChannelCmd = ENABLE;
	xNVIC.NVIC_IRQChannelPreemptionPriority = 1;
	switch (p->parent.id)
	{
	default:
	case 0:
		xNVIC.NVIC_IRQChannel = I2C1_EV_IRQChannel;
		xNVIC.NVIC_IRQChannelSubPriority = 0;
		break;
	case 1:
		xNVIC.NVIC_IRQChannel = I2C2_EV_IRQChannel;
		xNVIC.NVIC_IRQChannelSubPriority = 1;
		break;
	}
	NVIC_Init(&xNVIC);
#else
	I2C_ITConfig(pI2c, I2C_IT_BUF | I2C_IT_EVT | I2C_IT_ERR, DISABLE);
#endif

	return SYS_R_OK;
}



void arch_I2cStart(i2c_t *p)
{
	I2C_TypeDef *pI2c = stm32_tblI2cId[p->parent.id];

	I2C_GenerateSTART(pI2c, ENABLE);
	I2C_AcknowledgeConfig(pI2c, ENABLE);
	I2C_ITConfig(pI2c, I2C_IT_EVT | I2C_IT_BUF, ENABLE);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void arch_I2cIoHandler(i2c_t *p)
{
	I2C_TypeDef *pI2c = stm32_tblI2cId[p->parent.id];
	u32 nSte;
	int nData;

	nSte = (pI2c->SR2 << 16) | pI2c->SR1;
	//SB
	if (nSte & BITMASK(0))
	{
		switch (p->ste)
		{
		case I2C_S_W_SB:
			p->ste = I2C_S_W_ADR;
			I2C_Send7bitAddress(pI2c, p->adr, I2C_Direction_Transmitter);
			break;
		case I2C_S_R_SB:
			p->ste = I2C_S_R_ADR;
			I2C_Send7bitAddress(pI2c, p->adr, I2C_Direction_Receiver);
			break;
		default:
			break;
		}
	}
	
	//ADDR
	if (nSte & BITMASK(1))
	{
		switch (p->ste)
		{
		case I2C_S_W_ADR:
			p->ste = I2C_S_W_TXE;
			break;
		case I2C_S_R_ADR:
			p->ste = I2C_S_RXNE;
			break;
		default:
			break;
		}
	}
	
	//TXE
	if (nSte & BITMASK(7))
	{
		switch (p->ste)
		{
		case I2C_S_W_TXE:
			if (p->wlen)
			{
				p->wlen -= 1;
				nData = dque_PopChar(p->parent.id | I2C_DQUE_TX_CHL);
				if (nData >= 0)
					pI2c->DR = nData;
				if (p->wlen == 0)
					I2C_GenerateSTOP(pI2c, ENABLE);
			}
			else
			{
				p->ste = I2C_S_W_STOP;
			}
			break;
		default:
			break;
		}
	}

	//RXNE
	if (nSte & BITMASK(6))
	{
		switch (p->ste)
		{
		case I2C_S_RXNE:

			break;
		default:
			break;
		}
	}
	
	//STOPF
	if (nSte & BITMASK(4))
	{
		switch (p->ste)
		{
		case I2C_S_W_STOP:
			p->ste = I2C_S_W_SUCCESS;
			break;
		default:
			break;
		}
	}
}

void arch_I2cErrorHandler(i2c_t *p)
{

	arch_I2cInit(p);
	
	p->ste = I2C_S_IDLE;
	p->wlen = 0;
	p->rlen = 0;
	
	dque_Clear(p->parent.id | I2C_DQUE_RX_CHL);
	dque_Clear(p->parent.id | I2C_DQUE_TX_CHL);
}



//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
sys_res arch_I2cWrite(i2c_t *p, int nDev, const u8  *pData, size_t nLen)
{
	I2C_TypeDef *pI2c = stm32_tblI2cId[p->parent.id];

	I2C_GenerateSTART(pI2c, ENABLE);
	I2C_AcknowledgeConfig(pI2c, ENABLE);
	
	while(!I2C_CheckEvent(pI2c, I2C_EVENT_MASTER_MODE_SELECT));  
	pI2c->DR = nDev & BITANTI(0);
	
	while(!I2C_CheckEvent(pI2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	
	for (; nLen; nLen--)
	{
		pI2c->DR = *pData++;
		while(!I2C_CheckEvent(pI2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	}
	
	I2C_GenerateSTOP(pI2c, ENABLE);
	I2C_AcknowledgeConfig(pI2c, DISABLE);
	
	return SYS_R_OK;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
sys_res arch_I2cRead(i2c_t *p, int nDev, u8 *pBuf, size_t nLen)
{
	I2C_TypeDef *pI2c = stm32_tblI2cId[p->parent.id];

	I2C_GenerateSTART(pI2c, ENABLE);
	I2C_AcknowledgeConfig(pI2c, ENABLE);
	
	while(!I2C_CheckEvent(pI2c, I2C_EVENT_MASTER_MODE_SELECT));
	
	pI2c->DR = nDev & BITANTI(0);
	while(!I2C_CheckEvent(pI2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	
	pI2c->DR = nDev;
	while(!I2C_CheckEvent(pI2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C_GenerateSTART(pI2c, ENABLE);
	while(!I2C_CheckEvent(pI2c, I2C_EVENT_MASTER_MODE_SELECT));
	
	pI2c->DR = nDev | BITMASK(0);
	while(!I2C_CheckEvent(pI2c, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	
	for (; nLen; nLen--)
	{
		while ((pI2c->SR1 & I2C_FLAG_RXNE) == 0);
		*pBuf++ = pI2c->DR;
		if (nLen == 1)
		{
			I2C_GenerateSTOP(I2C1, ENABLE);
			I2C_AcknowledgeConfig(I2C1, DISABLE);
		}
	}
	
	I2C_GenerateSTOP(I2C1, ENABLE);
	I2C_AcknowledgeConfig(I2C1, DISABLE);
	
	return SYS_R_OK;
}

void arch_I2cReset(i2c_t *p) 
{ 
	I2C_TypeDef *pI2c = stm32_tblI2cId[p->parent.id];
	GPIO_InitTypeDef xGPIO; 
	u32 nEvent;

	arch_I2cInit(p); 
	sys_Delay(200);
	nEvent = I2C_GetLastEvent(pI2c);
	
	if (nEvent & I2C_FLAG_BUSY)
	{
		//I2C bus recovery 
		I2C_Cmd(pI2c, DISABLE); 

		xGPIO.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; 
		xGPIO.GPIO_Speed = GPIO_Speed_50MHz; 
		xGPIO.GPIO_Mode = GPIO_Mode_OUT; 
		xGPIO.GPIO_OType = GPIO_OType_PP;
		GPIO_Init(GPIOB, &xGPIO); 

		//generate manual STOP condition 
		GPIO_ResetBits(GPIOB, GPIO_Pin_6); 
		GPIO_ResetBits(GPIOB, GPIO_Pin_7); 
		sys_Delay(200);
		GPIO_ResetBits(GPIOB, GPIO_Pin_6); 
		sys_Delay(200); 
		GPIO_SetBits(GPIOB, GPIO_Pin_7); 
		sys_Delay(400); 

		xGPIO.GPIO_Mode = GPIO_Mode_AF; 
		xGPIO.GPIO_OType = GPIO_OType_OD;
		GPIO_Init(GPIOB, &xGPIO); 

		I2C_Cmd(pI2c, ENABLE); 
	}
} 


#endif
