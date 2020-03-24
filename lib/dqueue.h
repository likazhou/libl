#ifndef __DQUEUE_H__
#define __DQUEUE_H__


#ifdef __cplusplus
extern "C" {
#endif


// Public Defines
#define DQUE_BLK_SIZE 			128			//¿é´óÐ¡(<256)



struct dque_mb
{
	struct dque_mb *next;
	u16	chl;
	u8	in;
	u8	out;
	u8	data[DQUE_BLK_SIZE];
};


struct dque_list
{
	struct dque_mb *start;
	struct dque_mb *end;
};
typedef const struct dque_list dque[1];




//External Functions
#if DQUE_OBJECT_ENABLE
void dque_Init(dque dq);
int dque_Pop(dque dq, int chl, void *buf, size_t len);
int dque_Pop2Buf(dque dq, int chl, buf b);
int dque_PopChar(dque dq, int chl);
void dque_Push(dque dq, int chl, int c);
int dque_IsNotEmpty(dque dq, int chl);
void dque_Clear(dque dq, int chl);
#else
int dque_Pop(int chl, void *buf, size_t len);
int dque_Pop2Buf(int chl, buf b);
int dque_PopChar(int chl);
void dque_Push(int chl, int c);
void dque_Wait(void);
int dque_GetLen(int chl);
void dque_Clear(int chl);
#endif



#ifdef __cplusplus
}
#endif

#endif 


