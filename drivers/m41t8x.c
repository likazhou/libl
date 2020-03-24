#include <drivers/m41t8x.h>





//Private Defines
#define M41T8X_DEBUG_METHOD		0

#define M41T8X_SLAVEADR			0xD0

#define M41T8X_REG_SEC			0x01
#define M41T8X_REG_MIN			0x02
#define M41T8X_REG_HOUR			0x03
#define M41T8X_REG_WDAY			0x04
#define M41T8X_REG_MDAY			0x05
#define M41T8X_REG_MONTH		0x06
#define M41T8X_REG_YEAR			0x07

#define M41T8X_REG_CR0			0x08
#define M41T8X_REG_CR1			0x0C


//-------------------------------------------------------------------
//External Functions
//-------------------------------------------------------------------
sys_res rtc_i2c_Init(i2c_t *p)
{

	return SYS_R_OK;
}

sys_res rtc_i2c_GetTime(i2c_t *p, time_t *pTime)
{
	u8 aTime[7];

	if (i2c_WriteAdr(p, M41T8X_SLAVEADR, M41T8X_REG_CR1) != SYS_R_OK)
		return SYS_R_TMO;
	
	if (i2c_Read(p, M41T8X_SLAVEADR, aTime, 1) != SYS_R_OK)
		return SYS_R_TMO;
	
	if (aTime[0] & BITMASK(6))
		i2c_WriteByte(p, M41T8X_SLAVEADR, M41T8X_REG_CR1, 0);
	
	if (i2c_WriteAdr(p, M41T8X_SLAVEADR, M41T8X_REG_SEC) != SYS_R_OK)
		return SYS_R_TMO;
	
	if (i2c_Read(p, M41T8X_SLAVEADR, aTime, 7) != SYS_R_OK)
		return SYS_R_TMO;
	
	memmove(&aTime[3], &aTime[4], 3);
#if M41T8X_DEBUG_METHOD
	printf("<<<R202x TimeReg>>> %02X-%02X-%02X %02X:%02X:%02X\n", aTime[1], aTime[2], aTime[3], aTime[4], aTime[5], aTime[6]);
#endif
	*pTime = array2timet(aTime, 1);

	return SYS_R_OK;
}

sys_res rtc_i2c_SetTime(i2c_t *p, time_t tTime)
{
	u8 aTime[8];

	aTime[0] = M41T8X_REG_SEC;
	timet2array(tTime, &aTime[1], 1);
	memmove(&aTime[5], &aTime[4], 3);
	if (i2c_Write(p, M41T8X_SLAVEADR, aTime, sizeof(aTime)) != SYS_R_OK)
		return SYS_R_TMO;
	
	return SYS_R_OK;
}



