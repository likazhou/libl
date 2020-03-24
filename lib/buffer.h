#ifndef __LIB_BUFFER_H__
#define __LIB_BUFFER_H__

//-------------------------------------------------------------------------
//Buffer Management
//-------------------------------------------------------------------------

//Public Typedefs
struct _buf
{
	size_t	len;
	u8 *	p;
} PACK_STRUCT_STRUCT;
typedef struct _buf buf[1];



//External Functions
#define buf_Unpush(b, l)		buf_Cut(b, (b)->len - (l), l)	//delete from (len - l) to len
#define buf_Remove(b, l)		buf_Cut(b, 0, l)				//delete from 0 to l


void buf_Init(void);
sys_res buf_Push(buf b, const void *p, size_t len);
sys_res buf_PushData(buf b, u64 data, size_t len);
sys_res buf_Fill(buf b, int val, size_t len);
sys_res buf_Cut(buf b, int offset, size_t len);
void buf_Release(buf b);

#endif


