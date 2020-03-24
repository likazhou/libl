
//Private Defines
#define MEMORY_LOCK_ENABLE		0


//Private Macros
#if MEMORY_LOCK_ENABLE
#define mem_Lock()				os_thd_lock()
#define mem_Unlock()			os_thd_unlock()
#else
#define mem_Lock(...)
#define mem_Unlock(...)
#endif


#if OS_TYPE == OS_T_RTTHREAD
void *malloc(size_t size)
{

	return rt_malloc(size);
}
void *realloc(void *ptr, size_t size)
{

	return rt_realloc(ptr, size);
}
void free(void *ptr)
{

	rt_free(ptr);
}
#endif

#if OS_TYPE == OS_T_CHRT || OS_TYPE == OS_T_CHNIL
void *malloc(size_t size)
{

	return chHeapAlloc(NULL, size);
}
void *realloc(void *ptr, size_t size)
{
	void *p;

	p = chHeapAlloc(NULL, size);
	if (ptr != NULL)
	{
		memcpy(p, ptr, size);
		chHeapFree(ptr);
	}

	return p;
}
void free(void *ptr)
{

	chHeapFree(ptr);
}
#endif

//void *malloc(size_t size) __attribute__((weak, alias("_malloc")));
//void *realloc(void *ptr, size_t size) __attribute__((weak, alias("_realloc")));
//void free(void *prt) __attribute__((weak, alias("_free")));



#if DEBUG_MEMORY_ENABLE
extern void list_memdebug(int nStart, int nEnd);

typedef struct {
	u16		size;
	u16		linemalloc;
	void *	p;
	const char *fnmalloc;
} t_mem_debug;


static int mem_nCnt = 0;
static t_mem_debug mem_debug[64];


static void mem_DebugMalloc(void *p, size_t nSize, const char *fn, const int line)
{
	t_mem_debug *pDebug;

	if (p == NULL)
		return;
	
	mem_Lock();
	
	for (pDebug = &mem_debug[0]; pDebug < &mem_debug[mem_nCnt]; pDebug++)
	{
		if (pDebug->p == p)
		{
			list_memdebug(0, ARR_SIZE(mem_debug));
		}
	}
	
	if (mem_nCnt < ARR_SIZE(mem_debug))
	{
		pDebug = &mem_debug[mem_nCnt];
		pDebug->p = p;
		pDebug->size = nSize;
		pDebug->linemalloc = line;
		pDebug->fnmalloc = fn;
	}
	mem_nCnt += 1;
	RT_ASSERT(mem_nCnt < ARR_SIZE(mem_debug));
	
	mem_Unlock();
}

static void mem_DebugFree(void *p, const char *fn, const int line)
{
	t_mem_debug *pDebug;

	if (p == NULL)
		return;

	mem_Lock();
	
	for (pDebug = &mem_debug[0]; pDebug < &mem_debug[mem_nCnt]; pDebug++)
	{
		if (pDebug->p == p)
		{
			memmove(pDebug, pDebug + 1, (uint)&mem_debug[mem_nCnt] - (uint)pDebug);
			break;
		}
	}
	
	if (pDebug == &mem_debug[mem_nCnt])
	{
		list_memdebug(0, ARR_SIZE(mem_debug));
	}
	mem_nCnt -= 1;
	RT_ASSERT(mem_nCnt >= 0);

	mem_Unlock();
}

static void mem_DebugRealloc(void *p, void *pOld, const char *fn, const int line)
{
	t_mem_debug *pDebug;

	mem_Lock();

	for (pDebug = &mem_debug[0]; pDebug < &mem_debug[mem_nCnt]; pDebug++)
	{
		if (pDebug->p == pOld)
		{
			pDebug->p = p;
			pDebug->linemalloc = line;
			pDebug->fnmalloc = fn;
			break;
		}
	}
	
	if (pDebug == &mem_debug[mem_nCnt])
	{
		list_memdebug(0, ARR_SIZE(mem_debug));
	}
	RT_ASSERT(mem_nCnt >= 0);

	mem_Unlock();
}
#endif


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
#if DEBUG_MEMORY_ENABLE
void *_mem_Malloc(size_t nSize, const char *fn, const int line)
#else
void *mem_Malloc(size_t nSize)
#endif
{
	void *p;

	mem_Lock();
	
#if DEBUG_MEMORY_ENABLE
	p = _rt_malloc(nSize);
#else
	p = malloc(nSize);
#endif
	
	mem_Unlock();

#if DEBUG_MEMORY_ENABLE
	mem_DebugMalloc(p, nSize, fn, line);
#endif

	return p;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
#if DEBUG_MEMORY_ENABLE
void *_mem_Realloc(void *pOld, size_t nSize, const char *fn, const int line)
#else
void *mem_Realloc(void *pOld, size_t nSize)
#endif
{
	void *p;

	if (pOld == NULL)
	{
#if DEBUG_MEMORY_ENABLE
		return _mem_Malloc(nSize, fn, line);
#else
		return mem_Malloc(nSize);
#endif
	}
	if (nSize == 0)
	{
#if DEBUG_MEMORY_ENABLE
		_mem_Free(pOld, fn, line);
#else
		mem_Free(pOld);
#endif
		return NULL;
	}

	mem_Lock();

#if DEBUG_MEMORY_ENABLE
	p = _rt_realloc(pOld, nSize);
#else
	p = realloc(pOld, nSize);
#endif

	mem_Unlock();

#if DEBUG_MEMORY_ENABLE
	mem_DebugRealloc(p, pOld, fn, line);
#endif

	return p;
}


void *mem_Calloc(size_t nCount, size_t nSize)
{
	void *p;

	nSize *= nCount;
	p = mem_Malloc(nSize);

	if (p != NULL)
		memset(p, 0, nSize);

	return p;
}



//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
#if DEBUG_MEMORY_ENABLE
void _mem_Free(void *p, const char *fn, const int line)
#else
void mem_Free(void *p)
#endif
{

	mem_Lock();
	
#if DEBUG_MEMORY_ENABLE
	_rt_free(p);
#else
	free(p);
#endif

	mem_Unlock();

#if DEBUG_MEMORY_ENABLE
	mem_DebugFree(p, fn, line);
#endif
}


#if DEBUG_MEMORY_ENABLE
void list_memdebug(int nStart, int nEnd)
{
#ifdef RT_USING_FINSH
	extern void list_mem(void);
#endif
	t_mem_debug *p;
	size_t nSize = 0;

	mem_Lock();
	
	if (nEnd == 0)
		nEnd = mem_nCnt;
	
	for (p = &mem_debug[nStart]; p < &mem_debug[nEnd]; p++)
	{
		if (p->size < 12)//MIN_SIZE = 12, defined in mem.c
			nSize += (12 + 0x0c);
		else
			nSize += (p->size + 0x0c);
	}
	
#ifdef RT_USING_FINSH
	list_mem();
#endif

	mem_Unlock();
}
#ifdef RT_USING_FINSH
#include <hi/finsh/finsh.h>
FINSH_FUNCTION_EXPORT(list_memdebug, list memory debug information)
FINSH_VAR_EXPORT(mem_nCnt, finsh_type_int, memory alloced counter)
#endif

#endif

