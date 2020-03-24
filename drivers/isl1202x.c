#include <drivers/isl1202x.h>





//Private Defines
#define ISL1202X_DEBUG_METHOD	0

#define ISL1202X_SLAVEADR		0xDE

#define ISL1202X_REG_SEC		0x00
#define ISL1202X_REG_MIN		0x01
#define ISL1202X_REG_HOUR		0x02
#define ISL1202X_REG_MDAY		0x03
#define ISL1202X_REG_MONTH		0x04
#define ISL1202X_REG_YEAR		0x05

#define ISL1202X_REG_SR			0x07
#define ISL1202X_REG_INT		0x08



//-------------------------------------------------------------------
//Internal Functions
//-------------------------------------------------------------------
static sys_res isl1202x_RegSet(i2c_t *p, int nReg, int nData)
{

	return i2c_WriteByte(p, ISL1202X_SLAVEADR, nReg, nData);
}


//-------------------------------------------------------------------
//External Functions
//-------------------------------------------------------------------
sys_res rtc_i2c_Init(i2c_t *p)
{
	u8 nSR;
	if (i2c_WriteAdr(p, ISL1202X_SLAVEADR, ISL1202X_REG_SR) != SYS_R_OK)
		return SYS_R_TMO;
	
	if (i2c_Read(p, ISL1202X_SLAVEADR, &nSR, 1) != SYS_R_OK)
		return SYS_R_TMO;
	
#if ISL1202X_DEBUG_METHOD & 1
	rt_kprintf("<<<Isl12022 StateReg>>> %X\n", nSR);
#endif

	//Initialize need
	if (nSR & 0x01)
		rtc_i2c_SetTime(p, 0);
	
	return SYS_R_OK;
}

sys_res rtc_i2c_GetTime(i2c_t *p, time_t *pTime)
{
	u8 aTime[6];

	if (i2c_WriteAdr(p, ISL1202X_SLAVEADR, ISL1202X_REG_SEC) != SYS_R_OK)
		return SYS_R_TMO;
	
	if (i2c_Read(p, ISL1202X_SLAVEADR, aTime, sizeof(aTime)) != SYS_R_OK)
		return SYS_R_TMO;
	
#if ISL1202X_DEBUG_METHOD & 1
	rt_kprintf("<<<Isl12022 TimeReg>>> %02X-%02X-%02X %02X:%02X:%02X\n", aTime[0], aTime[1], aTime[2], aTime[3], aTime[4], aTime[5]);
#endif

	if (aTime[2] & 0x80)
		aTime[2] &= 0x3F;
	else if (aTime[2] & 0x20)
		aTime[2] = (aTime[2] & 0x1F) + 0x12;
	
	*pTime = array2timet(aTime, 1);
	
	return SYS_R_OK;
}

sys_res rtc_i2c_SetTime(i2c_t *p, time_t tTime)
{
	u8 aTime[6];

	timet2array(tTime, aTime, 1);
	//Write Enable
	if (isl1202x_RegSet(p, ISL1202X_REG_INT, 0x40) != SYS_R_OK)
		return SYS_R_TMO;
	
	//Write Time
	if (isl1202x_RegSet(p, ISL1202X_REG_SEC, aTime[0]) != SYS_R_OK)
		return SYS_R_TMO;
	
	if (isl1202x_RegSet(p, ISL1202X_REG_MIN, aTime[1]) != SYS_R_OK)
		return SYS_R_TMO;
	
	if (isl1202x_RegSet(p, ISL1202X_REG_HOUR, aTime[2] | 0x80) != SYS_R_OK)
		return SYS_R_TMO;
	
	if (isl1202x_RegSet(p, ISL1202X_REG_MDAY, aTime[3]) != SYS_R_OK)
		return SYS_R_TMO;
	
	if (isl1202x_RegSet(p, ISL1202X_REG_MONTH, aTime[4]) != SYS_R_OK)
		return SYS_R_TMO;
	
	if (isl1202x_RegSet(p, ISL1202X_REG_YEAR, aTime[5]) != SYS_R_OK)
		return SYS_R_TMO;
	
	//Write Disable
	if (isl1202x_RegSet(p, ISL1202X_REG_INT, 0x00) != SYS_R_OK)
		return SYS_R_TMO;
	
	return SYS_R_OK;
}




