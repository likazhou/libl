#include <string.h>


//Private Defines
#define BUF_LOCK_ENABLE		0

#define BUF_BLK_SIZE		(1 << 7)
#define BUF_BLK_MASK		(BUF_BLK_SIZE - 1)


//Private Variables
#if BUF_LOCK_ENABLE
static os_sem_t buf_sem;
#endif


//Private Macros
#if BUF_LOCK_ENABLE
#define buf_Lock()			os_sem_wait(&buf_sem)
#define buf_Unlock()		os_sem_signal(&buf_sem)
#else
#define buf_Lock()
#define buf_Unlock()
#endif


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void buf_Init()
{

#if BUF_LOCK_ENABLE
	os_sem_init(&buf_sem, 1);
#endif
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
sys_res buf_Push(buf b, const void *p, size_t len)
{
	sys_res res = SYS_R_OK;
	size_t lnew, alloc;

	buf_Lock();
	
	lnew = b->len + len;
	alloc = (lnew + BUF_BLK_MASK) & ~BUF_BLK_MASK;
	
	if (((b->len + BUF_BLK_MASK) & ~BUF_BLK_MASK) != alloc)
	{
		b->p = mem_Realloc(b->p, alloc);
		if (b->p == NULL)
			res = SYS_R_ERR;
	}
	
	if (res == SYS_R_OK)
	{
		memcpy(b->p + b->len, p, len);
		b->len = lnew;
	}
	
	buf_Unlock();
	return res;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
sys_res buf_PushData(buf b, u64 data, size_t len)
{

	return buf_Push(b, &data, len);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
sys_res buf_Fill(buf b, int val, size_t len)
{
	sys_res res = SYS_R_ERR;
	size_t fill;
	u8 buf[64];

	memset(buf, val, sizeof(buf));

 	for (; len; len -= fill)
	{
		fill = MIN(sizeof(buf), len);
		res = buf_Push(b, buf, fill);

		if (res != SYS_R_OK)
			break;
	}
	return res;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
sys_res buf_Cut(buf b, int offset, size_t len)
{
	size_t alloc, lnew;
	
	buf_Lock();

	if (len > b->len)
		lnew = offset;
	else
		lnew = b->len - len;

	if (lnew == 0)
	{
		mem_Free(b->p);
		b->p = NULL;
	}
	else
	{
		alloc = (lnew + BUF_BLK_MASK) & ~BUF_BLK_MASK;
		memmove(b->p + offset, b->p + offset + len, lnew - offset);
		
		if (((b->len + BUF_BLK_MASK) & ~BUF_BLK_MASK) != alloc)
		{
			b->p = mem_Realloc(b->p, alloc);
			if (b->p == NULL)
			{
				buf_Unlock();
				return SYS_R_ERR;
			}
		}
	}

	b->len = lnew;

	buf_Unlock();
	return SYS_R_OK;
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void buf_Release(buf b)
{

	if (b->p == NULL)
		return;

	buf_Lock();
	
	mem_Free(b->p);

	b->p = NULL;
	b->len = 0;
	
	buf_Unlock();
}



