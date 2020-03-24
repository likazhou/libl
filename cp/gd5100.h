#ifndef __GD5100_H__
#define __GD5100_H__


//Public Defines



//Public Typedefs

//接收报文结构
typedef struct
{
 	u16	rtua;			//地市区县码
 	u16	terid;			//终端地址
 	u16	msta : 6,		//主站地址
 		fseq : 7,		//帧序号
 		iseq : 3;		//帧内序号
 	u8	code : 6,		//控制码
 		abn : 1,		//异常标志
 		dir : 1;		//传送方向
 	buf	data;			//接收数据
}gd5100_rmsg[1];


//规约结构
typedef struct
{
	struct dlrcp	parent;
	u8		flag;
	u8		pwd[3];
 	u16		rtua;
 	u16		terid;
	gd5100_rmsg		rmsg;
}gd5100_t;






//External Functions
void gd5100_Init(gd5100_t *p, int nFlag);
sys_res gd5100_TmsgSend(gd5100_t *p, int nCode, buf b, int nType);
sys_res gd5100_TmsgError(gd5100_t *p, int nCode, int nErr);
sys_res gd5100_Transmit(gd5100_t *p, gd5100_t *pD);
int gd5100_RecvCheck(gd5100_t *p);
sys_res gd5100_Handler(gd5100_t *p);

void gd5100_Response(gd5100_t *p);



#endif

