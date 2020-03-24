#include <drivers/rx802x.h>




//Private Defines
#define RX802X_SLAVEADR			0x64

#define RX802X_REG_SEC			0x00
#define RX802X_REG_MIN			0x01
#define RX802X_REG_HOUR			0x02
#define RX802X_REG_WDAY			0x03
#define RX802X_REG_MDAY			0x04
#define RX802X_REG_MONTH		0x05
#define RX802X_REG_YEAR			0x06

#define RX802X_REG_EXTR			0x0D
#define RX802X_REG_FLAG			0x0E
#define RX802X_REG_CTLR			0x0F



//-------------------------------------------------------------------
//External Functions
//-------------------------------------------------------------------
sys_res rtc_i2c_Init(i2c_t *p)
{

	//Ð´±êÖ¾¼Ä´æÆ÷
	if (i2c_WriteByte(p, RX802X_SLAVEADR, RX802X_REG_EXTR, 0) != SYS_R_OK)
		return SYS_R_TMO;

	if (i2c_WriteByte(p, RX802X_SLAVEADR, RX802X_REG_FLAG, 0) != SYS_R_OK)
		return SYS_R_TMO;

	if (i2c_WriteByte(p, RX802X_SLAVEADR, RX802X_REG_CTLR, 0x60) != SYS_R_OK)
		return SYS_R_TMO;

	return SYS_R_OK;
}

sys_res rtc_i2c_GetTime(i2c_t *p, time_t *pTime)
{
	u8 aTime[7];

	if (i2c_WriteAdr(p, RX802X_SLAVEADR, RX802X_REG_SEC) != SYS_R_OK)
		return SYS_R_TMO;

	if (i2c_Read(p, RX802X_SLAVEADR, aTime, sizeof(aTime)) != SYS_R_OK)
		return SYS_R_TMO;

	memmove(&aTime[3], &aTime[4], 3);
	*pTime = array2timet(&aTime[0], 1);

	return SYS_R_OK;
}

sys_res rtc_i2c_SetTime(i2c_t *p, time_t tTime)
{
	u8 aTime[8];

	aTime[0] = RX802X_REG_SEC;
	timet2array(tTime, &aTime[1], 1);
	memmove(&aTime[5], &aTime[4], 3);

	if (i2c_Write(p, RX802X_SLAVEADR, aTime, sizeof(aTime)) != SYS_R_OK)
		return SYS_R_TMO;

	return SYS_R_OK;
}


