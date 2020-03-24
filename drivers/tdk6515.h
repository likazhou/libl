#ifndef __TDK6515_H__
#define __TDK6515_H__


#ifdef __cplusplus
extern "C" {
#endif




//Public Typedefs
typedef struct {
	int		ste;
	uart_t *uart;
} t_tdk6515;




// =============================================================
// 从计量IC中读取的原始值,最终会转换成相应的进制
// =============================================================
typedef struct {
    s32    temp;       // 1. 温度                  S32  0.1度
    u32    freq;       // 2. 频率                  U32  0.01HZ
    float  enp[4];     // 3. 合相ABC有功电能       Float
    float  enq[4];     // 7. 合相ABC无功电能       Float
    float  ahour[4];   // 11.ABCN相安培平方小时    Float
    float  vhour[3];   // 15.ABC相伏特平方小时     Float
    u32    caangle;    // 18.CA相电压相间相角      U32     0.1DEC
    u32    cbangle;    // 19.CB相电压相间相角      U32     0.1DEC
    u32    reserve;    // 20
    u32    vover0num;  // 21.电压过零次数          U32
    float  p[4];       // 22.合ABC相有功功率       Float
    float  q[4];       // 26.合ABC相无功功率       Float
    float  ui[4];      // 30.合ABC视在功率         Float
    float  viangle[4]; // 34.合ABC电压电流相角     Float
    float  cos[4];     // 38.合ABC功率因素         Float
    float  i[4];       // 42.ABCN电流有效值        Float
    float  u[3];       // 46.ABC相电压有效值       Float
    float  ptneed;     // 49.有功滑差需量          Float
    float  qtneed;     // 50.无功滑差需量          Float
    float  ppneed;     // 51.有功周期需量          Float
    float  qpneed;     // 52.无功周期需量          Float
    u32    workr;      // 53.计量工作状态寄存器    SW
    u32    powerdir;   // 54.功率方向寄存器        SW
    u32    netstatus;  // 55.电网运行状态字        SW
    u32    connstatus; // 56.电气接线状态字        SW
    u32    adjustsum;  // 57.校表数据校验和寄存器 U32
    u32    lasttx;     // 58.上一次TX发送值寄存器     
    u32    uab;        // 59.AB线电压有效值
    u32    ubc;        // 60.BC线电压有效值
    u32    uac;        // 61.AC线电压有效值
    u32    ppulse;     // 62.有功脉冲计数器
    u32    qpulse;     // 63.无功脉冲计数器
    u32    pulse1;     // 64.远动脉冲计数器1
    u32    pulse2;     // 65.远动脉冲计数器2
    u32    pulse3;     // 66.远动脉冲计数器3
    u32    pulse4;     // 67.远动脉冲计数器4
    u32    pulse5;     // 68.远动脉冲计数器5
    u32    ua1min;     // 69.A相电压一分钟平均值
    u32    ub1min;     // 70.B相电压一分钟平均值
    u32    uc1min;     // 71.C相电压一分钟平均值
} t_tdk6515_rtdata;


// =============================================================
// 从计量IC中读取的原始值,最终转换成了浮点数和整型数
// =============================================================
typedef struct {
    u32		curxbno;        // 72.当前谐波分析通道号
    float   xbrate[21];     // 73.当前通道总2-21次谐波含有率
    float   xbbase;         // 94. 当前谐波通道基本有效值
    u32		xbbasefreq;     // 95. 当前谐波基波频率
} t_tdk6515_xbdata;




//External Functions
void tdk6515_Reset(t_tdk6515 *p);
int tdk6515_IsJLReady(void);
int tdk6515_IsXBReady(void);
void tdk6515_CmdSend(t_tdk6515 *p, int nWrite, u16 nAdr, size_t nLen);


#ifdef __cplusplus
}
#endif

#endif

