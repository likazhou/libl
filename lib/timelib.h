#ifndef __LIB_TIME_H__
#define __LIB_TIME_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>


//Pulic typedef









//External Functions
time_t array2timet(u8 *p, int nIsBcd);
int timet2array(time_t tTime, u8 *p, int nIsBcd);
time_t getday0(time_t tTime);
time_t getmin0(time_t tTime);
void day4timet(time_t tTime, int nDay, u8 *p, int nIsBcd);
void nextmonth(u8 *p, int nIsBcd);
void prevmonth(u8 *p, int nIsBcd);
void month4timet(time_t tTime, int nMon, u8 *p, int nIsBcd);


#ifdef __cplusplus
}
#endif

#endif

