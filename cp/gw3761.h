#ifndef __GW3761_H__
#define __GW3761_H__


#ifdef __cplusplus
extern "C" {
#endif





//Public Defines
//国网2009通讯规约
#define GW3761_T_GWJC2009			0
//PB325配电通讯规约
#define GW3761_T_PB325				1
//云南电压通讯规约
#define GW3761_T_YUNNANDY			2

//南网负控2005通讯规约
#define GW3761_T_NWFK2005			4
//南网负控2005通讯规约
#define GW3761_T_NWFK2008			5

//国网负控2004通讯规约
#define GW3761_T_GWFK2004			8
//国网负控2005通讯规约
#define GW3761_T_GWFK2005			9
//重庆负控2006通讯规约
#define GW3761_T_CQFK2006			10





#if GW3761_TYPE == GW3761_T_GWJC2009
#define GW3761_PROTOCOL_ID			2
#define GW3761_PW_SIZE				16
#define GW3761_ECREPORT_ENABLE		0
#define GW3761_EC_SIZE				2
#define GW3761_DATA01_WDAY			1
#endif

#if GW3761_TYPE == GW3761_T_PB325
#define GW3761_PROTOCOL_ID			2
#define GW3761_PW_SIZE				16
#define GW3761_ECREPORT_ENABLE		1
#define GW3761_EC_SIZE				2
#define GW3761_DATA01_WDAY			1
#endif

#if GW3761_TYPE == GW3761_T_YUNNANDY
#define GW3761_PROTOCOL_ID			1
#define GW3761_PW_SIZE				16
#define GW3761_ECREPORT_ENABLE		0
#define GW3761_EC_SIZE				8
#define GW3761_DATA01_WDAY			0
#endif

#if GW3761_TYPE == GW3761_T_GWFK2004
#define GW3761_PROTOCOL_ID			1
#define GW3761_PW_SIZE				2
#define GW3761_ECREPORT_ENABLE		0
#define GW3761_EC_SIZE				2
#define GW3761_DATA01_WDAY			1
#endif

#if GW3761_TYPE == GW3761_T_GWFK2005
#define GW3761_PROTOCOL_ID			1
#define GW3761_PW_SIZE				16
#define GW3761_ECREPORT_ENABLE		0
#define GW3761_EC_SIZE				2
#define GW3761_DATA01_WDAY			1
#endif

#if GW3761_TYPE == GW3761_T_CQFK2006
#define GW3761_PROTOCOL_ID			1
#define GW3761_PW_SIZE				2
#define GW3761_ECREPORT_ENABLE		0
#define GW3761_EC_SIZE				2
#define GW3761_DATA01_WDAY			1
#endif

#if GW3761_TYPE == GW3761_T_NWFK2005
#define GW3761_PROTOCOL_ID			1
#define GW3761_PW_SIZE				2
#define GW3761_ECREPORT_ENABLE		0
#define GW3761_EC_SIZE				2
#define GW3761_DATA01_WDAY			1
#endif

#if GW3761_TYPE == GW3761_T_NWFK2008
#define GW3761_PROTOCOL_ID			1
#define GW3761_PW_SIZE				16
#define GW3761_ECREPORT_ENABLE		0
#define GW3761_EC_SIZE				2
#define GW3761_DATA01_WDAY			1
#endif




//传送方向定义
#define GW3761_DIR_RECV             0   //主站发出
#define GW3761_DIR_SEND             1   //终端发出

//AFN
#define GW3761_AFN_CONFIRM			0x00
#define GW3761_AFN_LINKCHECK		0x02


//FUN
#define GW3761_FUN_RESET			1
#define GW3761_FUN_DATA_USER		4
#define GW3761_FUN_LINKCHECK		9
#define GW3761_FUN_DATA_L1			10
#define GW3761_FUN_DATA_L2			11

#define GW3761_FUN_CONFIRM			0
#define GW3761_FUN_RESPONSE			8
#define GW3761_FUN_NODATA			9
#define GW3761_FUN_LINKSTATE		11

//无效数据值
#define GW3761_DATA_INVALID			0xEEEEEEEE

//信息类处理类型定义
#define GW3761_DT_T_DATA_CURRENT	0x01
#define GW3761_DT_T_DATA_HOUR		0x05
#define GW3761_DT_T_DATA_CURVE		0x0C
#define GW3761_DT_T_DATA_DAY		0x0D
#define GW3761_DT_T_DATA_MDAY		0x0E
#define GW3761_DT_T_DATA_MONTH		0x0F

#define GW3761_DT_T_PARAM_TER		0x10
#define GW3761_DT_T_PARAM_METER		0x11
#define GW3761_DT_T_PARAM_PULSE		0x12
#define GW3761_DT_T_PARAM_ANA		0x13
#define GW3761_DT_T_PARAM_GROUP		0x14
#define GW3761_DT_T_PARAM_DIFF		0x15
#define GW3761_DT_T_PARAM_FREEZE	0x16
#define GW3761_DT_T_PARAM_PORT		0x17

//信息点类型定义
#define GW3761_DA_T_TER				0
#define GW3761_DA_T_TN				1
#define GW3761_DA_T_GROUP			2
#define GW3761_DA_T_ANA				3

//规约数据类型定义
#define GW3761_DATA_T_01			1
#define GW3761_DATA_T_02			2
#define GW3761_DATA_T_03			3
#define GW3761_DATA_T_04			4
#define GW3761_DATA_T_05			5
#define GW3761_DATA_T_06			6
#define GW3761_DATA_T_07			7
#define GW3761_DATA_T_08			8
#define GW3761_DATA_T_09			9
#define GW3761_DATA_T_10			10
#define GW3761_DATA_T_11			11
#define GW3761_DATA_T_12			12
#define GW3761_DATA_T_13			13
#define GW3761_DATA_T_14			14
#define GW3761_DATA_T_15			15
#define GW3761_DATA_T_16			16
#define GW3761_DATA_T_17			17
#define GW3761_DATA_T_18			18
#define GW3761_DATA_T_19			19
#define GW3761_DATA_T_20			20
#define GW3761_DATA_T_21			21
#define GW3761_DATA_T_22			22
#define GW3761_DATA_T_23			23
#define GW3761_DATA_T_24			24
#define GW3761_DATA_T_25			25
#define GW3761_DATA_T_26			26
#define GW3761_DATA_T_27			27

#define GW3761_DATA_T_BIN			40
#define GW3761_DATA_T_TIME			41
#define GW3761_DATA_T_PERCENT		42
#define GW3761_DATA_T_OTHER			43
#define GW3761_DATA_T_CONTROL		44
#define GW3761_DATA_T_CONST			45
#define GW3761_DATA_T_RDONLY		46
#define GW3761_DATA_T_RATE			47
#define GW3761_DATA_T_DYNAMIC		60
#define GW3761_DATA_T_INDEX			61
#define GW3761_DATA_T_REPEAT		62
#define GW3761_DATA_T_FREEZEPARAM	63

#define GW3761_DATA_T_DIVIDE		0x40
#define GW3761_DATA_T_MULTI			0x80



//Public Typedefs
//帧控制域
struct gw3761_c
{
	u8	fun : 4,
		fcv : 1,
		fcb_acd : 1,
		prm : 1,
		dir : 1;
} PACK_STRUCT_STRUCT;

//帧序列域
struct gw3761_seq
{
	u8	seq : 4,
		con : 1,
		fin : 1,
		fir : 1,
		tpv : 1;
} PACK_STRUCT_STRUCT;

//时间标签域
struct gw3761_tp
{
	u8	pfc;
	u8	tp[4];
	u8	dly;
} PACK_STRUCT_STRUCT;

//GDW3761接收帧
struct gw3761_rmsg
{
 	u16	a1;			//地市区县码
 	u16	a2;			//终端地址
	u8	group : 1,
		msa : 7;
	struct gw3761_c		c;
	u8	afn;
	struct gw3761_seq	seq;
	struct gw3761_tp	tp;
	u8	pw[GW3761_PW_SIZE];
	buf	data;
} PACK_STRUCT_STRUCT;

//GDW3761规约结构
typedef struct
{
	struct dlrcp		parent;
 	u16	rtua;
 	u16	terid;
	struct gw3761_rmsg	rmsg;
} gw3761_t;



//GDW3761数据结构
typedef const struct
{
	u8	type; 		//数据类型
	u8	size; 		//该信息类元数据大小
	u16	para; 		//附加参数
	u16	reg; 		//对应寄存器编号
} t_gw3761_item, *p_gw3761_item;

typedef const struct
{
	u8	dttype : 5, //信息类处理类型
		datype : 3;	//信息点类型
	u8	qty; 		//数据项数量
	u16	dtid;		//信息类组
	p_gw3761_item item; 	//对应DT1表的地址
} t_gw3761_dt, *p_gw3761_dt;



//External Functions
void gw3761_Init(gw3761_t *p);
sys_res gw3761_Transmit(gw3761_t *p, gw3761_t *pD);
int gw3761_RecvCheck(gw3761_t *p);
sys_res gw3761_Handler(gw3761_t *p);

void gw3761_Response(gw3761_t *p);

sys_res gw3761_TmsgSend(gw3761_t *p, int nFun, int nAfn, buf b, int nType);
sys_res gw3761_TmsgConfirm(gw3761_t *p);
sys_res gw3761_TmsgReject(gw3761_t *p);

u16 gw3761_ConvertDa2DA(int nDa);
u16 gw3761_ConvertFn2DT(int nFN);
int gw3761_ConvertDt2Fn(u16 nDT);
int gw3761_ConvertDA2Map(u16 nDA, void *pData);
void gw3761_ConvertData(void *p, u32 nData, int nDec, u32 nMark, int nSignBit, size_t nSize, int nSign);
void gw3761_ConvertData_Time(u8 *p, time_t tTime, int nType);
void gw3761_ConvertData_01(void *p, time_t tTime);
void gw3761_ConvertData_03(void *p, u32 nData, int nSign);
void gw3761_ConvertData_05(void *p, u32 nData, int nSign);
void gw3761_ConvertData_05_Percent(void *p, u32 nData, int nSign);
void gw3761_ConvertData_06(void *p, u32 nData, int nSign);
void gw3761_ConvertData_07(void *p, u32 nData);
void gw3761_ConvertData_09(void *p, u32 nData, int nSign);
void gw3761_ConvertData_11(void *p, u32 nData);
void gw3761_ConvertData_13(void *p, u32 nData);
void gw3761_ConvertData_14(u8 *p, float nData);
void gw3761_ConvertData_15(void *p, time_t tTime);
void gw3761_ConvertData_17(void *p, time_t tTime);
void gw3761_ConvertData_18(void *p, time_t tTime);
void gw3761_ConvertData_22(void *p, u32 nData);
void gw3761_ConvertData_23(void *p, u32 nData);
void gw3761_ConvertData_25(void *p, u32 nData, int nSign);
void gw3761_ConvertData_26(void *p, u32 nData);

u64 gw3761_EvtCount(void);




#ifdef __cplusplus
}
#endif


#endif


