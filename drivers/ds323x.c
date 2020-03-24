#include <drivers/ds323x.h>








//Private Defines
#define DS323X_DEBUG_METHOD		0

#define DS323X_SLAVEADR			0xD0

#define DS323X_REG_SEC			0x00
#define DS323X_REG_MIN			0x01
#define DS323X_REG_HOUR			0x02
#define DS323X_REG_MDAY			0x04
#define DS323X_REG_MONTH		0x05
#define DS323X_REG_YEAR			0x06

#define DS323X_REG_CR			0x0E
#define DS323X_REG_SR			0x0F



//-------------------------------------------------------------------
//External Functions
//-------------------------------------------------------------------
sys_res rtc_i2c_Init(i2c_t *p)
{

	return i2c_WriteByte(p, DS323X_SLAVEADR, DS323X_REG_CR, 0);
}

sys_res rtc_i2c_GetTime(i2c_t *p, time_t *pTime)
{
	u8 aTime[7];

	if (i2c_WriteAdr(p, DS323X_SLAVEADR, DS323X_REG_SEC) != SYS_R_OK)
		return SYS_R_TMO;
	if (i2c_Read(p, DS323X_SLAVEADR, aTime, sizeof(aTime)) != SYS_R_OK)
		return SYS_R_TMO;
	rt_memmove(&aTime[3], &aTime[4], 3);
#if DS323X_DEBUG_METHOD
	rt_kprintf("<<<Ds3231 TimeReg>>> %02X-%02X-%02X %02X:%02X:%02X\n", aTime[0], aTime[1], aTime[2], aTime[3], aTime[4], aTime[5]);
#endif
	if (aTime[2] & 0x40) {
		if (aTime[2] & 0x20)
			aTime[2] = (aTime[2] & 0x1F) + 0x12;
		else
			aTime[2] = (aTime[2] & 0x1F);
	}
	*pTime = array2timet(aTime, 1);
	return SYS_R_OK;
}

sys_res rtc_i2c_SetTime(i2c_t *p, time_t tTime)
{
	u8 aTime[8];

	aTime[0] = DS323X_REG_SEC;
	timet2array(tTime, &aTime[1], 1);
	rt_memmove(&aTime[5], &aTime[4], 3);
	if (i2c_Write(p, DS323X_SLAVEADR, aTime, sizeof(aTime)) != SYS_R_OK)
		return SYS_R_TMO;
	return SYS_R_OK;
}



