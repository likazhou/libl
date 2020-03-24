#ifndef __LCP_PLC_H__
#define __LCP_PLC_H__

#ifdef __cplusplus
extern "C" {
#endif


//Public Defines
#define PLC_S_INIT				0
#define PLC_S_SYNC				1
#define PLC_S_IDLE				2
#define PLC_S_WAIT				3
#define PLC_S_SLAVE				4
#define PLC_S_PROBE				5

//Modem Type Defines
#define PLC_T_NULL				0
#define PLC_T_XC_GW				1	//晓程
#define PLC_T_XC_RT				2
#if XCN6N12_ENABLE
#define PLC_T_XC_GD				3
#endif

#define PLC_T_ES_RT				4	//东软
#define PLC_T_ES_38				5

#define PLC_T_TOPCOM			7	//鼎信
#define PLC_T_BOST				8	//博高
#define PLC_T_RISECOM			9	//瑞斯康
#define PLC_T_LEAGUERME			10	//力合微
#define PLC_T_MIARTECH			11	//弥亚微
#define PLC_T_HTSST				12	//航天赛世特


//Public Typedefs
struct plc_def
{
	u8	uartid;
	u8	rst_effect;
	t_gpio_def	rst;
#if PLC_SET_ENABLE
	u8	set_effect;
	t_gpio_def	set;
#endif
} PACK_STRUCT_STRUCT;
typedef const struct plc_def t_plc_def;

struct gw3762_c
{
	u8	fun : 6,
		prm : 1,
		dir : 1;
} PACK_STRUCT_STRUCT;

struct gw3762_rdown
{
	u8	route : 1,
		addit : 1,
		module : 1,
		clash : 1,
		relay : 4;
	u8	chlid : 4,
		eccid : 4;
	u8	acklen;
	u16	rate : 15,
		unit : 1;
	u8	reserve;
} PACK_STRUCT_STRUCT;

struct gw3762_ru
{
	u8	route : 1,
		addit : 1,
		module : 1,
		clash : 1,
		relay : 4;
	u8	chlid : 4,
		eccid : 4;
	u8	pulse : 4,
		chltype : 4;
	u8	cmdq: 4,
		ackq: 4;
	u16	reserve;
} PACK_STRUCT_STRUCT;

struct plc 
{
	u8	ste;
	u8	type;
	u8	mode;
	u8	time;
	u8	inited;
	u8	rstcnt;
#if PLC_PROBE_ENABLE
	u8	probe;
#endif
	u16	tmo;
	u8	adr[6];
	u8	info[9];
	chl	chl;
	buf	rbuf;
	u8	afn;
	struct gw3762_c		c;
	struct gw3762_ru	rup;
	u16	fn;
	u8	madr[6];
	u8	radr[6];
	buf data;
} PACK_STRUCT_STRUCT;
typedef struct plc plc_t;
 


//External Functions
void plc_Init(plc_t *p, const u8 *pAdr);

sys_res plc_RealRead(plc_t *p, buf b, const u8 *pAdr, int nCode, const void *pData, size_t nLen, int nRelay, const u8 *pRtAdr);
sys_res plc_Transmit(plc_t *p, buf b, const void *pData, size_t nLen);
void plc_Broadcast(plc_t *p);
void plc_Heartbeat(plc_t *p, const void *pData, size_t nLen);
sys_res plc_Handler(plc_t *p, buf b, u8 *pAdr);

int plc_GetRetry(plc_t *p);
int plc_GetWait(plc_t *p, int nRelay);
int plc_IsNeedRt(plc_t *p);
int plc_IsNotSync(plc_t *p);
void plc_GetInfo(plc_t *p, char *pInfo);
void plc_ProbeStart(plc_t *p, int nMin);
void plc_ProbeEnd(plc_t *p);


//User Functions
int plc_MeterAdr(int nSn, void *pAdr);
u32 plc_Request(const void *pAdr, int *pIs97);
void plc_NewMeter(const u8 *pAdr);
int plc_IsInTime(void);


#ifdef __cplusplus
}
#endif

#endif



