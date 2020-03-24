#if RTC_ENABLE


//Private Defines
//最大数据域长度
#define GWVMS_DATA_SIZE					1453

//命令状态
#define GWVMS_CMD_SUCC				0xFF	//成功
#define GWVMS_CMD_FAIL				0x00	//失败

//帧类型定义
#define GWVMS_FTYPE_KEEPALIVE			0x01	//心跳
#define GWVMS_FTYPE_KEEPALIVE_R			0x02	//心跳确认
#define GWVMS_FTYPE_MEASURE				0x03	//监测数据
#define GWVMS_FTYPE_MEASURE_R			0x04	//监测数据确认
#define GWVMS_FTYPE_DATEREQUEST			0x05	//数据请求
#define GWVMS_FTYPE_DATEREQUEST_R		0x06	//数据请求确认
#define GWVMS_FTYPE_PARASET				0x07	//参数查询设置
#define GWVMS_FTYPE_PARASET_R			0x08	//参数查询设置响应
#define GWVMS_FTYPE_FLOW				0x09	//流量数据
#define GWVMS_FTYPE_FLOW_R				0x10	//流量数据确认
#define GWVMS_FTYPE_EVENT				0x11	//事件信息
#define GWVMS_FTYPE_EVENT_R				0x12	//事件信息确认
#define GWVMS_FTYPE_UPDATE				0x13	//远程升级数据
#define GWVMS_FTYPE_UPDATE_R			0x14	//远程升级数据确认


//报文类型定义
#define GWVMS_PTYPE_KEEPALIVE			0x01	//心跳 0x02 -- 0x03	预留
#define GWVMS_PTYPE_MEASURE_VOL			0x04	//电压数据报
#define GWVMS_PTYPE_MEASURE_DAY			0x05	//日统计数据报 
#define GWVMS_PTYPE_MEASURE_MON			0x06	//月统计数据报 0x07 -- 0xA0 	预留 
#define GWVMS_PTYPE_DATEREQUEST			0xA1	//数据请求 0xA2 -- 0xA3 	预留 
#define GWVMS_PTYPE_TIME				0xA4	//时间查询/设置
#define GWVMS_PTYPE_PARA_COM			0xA6	//装置通信参数查询/设置
#define GWVMS_PTYPE_PARA_MSP			0xA7	//监测点参数查询/设置
#define GWVMS_PTYPE_PARA_EVENT			0xA8	//装置事件参数查询/设置 
#define GWVMS_PTYPE_PARA_CAC			0xA9	//装置所属CAC的信息查询/设置
#define GWVMS_PTYPE_DEVICE_INFO			0xAA	//装置基本信息查询 
#define GWVMS_PTYPE_DEVICE_WORKSTATUS	0xAB	//装置工作状态信息查询
#define GWVMS_PTYPE_COMFLOW				0xAC	//装置通信流量信息查询
#define GWVMS_PTYPE_ORIGINAL_ID			0xAD	//装置ID查询/设置
#define GWVMS_PTYPE_RESET				0xAE	//装置复位
#define GWVMS_PTYPE_FLOWDATA			0xC1	//流量数据 0xC2 -- 0xC3 	预留 
#define GWVMS_PTYPE_EVENT				0xC4	//事件信息报 0xC5 -- 0xC6 	预留 
#define GWVMS_PTYPE_UPDATE_START		0xC7	//升级启动数据报
#define GWVMS_PTYPE_UPDATE				0xC8	//升级过程数据报
#define GWVMS_PTYPE_UPDATE_END			0xC9	//升级结束数据报 0xCA -- 0xCF 	预留 

//Private Typedef
struct gwvms_header
{
	u16	sc;			//0x5AA5
//	u8		sc2;			//
	u16	len;			//数据长度
	u8		adr[GWVMS_ADR_SIZE];
	u8		ftype;			//帧类型
	u8		ptype;			//报文类型
	u8		fseq;			//帧序号
} PACK_STRUCT_STRUCT;



//Internal Functions


//-------------------------------------------------------------------------
//分析报文
//-------------------------------------------------------------------------
static sys_res gwvms_RmsgAnalyze(void *args)
{
	p_gwvms p = (t_gwvms *)args;
	u8 *pTemp;
	u16 nCRC;
	struct gwvms_header *pH;
	p_dlrcp pRcp = &p->parent;

	chl_RecData(pRcp->chl, pRcp->rbuf, OS_TICK_MS);
	
	for (; ; buf_Remove(pRcp->rbuf, 1))
	{
		for (; ; buf_Remove(pRcp->rbuf, 1))
		{
			//不足报文头长度
			if (pRcp->rbuf->len < sizeof(struct gwvms_header))
				return SYS_R_ERR;
			
			pH = (p_gwvms_header)pRcp->rbuf->p;
			if (pH->sc == 0x5AA5)
			{
				if (pH->len > GWVMS_DATA_SIZE)
					continue;
				break;
			}
		}
		
		//不足长度
		if (pRcp->rbuf->len < (sizeof(struct gwvms_header) + pH->len + 2))
			return SYS_R_ERR;
		
		pTemp = pRcp->rbuf->p + sizeof(struct gwvms_header) + pH->len;
		nCRC = (pTemp[0] << 8) | pTemp[1];
		pTemp += 2;
		
		//CRC
		if (crc16(pRcp->rbuf->p + 2, sizeof(struct gwvms_header) + pH->len - 2) != nCRC)
			continue;
		
		//结束符
		if (*pTemp != 0x96)
			continue;
		
		//接收到报文
		p->fseq = pH->fseq;
		p->ftype = pH->ftype;
		p->ptype = pH->ptype;
		
		buf_Release(p->data);
		buf_Push(p->data, pRcp->rbuf->p + sizeof(struct gwvms_header), pH->len);
		buf_Remove(pRcp->rbuf, sizeof(struct gwvms_header) + pH->len + 2);
		
		return SYS_R_OK;
	}
}


//-------------------------------------------------------------------------
//报文头初始化
//-------------------------------------------------------------------------
static void gwvms_TmsgHeaderInit(p_gwvms p, p_gwvms_header pH)
{
	
	pH->sc = 0x5AA5;
 	memcpy(pH->adr, p->adr, GWVMS_ADR_SIZE);
}


//-------------------------------------------------------------------------
//登录、心跳
//-------------------------------------------------------------------------
 static sys_res gwvms_TmsgLinkcheck (void *p, int nCmd)
 {
 	sys_res res;
 	buf b = {0};

 	res = gwvms_TmsgSend(p, 0x01,0x01, b, DLRCP_TMSG_REPORT);//心跳
 	buf_Release(b);
	
 	return res;
 }







//External Functions
//-------------------------------------------------------------------------
//初始化
//-------------------------------------------------------------------------
void gwvms_Init(p_gwvms p)
{

	memset(p, 0, sizeof(t_gwvms));
	dlrcp_Init(&p->parent, gwvms_TmsgLinkcheck, gwvms_RmsgAnalyze);
}

//-------------------------------------------------------------------------
//发送报文
//-------------------------------------------------------------------------
sys_res gwvms_TmsgSend(p_gwvms p, int nFType, int nPType, buf b, int nType)
{
	struct gwvms_header xH;
	u16 nCRC;
	buf bBuf = {0};

	gwvms_TmsgHeaderInit(p, &xH);
	
	if (nType == DLRCP_TMSG_REPORT)
	{
		if (p->parent.pfc == 0)
			p->parent.pfc = 1;
		else
			p->parent.pfc += 1;
		xH.fseq = p->parent.pfc;
	}
	else
	{
		xH.fseq = p->fseq;
	}
	xH.ftype = nFType;
	xH.ptype = nPType;
	xH.len = b->len;
	buf_Push(bBuf,&xH,sizeof(struct gwvms_header));
	buf_Push(bBuf,b->p,b->len);
	
 	nCRC = crc16(bBuf->p+2, bBuf->len-2);
	
	buf_Release(bBuf);
	
	buf_PushData(b, nCRC, 2);
	buf_PushData(b, 0x96, 1);
	
	return dlrcp_TmsgSend(&p->parent, &xH, sizeof(struct gwvms_header), b->p, b->len, nType);
}



//-------------------------------------------------------------------------
//规约处理
//-------------------------------------------------------------------------
sys_res gwvms_Handler(p_gwvms p)
{

	return dlrcp_Handler(&p->parent);
}

#endif


