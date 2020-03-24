#ifndef __GDVMS_H__
#define __GDVMS_H__


//Public Defines
#define GDVMS_ADR_SIZE					6



//Public Typedefs
//规约结构
struct gdvms
{
	struct dlrcp	parent;
	u8	ste;
	u8	adr[GDVMS_ADR_SIZE];
	u8	fseq;			//帧序号
	u8	code : 6,		//控制码
		abn : 1,		//异常标志
		dir : 1;		//传送方向
	buf	data;			//接收数据
};
typedef struct gdvms gdvms_t;






//External Functions
void gdvms_Init(gdvms_t *p);
sys_res gdvms_TmsgSend(gdvms_t *p, int nCode, buf b, int nType);
sys_res gdvms_TmsgError(gdvms_t *p, int nCode, int nErr);
sys_res gdvms_Transmit(gdvms_t *p, gdvms_t *pD);
sys_res gdvms_Handler(gdvms_t *p);

void gdvms_Response(gdvms_t *p);



#endif

