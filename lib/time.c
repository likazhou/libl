

#if LIB_MINILIBC_ENABLE
/* days per month -- nonleap! */
const short __spm[13] =
  { 0,
    (31),
    (31+28),
    (31+28+31),
    (31+28+31+30),
    (31+28+31+30+31),
    (31+28+31+30+31+30),
    (31+28+31+30+31+30+31),
    (31+28+31+30+31+30+31+31),
    (31+28+31+30+31+30+31+31+30),
    (31+28+31+30+31+30+31+31+30+31),
    (31+28+31+30+31+30+31+31+30+31+30),
    (31+28+31+30+31+30+31+31+30+31+30+31),
  };

/* seconds per day */
#define SPD 24*60*60

int __isleap(int year)
{
	/* every fourth year is a leap year except for century years that are
	 * not divisible by 400. */
	/*  return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)); */
	return (!(year % 4) && ((year % 100) || !(year % 400)));
}

struct tm *gmtime_r(const time_t *timep, struct tm *r)
{
	time_t i;
	register time_t work = *timep % (SPD);
	r->tm_sec = work % 60;
	work /= 60;
	r->tm_min = work % 60;
	r->tm_hour = work / 60;
	work = *timep / (SPD);
	r->tm_wday = (4 + work) % 7;
	for (i = 1970;; ++i)
	{
		register time_t k = __isleap(i) ? 366 : 365;
		if (work >= k)
			work -= k;
		else
			break;
	}
	r->tm_year = i - 1900;
	r->tm_yday = work;

	r->tm_mday = 1;
	if (__isleap(i) && (work > 58))
	{
		if (work == 59)
			r->tm_mday = 2; /* 29.2. */
		work -= 1;
	}

	for (i = 11; i && (__spm[i] > work); --i)
		;
	r->tm_mon = i;
	r->tm_mday += work - __spm[i];
	return r;
}

struct tm* localtime_r(const time_t* t, struct tm* r)
{

	return gmtime_r(t, r);
}

time_t mktime(struct tm * const t)
{
	register time_t day;
	register time_t i;
	register time_t years = t->tm_year - 70;

	if (t->tm_sec > 60)
	{
		t->tm_min += t->tm_sec / 60;
		t->tm_sec %= 60;
	}
	if (t->tm_min > 60)
	{
		t->tm_hour += t->tm_min / 60;
		t->tm_min %= 60;
	}
	if (t->tm_hour > 24)
	{
		t->tm_mday += t->tm_hour / 24;
		t->tm_hour %= 24;
	}
	if (t->tm_mon > 12)
	{
		t->tm_year += t->tm_mon / 12;
		t->tm_mon %= 12;
	}
	while (t->tm_mday > __spm[1 + t->tm_mon])
	{
		if (t->tm_mon == 1 && __isleap(t->tm_year + 1900))
		{
			--t->tm_mday;
		}
		t->tm_mday -= __spm[t->tm_mon];
		++t->tm_mon;
		if (t->tm_mon > 11)
		{
			t->tm_mon = 0;
			++t->tm_year;
		}
	}

	if (t->tm_year < 70)
		return (time_t) -1;

	/* Days since 1970 is 365 * number of years + number of leap years since 1970 */
	day = years * 365 + (years + 1) / 4;

	/* After 2100 we have to substract 3 leap years for every 400 years
	 This is not intuitive. Most mktime implementations do not support
	 dates after 2059, anyway, so we might leave this out for it's
	 bloat. */
	if (years >= 131)
	{
		years -= 131;
		years /= 100;
		day -= (years >> 2) * 3 + 1;
		if ((years &= 3) == 3)
			years--;
		day -= years;
	}

	day += t->tm_yday = __spm[t->tm_mon] + t->tm_mday - 1 +
			(__isleap(t->tm_year + 1900) & (t->tm_mon > 1));

	/* day is now the number of days since 'Jan 1 1970' */
	i = 7;
	t->tm_wday = (day + 4) % i; /* Sunday=0, Monday=1, ..., Saturday=6 */

	i = 24;
	day *= i;
	i = 60;
	return ((day + t->tm_hour) * i + t->tm_min) * i + t->tm_sec;
}
#endif


//-------------------------------------------------------------------------
//转换日期时间为time_t
//-------------------------------------------------------------------------
time_t array2timet(u8 *p, int nIsBcd)
{
	struct tm tmTime;

	if (nIsBcd)
	{
		tmTime.tm_sec = bcd2bin8(*p++);
		tmTime.tm_min = bcd2bin8(*p++);
		tmTime.tm_hour = bcd2bin8(*p++);
		tmTime.tm_mday = bcd2bin8(*p++);
		tmTime.tm_mon = bcd2bin8(*p++) - 1;
		tmTime.tm_year = bcd2bin8(*p) + 100;
	}
	else
	{
		tmTime.tm_sec = *p++;
		tmTime.tm_min = *p++;
		tmTime.tm_hour = *p++;
		tmTime.tm_mday = *p++;
		tmTime.tm_mon = *p++ - 1;
		tmTime.tm_year = *p + 100;
	}
	
	return mktime(&tmTime);
}

int timet2array(time_t tTime, u8 *p, int nIsBcd)
{
	struct tm tmTime;

	localtime_r(&tTime, &tmTime);
	if (nIsBcd)
	{
		*p++ = bin2bcd8(tmTime.tm_sec);
		*p++ = bin2bcd8(tmTime.tm_min);
		*p++ = bin2bcd8(tmTime.tm_hour);
		*p++ = bin2bcd8(tmTime.tm_mday);
		*p++ = bin2bcd8(tmTime.tm_mon + 1);
		*p = bin2bcd8(tmTime.tm_year - 100);
	}
	else
	{
		*p++ = tmTime.tm_sec;
		*p++ = tmTime.tm_min;
		*p++ = tmTime.tm_hour;
		*p++ = tmTime.tm_mday;
		*p++ = tmTime.tm_mon + 1;
		*p = tmTime.tm_year - 100;
	}
	
	return 1;
}

time_t getday0(time_t tTime)
{
	
	tTime /= (3600 * 24);
	tTime *= (3600 * 24);
	
	return tTime;
}

time_t getmin0(time_t tTime)
{
	
	tTime /= 60;
	tTime *= 60;
	
	return tTime;
}

void day4timet(time_t tTime, int nDay, u8 *p, int nIsBcd)
{
	u8 aTime[6];

	timet2array(tTime + nDay * (24 * 3600), aTime, nIsBcd);
	memcpy(p, &aTime[3], 3);
}

void nextmonth(u8 *p, int nIsBcd)
{
	
	if (nIsBcd)
	{
		if (p[0] >= 0x12)
		{
			p[0] = 1;
			p[1] = bin2bcd8(bcd2bin8(p[1]) + 1);
		}
		else
		{
			p[0] = bin2bcd8(bcd2bin8(p[0]) + 1);
		}
	}
	else
	{
		if (p[0] >= 12)
		{
			p[0] = 1;
			p[1] += 1;
		}
		else
		{
			p[0] += 1;
		}
	}
}

void prevmonth(u8 *p, int nIsBcd)
{

	if (nIsBcd)
	{
		if (p[0] <= 1)
		{
			p[0] = 0x12;
			p[1] = bin2bcd8(bcd2bin8(p[1]) - 1);
		}
		else
		{
			p[0] = bin2bcd8(bcd2bin8(p[0]) - 1);
		}
	}
	else
	{
		if (p[0] <= 1)
		{
			p[0] = 12;
			p[1] -= 1;
		}
		else
		{
			p[0] -= 1;
		}
	}
}

void month4timet(time_t tTime, int nMon, u8 *p, int nIsBcd)
{
	u8 aTime[6];
	int i;

 	timet2array(tTime, aTime, nIsBcd);
	
	if (nMon < 0)
	{
 		nMon = -nMon;
 		for (i = 0; i < nMon; i++)
		{
			prevmonth(&aTime[4], nIsBcd);
 		}
 		memcpy(p, &aTime[4], 2);
	}
	else
	{
		for (i = 0; i < nMon; i++)
		{
			nextmonth(&aTime[4], nIsBcd);
		}
		memcpy(p, &aTime[4], 2);
	}
}




