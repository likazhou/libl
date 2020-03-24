#if RTC_TYPE == RTC_T_ISL1202X
#include <drivers/isl1202x.c>
#elif RTC_TYPE == RTC_T_DS323X
#include <drivers/ds323x.c>
#elif RTC_TYPE == RTC_T_R202X
#include <drivers/r202x.c>
#elif RTC_TYPE == RTC_T_RX802X
#include <drivers/rx802x.c>
#elif RTC_TYPE == RTC_T_M41T8X
#include <drivers/m41t8x.c>
#endif

//Private Defines
#define RTC_DEBUG_METHOD			0


//Internal Variables
static time_t rtc_tTime = 0;
static struct tm rtc_Tm;
static int rtc_nPause = 0;


//External Functions
void rtc_OsTick()
{

	if (rtc_nPause)
	{
		rtc_nPause = 0;
	}
	else
	{
		rtc_tTime += 1;
		localtime_r(&rtc_tTime, &rtc_Tm);
	}
}


void rtc_Maintian()
{
	time_t tTime = rtc_tTime;

#if RTC_TYPE & RTC_T_I2C
	i2c_t *p;

	if ((p = i2c_Open(RTC_COMID, OS_TMO_FOREVER)) != NULL)
	{
		rtc_i2c_GetTime(p, &tTime);
		i2c_Close(p);
	}
#elif RTC_TYPE == RTC_T_INT
	rtc_tTime = arch_RtcGet();
#elif RTC_TYPE == RTC_T_RNSS	
	rtc_tTime = rnss_GetTimet();
#endif

	if (tTime < rtc_tTime)
	{
		rtc_nPause = 1;
	}
	else
	{
		rtc_tTime = tTime;
		localtime_r(&rtc_tTime, &rtc_Tm);
	}
}


//-------------------------------------------------------------------------
//rtc_Init - 初始化RTC
//
//@
//
//Note: 
//
//Return: SYS_R_OK on success,  errno otherwise
//-------------------------------------------------------------------------
sys_res rtc_Init()
{
	sys_res res = SYS_R_OK;
#if RTC_TYPE & RTC_T_I2C
	i2c_t *p;

	if ((p = i2c_Open(RTC_COMID, OS_TMO_FOREVER)) != NULL)
	{
		res = rtc_i2c_Init(p);
		i2c_Close(p);
	}
	else
	{
		res = SYS_R_TMO;
	}
#elif RTC_TYPE == RTC_T_INT
	arch_RtcInit();
#endif

	rtc_Maintian();

	return res;
}


//-------------------------------------------------------------------------
//rtc_GetTimet - 获得time_t结构时间
//
//@
//
//Note: 
//
//Return: time_t on success,  errno otherwise
//-------------------------------------------------------------------------
time_t rtc_GetTimet()
{

	return rtc_tTime;
}


//-------------------------------------------------------------------------
//rtc_GetTm - 获得struct tm *结构时间
//
//@tmTime:存放获取的struct tm结构内容的指针
//
//Note: 
//
//Return: SYS_R_OK on success,  errno otherwise
//-------------------------------------------------------------------------
struct tm *rtc_pTm()
{

	return &rtc_Tm;
}


//-------------------------------------------------------------------------
//rtc_SetTimet - 通过time_t结构设置时间
//
//@tTime:设置的时间tTime结构
//
//Note: 
//
//Return: SYS_R_OK on success,  errno otherwise
//-------------------------------------------------------------------------
sys_res rtc_SetTimet(time_t tTime)
{
	sys_res res = SYS_R_OK;
#if RTC_TYPE & RTC_T_I2C
	i2c_t *p;

	if ((p = i2c_Open(RTC_COMID, OS_TMO_FOREVER)) != NULL)
	{
		res = rtc_i2c_SetTime(p, tTime);
		i2c_Close(p);
	}
	else
	{
		res = SYS_R_TMO;
	}
#elif RTC_TYPE == RTC_T_INT
	arch_RtcSet(tTime);
#endif

	rtc_tTime = tTime;

	return res;
}



