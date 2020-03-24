#ifndef __SYS_BATTERY_H__
#define __SYS_BATTERY_H__


#ifdef __cplusplus
extern "C" {
#endif







//External Functions
void bat_On(void);
void bat_Off(void);
float bat_Voltage(void);
int bat_IsPowerOn(void);
int bat_IsBatteryOff(void);
int bat_GetPowerOffCnt(void);
void bat_Maintian(void);



#ifdef __cplusplus
}
#endif

#endif

