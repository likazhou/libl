#ifndef __LIB_MEMORY_H__
#define __LIB_MEMORY_H__



#ifdef __cplusplus
extern "C" {
#endif





//-------------------------------------------------------------------------
//Memory Management
//-------------------------------------------------------------------------
#if DEBUG_MEMORY_ENABLE
void *_mem_Malloc(size_t nSize, const char *fn, const int line);
void *_mem_Realloc(void *pOld, size_t nSize, const char *fn, const int line);
void _mem_Free(void *p, const char *fn, const int line);

#define mem_Malloc(s)			_mem_Malloc(s, __FUNCTION__, __LINE__)
#define mem_Realloc(p, s)		_mem_Realloc(p, s, __FUNCTION__, __LINE__)
#define mem_Free(p)				_mem_Free(p, __FUNCTION__, __LINE__)

#define rt_malloc 				mem_Malloc
#define rt_realloc				mem_Realloc
#define rt_calloc				mem_Calloc
#define rt_free					mem_Free

#else
void *mem_Malloc(size_t nSize);
void *mem_Realloc(void *p, size_t nSize);
void *mem_Calloc(size_t nCount, size_t nSize);
void mem_Free(void *p);

#endif



#ifdef __cplusplus
}
#endif


#endif

