#include <string.h>


//Private Defines
#define DQUE_LOCK_ENABLE		1


//Private Variables
#if DQUE_LOCK_ENABLE
static os_sem_t dq_sem;
#endif

#if DQUE_OBJECT_ENABLE == 0
static struct dque_mb			__dq_allinone[DQUEUE_QTY] = {0};
dque							dq_all = {&__dq_allinone[0], ARR_ENDADR(__dq_allinone)};

static os_mbox_t				dq_mb;
static u32 						dq_mb_pool[DQUEUE_QTY * 4];
#endif


//Private Macros
#if DQUE_LOCK_ENABLE
#define dque_Lock()				os_sem_wait(&dq_sem)
#define dque_Unlock()			os_sem_signal(&dq_sem)
#else
#define dque_Lock()
#define dque_Unlock()
#endif


//Internal Functions
//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
static void dque_Release(struct dque_mb *p)
{

	p->chl = 0;
	p->in = 0;
	p->out = 0;
	p->next = NULL;
}

static struct dque_mb *dque_First(dque dq, int chl)
{
	struct dque_mb *p;

	for (p = dq->start; p < dq->end; p++)
	{
		if (p->chl == chl)
			return p;
	}

	return NULL;
}


//External Functions
//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void dque_SystemInit()
{

#if DQUE_LOCK_ENABLE
	os_sem_init(&dq_sem, 1);
#endif

#if DQUE_OBJECT_ENABLE == 0
	os_mb_init(&dq_mb, &dq_mb_pool[0], sizeof(dq_mb_pool) / sizeof(u32));
#endif
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
#if DQUE_OBJECT_ENABLE
void dque_Init(dque dq)
#else
void _dque_Init(dque dq)
#endif
{

	memset(dq->start, 0, dq->end - dq->start);
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
#if DQUE_OBJECT_ENABLE
int dque_Pop(dque dq, int chl, void *buf, size_t len)
#else
int _dque_Pop(dque dq, int chl, void *buf, size_t len)
#endif
{
	int have;
	u8 *pd = (u8 *)buf, *end = pd + len;
	struct dque_mb *p, *pn;

	dque_Lock();

	for (p = dque_First(dq, chl); p && (pd < end); )
	{
		have = p->in - p->out;
		if (have <= 0)
			break;

		len = MIN(have, end - pd);
		pd = ptrcpy(pd, &p->data[p->out], len);
		p->out += len;

		if (p->out >= DQUE_BLK_SIZE)
		{
			pn = p->next;
			dque_Release(p);
			p = pn;
			
			if (p)
				p->chl = chl;
		}
	}

	dque_Unlock();

	return (pd - (u8 *)buf);
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
#if DQUE_OBJECT_ENABLE
int dque_Pop2Buf(dque dq, int chl, buf b)
#else
int _dque_Pop2Buf(dque dq, int chl, buf b)
#endif
{
	size_t old = b->len;
	int have;
	struct dque_mb *p, *pn;

	dque_Lock();

	for (p = dque_First(dq, chl); p; )
	{
		have = p->in - p->out;
		if (have <= 0)
			break;

		buf_Push(b, &p->data[p->out], have);
		p->out += have;

		if (p->out >= DQUE_BLK_SIZE)
		{
			pn = p->next;
			dque_Release(p);
			p = pn;

			if (p)
				p->chl = chl;
		}
	}
	
	dque_Unlock();
	
	return (b->len - old);
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
#if DQUE_OBJECT_ENABLE
int dque_PopChar(dque dq, int chl)
#else
int _dque_PopChar(dque dq, int chl)
#endif
{
	int nData = -1;
	struct dque_mb *p;
	
	dque_Lock();

	p = dque_First(dq, chl);

	if (p)
	{
		if (p->in > p->out)
		{
			nData = p->data[p->out++];
			
			if (p->out >= DQUE_BLK_SIZE)
			{
				if (p->next)
					p->next->chl = chl;

				dque_Release(p);
			}
		}
	}
	
	dque_Unlock();
	
	return nData;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
#if DQUE_OBJECT_ENABLE
void dque_Push(dque dq, int chl, int c)
#else
void _dque_Push(dque dq, int chl, int c)
#endif
{
	struct dque_mb *p, *last = NULL;
	int nnew = 1;

	dque_Lock();

	for (p = dque_First(dq, chl); p; p = p->next)
	{
		//ÕÒ½áÎ²¿é
		if (p->next == NULL)
		{
			if (p->in < DQUE_BLK_SIZE)
			{
				p->data[p->in++] = c;
				nnew = 0;
			}

			last = p;
			break;
		}
	}

	//ÉêÇëÐÂ¿é
	if (nnew)
	{
		for (p = dq->start; p < dq->end; p++)
		{
			//·ÖÅä¿Õ¿é
			if (p->in == 0)
			{
				p->data[p->in++] = c;

				if (last)
					last->next = p;
				else
					p->chl = chl;
				
				break;
			}
		}
	}
	
	dque_Unlock();
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
#if DQUE_OBJECT_ENABLE
int dque_GetLen(dque dq, int chl)
#else
int _dque_GetLen(dque dq, int chl)
#endif
{
	int len = 0;
	struct dque_mb *p;

	for (p = dque_First(dq, chl); p; p = p->next)
	{
		len += (p->in - p->out);
	}
	
	return len;
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
#if DQUE_OBJECT_ENABLE
void dque_Clear(dque dq, int chl)
#else
void _dque_Clear(dque dq, int chl)
#endif
{
	struct dque_mb *p, *pn;

	dque_Lock();
	
	for (p = dque_First(dq, chl); p; )
	{
		pn = p->next;
		dque_Release(p);
		p = pn;
	}
	
	dque_Unlock();
}



#if DQUE_OBJECT_ENABLE == 0
__INLINE int dque_Pop(int chl, void *buf, size_t len)
{

	return _dque_Pop(dq_all, chl, buf, len);
}

__INLINE int dque_Pop2Buf(int chl, buf b)
{

	return _dque_Pop2Buf(dq_all, chl, b);
}

__INLINE int dque_PopChar(int chl)
{

	return _dque_PopChar(dq_all, chl);
}

void dque_Push(int chl, int c)
{
	u_word2 uv;

	uv.word[0] = c;
	uv.word[1] = chl;
	os_mb_send(&dq_mb, uv.n);
}

void dque_Wait()
{
	u_word2 uv;

	if (os_mb_recv(&dq_mb, (rt_uint32_t *)&uv) == RT_EOK)
		_dque_Push(dq_all, uv.word[1], uv.word[0]);
}

__INLINE int dque_GetLen(int chl)
{

	return _dque_GetLen(dq_all, chl);
}

__INLINE void dque_Clear(int chl)
{

	_dque_Clear(dq_all, chl);
}
#endif

