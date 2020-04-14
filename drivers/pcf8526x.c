#include <drivers/pcf8526x.h>





//Private Defines
#define PCF8526X_SLAVEADR			0xA2

#define PCF8526X_REG_SEC_100		0x00
#define PCF8526X_REG_SEC			0x01
#define PCF8526X_REG_MIN			0x02
#define PCF8526X_REG_HOUR			0x03
#define PCF8526X_REG_MDAY			0x04
#define PCF8526X_REG_WDAY			0x05
#define PCF8526X_REG_MONTH			0x06
#define PCF8526X_REG_YEAR			0x07

#define PCF8526X_REG_OCR			0x25



//-------------------------------------------------------------------
//External Functions
//-------------------------------------------------------------------
sys_res rtc_i2c_Init(i2c_t *p)
{

#if 0
	//24小时制使能
	return i2c_WriteByte(p, PCF8526X_SLAVEADR, PCF8526X_REG_OCR, 0x00);
#else	
	return SYS_R_OK;
#endif
}

sys_res rtc_i2c_GetTime(i2c_t *p, time_t *pTime)
{
	u8 aTime[7];

	if (i2c_WriteAdr(p, PCF8526X_SLAVEADR, PCF8526X_REG_SEC) != SYS_R_OK)
		return SYS_R_TMO;
	if (i2c_Read(p, PCF8526X_SLAVEADR, aTime, sizeof(aTime)) != SYS_R_OK)
		return SYS_R_TMO;

	aTime[0] &= 0x7F;
	aTime[1] &= 0x7F;
	
	memmove(&aTime[4], &aTime[5], 2);
	*pTime = array2timet(aTime, 1);

	return SYS_R_OK;
}

sys_res rtc_i2c_SetTime(i2c_t *p, time_t tTime)
{
	u8 aTime[6];

	timet2array(tTime, aTime, 1);

	if (i2c_WriteByte(p, PCF8526X_SLAVEADR, PCF8526X_REG_SEC, aTime[0]) != SYS_R_OK)
		return SYS_R_TMO;

	if (i2c_WriteByte(p, PCF8526X_SLAVEADR, PCF8526X_REG_MIN, aTime[1]) != SYS_R_OK)
		return SYS_R_TMO;

	if (i2c_WriteByte(p, PCF8526X_SLAVEADR, PCF8526X_REG_HOUR, aTime[2]) != SYS_R_OK)
		return SYS_R_TMO;

	if (i2c_WriteByte(p, PCF8526X_SLAVEADR, PCF8526X_REG_MDAY, aTime[3]) != SYS_R_OK)
		return SYS_R_TMO;

	if (i2c_WriteByte(p, PCF8526X_SLAVEADR, PCF8526X_REG_MONTH, aTime[4]) != SYS_R_OK)
		return SYS_R_TMO;

	if (i2c_WriteByte(p, PCF8526X_SLAVEADR, PCF8526X_REG_YEAR, aTime[5]) != SYS_R_OK)
		return SYS_R_TMO;

	return SYS_R_OK;
}


