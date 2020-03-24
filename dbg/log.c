
//Private Defines
#define LOG_LOCK_ENABLE			1
#define LOG_CODE_INDEX			0xFFFF3947
#define LOG_CODE_READ			0xFFFF3948


//Private Typedefs
struct log
{
	u8	type;
	u8	reserve;
	time_t	time;
} PACK_STRUCT_STRUCT;
typedef struct log log_t;


//Private Macros
#if LOG_LOCK_ENABLE
#define log_Lock()				os_sem_wait(&log_sem)
#define log_Unlock()			os_sem_signal(&log_sem)
#else
#define log_Lock()
#define log_Unlock()
#endif

//Private Variables
#if LOG_LOCK_ENABLE
static os_sem_t log_sem;
#endif
static u8 log_buf[sizeof(log_t) + DEBUG_LOG_SIZE];


//Defined in App
extern flash_dev_t log_Sfs;


//Internal Functions
static void log_Decode(buf b, size_t nLen)
{
	char str[24];
	u8 aTime[6], *p, *pEnd;
	log_t xLog;
	
	buf_Push(b, STRING_0D0A, 2);
	memcpy(&xLog, &log_buf[0], sizeof(log_t));

	timet2array(xLog.time, aTime, 1);
	sprintf(str, "%X-%X %02X:%02X:%02X ", aTime[4], aTime[3], aTime[2], aTime[1], aTime[0]);
	buf_Push(b, str, strlen(str));

	p = &log_buf[sizeof(log_t)];

	if (xLog.type == LOG_T_STRING)
	{
		buf_Push(b, p, nLen);
	}
	else
	{
		switch (xLog.type)
		{
		case LOG_T_GW3762T:
			buf_Push(b, "<376.2T>", 8);
			break;
		case LOG_T_GW3762R:
			buf_Push(b, "<376.2R>", 8);
			break;
		default:
			buf_Push(b, "<Unknow>", 8);
			break;
		}

		for (pEnd = p + nLen; p < pEnd; )
		{
			sprintf(str, " %02X", *p++);
			buf_Push(b, str, 3);
		}
	}
}



//External Functions
void log_Init()
{
	u32 nIdx;

#if LOG_LOCK_ENABLE
	os_sem_init(&log_sem, 1);
#endif
	if (sfs_Read(&log_Sfs, LOG_CODE_INDEX, &nIdx, 4) < 0)
	{
		sfs_Init(&log_Sfs);
		nIdx = 0;
		sfs_Write(&log_Sfs, LOG_CODE_INDEX, &nIdx, 4);
	}

	if (sfs_Read(&log_Sfs, LOG_CODE_READ, &nIdx, 4) < 0)
	{
		nIdx = 0;
		sfs_Write(&log_Sfs, LOG_CODE_READ, &nIdx, 4);
	}
}

void log_Write(int nType, const void *pData, size_t nLen)
{
	size_t nWlen;
	u32 nIdx = 0;
	log_t xLog;

	log_Lock();

	nLen = MIN(nLen, DEBUG_LOG_SIZE);
	nWlen = sizeof(log_t) + nLen;
	sfs_Read(&log_Sfs, LOG_CODE_INDEX, &nIdx, 4);

	xLog.time = rtc_GetTimet();
	xLog.type = nType;

	memcpy(&log_buf[0], &xLog, sizeof(log_t));
	memcpy(&log_buf[sizeof(log_t)], pData, nLen);

	sfs_Write(&log_Sfs, nIdx % DEBUG_LOG_QTY, log_buf, nWlen);
	nIdx += 1;
	sfs_Write(&log_Sfs, LOG_CODE_INDEX, &nIdx, 4);

	log_Unlock();
}

int log_Read(buf b)
{
	u32 nIdx = 0, nRead = 0;
	size_t nLen = 0;

	log_Lock();

	sfs_Read(&log_Sfs, LOG_CODE_INDEX, &nIdx, 4);
	sfs_Read(&log_Sfs, LOG_CODE_READ, &nRead, 4);

	if (nRead > nIdx)
	{
		nRead = nIdx;
		sfs_Write(&log_Sfs, LOG_CODE_READ, &nRead, 4);
	}

	if (nRead != nIdx)
	{
		if ((nIdx - nRead) >= DEBUG_LOG_QTY)
			nRead = nIdx - (DEBUG_LOG_QTY - 1);
		
		nLen = sfs_Read(&log_Sfs, nRead % DEBUG_LOG_QTY, log_buf, sizeof(log_buf)) - sizeof(log_t);
		
		if (nLen > 0)
			log_Decode(b, nLen);
		
		nRead += 1;
		sfs_Write(&log_Sfs, LOG_CODE_READ, &nRead, 4);
	}

	log_Unlock();
	return nLen;
}

void log_Read2Start()
{
	u32 nIdx = 0, nRead = 0;

	log_Lock();

	sfs_Read(&log_Sfs, LOG_CODE_INDEX, &nIdx, 4);

	if (nIdx >= DEBUG_LOG_QTY)
		nRead = nIdx - (DEBUG_LOG_QTY - 1);

	sfs_Write(&log_Sfs, LOG_CODE_READ, &nRead, 4);

	log_Unlock();
}

void log_Read2End()
{
	u32 nIdx = 0;

	log_Lock();

	sfs_Read(&log_Sfs, LOG_CODE_INDEX, &nIdx, 4);
	sfs_Write(&log_Sfs, LOG_CODE_READ, &nIdx, 4);

	log_Unlock();
}

void log_Read2Day0()
{
	u32 nIdx = 0, nRead = 0;
	size_t nLen;
	log_t xLog;
	time_t tTime = getday0(rtc_GetTimet());
	
	log_Lock();

	sfs_Read(&log_Sfs, LOG_CODE_INDEX, &nIdx, 4);
	sfs_Read(&log_Sfs, LOG_CODE_READ, &nRead, 4);

	if (nRead > nIdx)
	{
		nRead = nIdx;
	}
	else
	{
		if ((nIdx - nRead) >= DEBUG_LOG_QTY)
			nRead = nIdx - (DEBUG_LOG_QTY - 1);
	}

	for (; nRead < nIdx; nRead += 1)
	{
		nLen = sfs_Read(&log_Sfs, nRead % DEBUG_LOG_QTY, log_buf, sizeof(log_buf)) - sizeof(log_t);
		if (nLen > 0)
		{
			memcpy(&xLog, &log_buf[0], sizeof(log_t));
			if (xLog.time > tTime)
				break;
		}
	}

	sfs_Write(&log_Sfs, LOG_CODE_READ, &nRead, 4);

	log_Unlock();
}

