

//Private Defines
#define STM32_BKP_SIZE      16


/*******************************************************************************
* Function Name  : stm32_BKPinit
* Description    : 初始化BKP
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void arch_BkpInit(void)
{
	/* Enable PWR and BKP clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_AHB1Periph_BKPSRAM, ENABLE);

	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);
}

/*******************************************************************************
* Function Name  :stm32_WriteBKP
* Description    : 把以nStartAdr开头的nLen个字节写入BKP的寄存器，
				   每次写一个字节，内容为*P
* Input          : -nStartAdr:0~19的整数 ，BKP共10组寄存器，2个字节为一组
				-*p:写进BKP的内容，一次写一个字节
				-nLen:1~(20-nStartAdr)
* Output         : Non
* Return         : SYS_R_ERR 或SYS_R_OK
*******************************************************************************/
sys_res arch_BkpWrite(adr_t nStartAdr, const void *pData, size_t nLen)
{
	u32 nReg;
	const u8 *p = (const u8 *)pData;

	if ((int)nLen > (STM32_BKP_SIZE - nStartAdr))
		return SYS_R_ERR;
	
	nReg = ((nStartAdr >> 1) << 2) + 4;
	if (nStartAdr & 1)
	{
		RTC_WriteBackupRegister(nReg, (RTC_ReadBackupRegister(nReg) & 0x00FF) | (*p++ << 8));
		nLen -= 1;
	}
	
	for (; nLen; nReg += 4)
	{
		if (--nLen)
		{
			RTC_WriteBackupRegister(nReg, *p | *(p + 1) << 8);
			p += 2;
			nLen -= 1;
		}
		else
		{
			RTC_WriteBackupRegister(nReg, (RTC_ReadBackupRegister(nReg) & 0xFF00) | *p++);
		}
	}
	
	return SYS_R_OK;
}

/*******************************************************************************
* Function Name  :stm32_ReadBKP
* Description    : 把以nStartAdr开头的nLen个字节从BKP寄存器  读出
				   每次读一个字节，内容为*P
* Input          : -nStartAdr:0~19的整数 ，BKP共10组寄存器，2个字节为一组
				-*p: 被读出的BKP寄存器的内容，一次读一个字节
				-nLen:1~(20-nStartAdr)
* Output         : None
* Return         : SYS_R_ERR 或SYS_R_OK
*******************************************************************************/
sys_res arch_BkpRead(adr_t nStartAdr, void *pData, size_t nLen)
{
	u16 nTemp;
	u32 nReg;
	u8 *p = (u8 *)pData;

	if ((int)nLen > (STM32_BKP_SIZE - nStartAdr))
		return SYS_R_ERR;
	
	nReg = ((nStartAdr >> 1) << 2) + 4;
	if (nStartAdr & 1)
	{
		*p++ = RTC_ReadBackupRegister(nReg) >> 8;
		nLen--;
	}
	
	for (; nLen; nReg += 4)
	{
		nTemp = RTC_ReadBackupRegister(nReg);
		*p++ = nTemp;  //把低位赋给*p
		if (--nLen)
			*p++ = nTemp >> 8, nLen -= 1;
	}
	
	return SYS_R_ERR;
}

