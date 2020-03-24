#ifndef __ATT7022_H__
#define __ATT7022_H__


#ifdef __cplusplus
extern "C" {
#endif








//------------------------------------------------------------------------
//ATT7022B calibration register define(write only)
//------------------------------------------------------------------------
#define ATT7022_REG_Irgion1			0x02		//相位补偿区域设置1
#define	ATT7022_REG_Irgion2			0x03		//相位补偿区域设置2
#define	ATT7022_REG_Irgion3			0x04		//相位补偿区域设置3
#define	ATT7022_REG_Irgion4			0x05		//相位补偿区域设置4

#define	ATT7022_REG_PgainA0			0x06		//A相功率增益0
#define	ATT7022_REG_PgainB0			0x07		//B相功率增益0
#define	ATT7022_REG_PgainC0			0x08		//C相功率增益0
#define	ATT7022_REG_PgainA1			0x09		//A相功率增益1
#define	ATT7022_REG_PgainB1			0x0a		//B相功率增益1
#define	ATT7022_REG_PgainC1			0x0b		//C相功率增益1

#define	ATT7022_REG_PhsregA0		0x0c		//A相区域0相位校正
#define	ATT7022_REG_PhsregA1		0x0d		//A相区域1相位校正
#define	ATT7022_REG_PhsregA2		0x0e		//A相区域2相位校正
#define	ATT7022_REG_PhsregA3		0x0f		//A相区域3相位校正
#define	ATT7022_REG_PhsregA4		0x10		//A相区域4相位校正

#define	ATT7022_REG_PhsregB0		0x11		//B相区域0相位校正
#define	ATT7022_REG_PhsregB1		0x12		//B相区域1相位校正
#define	ATT7022_REG_PhsregB2		0x13		//B相区域2相位校正
#define	ATT7022_REG_PhsregB3		0x14		//B相区域3相位校正
#define	ATT7022_REG_PhsregB4		0x15		//B相区域4相位校正

#define	ATT7022_REG_PhsregC0		0x16		//C相区域0相位校正
#define	ATT7022_REG_PhsregC1		0x17		//C相区域1相位校正
#define	ATT7022_REG_PhsregC2		0x18		//C相区域2相位校正 
#define	ATT7022_REG_PhsregC3		0x19		//C相区域3相位校正
#define	ATT7022_REG_PhsregC4		0x1a		//C相区域4相位校正

#define	ATT7022_REG_UgainA			0x1b		//A相电压增益设置
#define	ATT7022_REG_UgainB			0x1c		//B相电压增益设置
#define	ATT7022_REG_UgainC			0x1d		//C相电压增益设置

#define	ATT7022_REG_Irechg			0x1e		//比差补偿区域设置
#define	ATT7022_REG_Istartup		0x1f		//启动电流阈值设置
#define	ATT7022_REG_HFConst			0x20		//高频脉冲输出设置

#define	ATT7022_REG_IgainA			0x26		//A相电流增益设置
#define	ATT7022_REG_IgainB			0x27		//B相电流增益设置
#define	ATT7022_REG_IgainC			0x28		//C相电流增益设置

#define	ATT7022_REG_FailVoltage		0x29		//失压阈值设置
#define	ATT7022_REG_EAddMode		0x2a		//合相能量累加模式选择
#define	ATT7022_REG_GainAdc7		0x2b		//第七路ADC有效值校正
#define	ATT7022_REG_GCtrlT7Adc		0x2c		//温度/第七路ADC选择控制
#define	ATT7022_REG_EnlineFreq		0x2d		//基波测量使能控制
#define	ATT7022_REG_EnUAngle		0x2e		//电压夹角测量使能控制
#define	ATT7022_REG_SelectPQSU		0x2f		//基波电压功率输出选择
#define	ATT7022_REG_EnDtIorder		0x30		//电流相序检测使能控制
#define	ATT7022_REG_LineFreqPg		0x31		//基波功率校正
#define	ATT7022_REG_EnHarmonic		0x3C		//基波测量鱼谐波测量切换选择
#define	ATT7022_REG_UADCPga			0x3f		//电压通道ADC增益选择

//------------------------------------------------------------------------
//ATT7022B data register define(read only)
//------------------------------------------------------------------------
#define	ATT7022_REG_PA				0x01		//A相有功功率
#define	ATT7022_REG_PB				0x02		//B相有功功率
#define	ATT7022_REG_PC				0x03		//C相有功功率
#define	ATT7022_REG_PT				0x04		//合相有功功率

#define	ATT7022_REG_QA				0x05		//A相无功功率
#define	ATT7022_REG_QB				0x06		//B相无功功率
#define	ATT7022_REG_QC				0x07		//C相无功功率
#define	ATT7022_REG_QT				0x08		//合相无功功率

#define	ATT7022_REG_SA				0x09		//A相视在功率
#define	ATT7022_REG_SB				0x0a		//B相视在功率
#define	ATT7022_REG_SC				0x0b		//C相视在功率
#define	ATT7022_REG_ST				0x0c		//合相视在功率

#define	ATT7022_REG_URmsA			0x0d		//A相电压有效值
#define	ATT7022_REG_URmsB			0x0e		//B相电压有效值
#define	ATT7022_REG_URmsC			0x0f		//C相电压有效值

#define	ATT7022_REG_IRmsA			0x10		//A相电流有效值
#define	ATT7022_REG_IRmsB			0x11		//B相电流有效值
#define	ATT7022_REG_IRmsC			0x12		//C相电流有效值
#define	ATT7022_REG_IRmsT			0x13		//ABC相电流矢量和的有效值

#define	ATT7022_REG_PfA				0x14		//A相功率因数
#define	ATT7022_REG_PfB				0x15		//B相功率因数
#define	ATT7022_REG_PfC				0x16		//C相功率因数
#define	ATT7022_REG_PfT				0x17		//合相功率因数

#define	ATT7022_REG_PgA				0x18		//A相电流与电压的相角
#define	ATT7022_REG_PgB				0x19		//B相电流与电压的相角		
#define	ATT7022_REG_PgC				0x1a		//C相电流与电压的相角
#define	ATT7022_REG_PgT				0x1b		//合相电流与电压的相角	

#define	ATT7022_REG_Freq			0x1c		//线频率

#define	ATT7022_REG_EpA				0x1e		//A相有功电能
#define	ATT7022_REG_EpB				0x1f		//B相有功电能
#define	ATT7022_REG_EpC				0x20		//C相有功电能
#define	ATT7022_REG_EpT				0x21		//合相有功电能

#define	ATT7022_REG_EqA				0x22		//A相无功电能
#define	ATT7022_REG_EqB				0x23		//B相无功电能
#define	ATT7022_REG_EqC				0x24		//C相无功电能
#define	ATT7022_REG_EqT				0x25		//合相无功电能

#define	ATT7022_REG_RSPIData		0x28		//上次SPI读出的数据
#define	ATT7022_REG_RmsADC7			0x29		//第七路ADC输入信号的有效值
#define	ATT7022_REG_TempD			0x2a		//温度传感器输出

#define	ATT7022_REG_URmst			0x2b		//ABC电压矢量和的有效值
#define ATT7022_REG_gainADC7		0x2b		//ADC7增益寄存器

#define	ATT7022_REG_SFlag			0x2c		//系统状态标志
#define	ATT7022_REG_WSPIData1		0x2d		//上一次写入SPI的数据
#define	ATT7022_REG_WSPIData2		0x2e		//

#define	ATT7022_REG_EFlag			0x30		//电能寄存器的工作状态

#define	ATT7022_REG_EpA2			0x31		//A相有功电能
#define	ATT7022_REG_EpB2			0x32		//B相有功电能
#define	ATT7022_REG_EpC2			0x33		//C相有功电能
#define	ATT7022_REG_EpT2			0x34		//合相有功电能

#define	ATT7022_REG_EqA2			0x35		//A相有功电能
#define	ATT7022_REG_EqB2			0x35		//B相有功电能
#define	ATT7022_REG_EqC2			0x35		//C相有功电能
#define	ATT7022_REG_EqT2			0x35		//合相有功电能


#define	ATT7022_REG_LEFlag			0x3c		//基波电能寄存器工作状态
#define	ATT7022_REG_PFlag			0x3d		//有功和无功功率方向(正向为0, 负向为1)

#define	ATT7022_REG_ChkSum1			0x3e		//校表数据校验寄存器

#define	ATT7022_REG_InstADC7		0x3f		//第7路ADC采样数据输出

#define	ATT7022_REG_PosEpA			0x40		//A相正向有功电能
#define	ATT7022_REG_PosEpB			0x41		//B相正向有功电能
#define	ATT7022_REG_PosEpC			0x42		//C相正向有功电能
#define	ATT7022_REG_PosEpT			0x43		//合相正向有功电能

#define	ATT7022_REG_NegEpA			0x44		//A相负向有功电能
#define	ATT7022_REG_NegEpB			0x45		//B相负向有功电能
#define	ATT7022_REG_NegEpC			0x46		//C相负向有功电能
#define	ATT7022_REG_NegEpT			0x47		//合相负向有功电能

#define	ATT7022_REG_PosEqA			0x48		//A相正向无功电能
#define	ATT7022_REG_PosEqB			0x49		//B相正向无功电能
#define	ATT7022_REG_PosEqC			0x4a		//C相正向无功电能
#define	ATT7022_REG_PosEqT			0x4b		//合相正向无功电能

#define	ATT7022_REG_NegEqA			0x4c		//A相负向无功电能
#define	ATT7022_REG_NegEqB			0x4d		//B相负向无功电能
#define	ATT7022_REG_NegEqC			0x4e		//C相负向无功电能
#define	ATT7022_REG_NegEqT			0x4f		//合相负向无功电能

#define	ATT7022_REG_LineEpA			0x50		//A相基波有功电能
#define	ATT7022_REG_LineEpB			0x51		//A相基波有功电能
#define	ATT7022_REG_LineEpC			0x52		//A相基波有功电能
#define	ATT7022_REG_LineEpT			0x53		//合相基波有功电能

#define	ATT7022_REG_LineEqA			0x54		//A相基波无功电能
#define	ATT7022_REG_LineEqB			0x55		//A相基波无功电能
#define	ATT7022_REG_LineEqC			0x56		//A相基波无功电能
#define	ATT7022_REG_LineEqT			0x57		//合相基波无功电能

#define	ATT7022_REG_YUaUb			0x5c		//Ua与Ub的电压夹角
#define	ATT7022_REG_YUaUc			0x5d		//Ua与Uc的电压夹角	
#define	ATT7022_REG_YUbUc			0x5e		//Ub与Uc的电压夹角
#define	ATT7022_REG_ChkSum2			0x5f		//校表数据校验寄存器

#define	ATT7022_REG_PosEpA2			0x60		//A相正向有功电能(读取一次被清除)
#define	ATT7022_REG_PosEpB2			0x61		//A相正向有功电能(读取一次被清除)
#define	ATT7022_REG_PosEpC2			0x62		//A相正向有功电能(读取一次被清除)
#define	ATT7022_REG_PosEpT2			0x63		//A相正向有功电能(读取一次被清除)

#define	ATT7022_REG_NegEpA2			0x64		//A相负向有功电能(读取一次被清除)
#define	ATT7022_REG_NegEpB2			0x65		//B相负向有功电能(读取一次被清除)
#define	ATT7022_REG_NegEpC2			0x66		//C相负向有功电能(读取一次被清除)
#define	ATT7022_REG_NegEpT2			0x67		//合相负向有功电能(读取一次被清除)

#define	ATT7022_REG_PosEqA2			0x68		//A相正向无功电能(读取一次被清除)
#define	ATT7022_REG_PosEqB2			0x69		//B相正向无功电能(读取一次被清除)
#define	ATT7022_REG_PosEqC2			0x6a		//C相正向无功电能(读取一次被清除)
#define	ATT7022_REG_PosEqT2			0x6b		//合相正向无功电能(读取一次被清除)

#define	ATT7022_REG_NegEqA2			0x6c		//A相负向无功电能(读取一次被清除)
#define	ATT7022_REG_NegEqB2			0x6d		//B相负向无功电能(读取一次被清除)
#define	ATT7022_REG_NegEqC2			0x6e		//C相负向无功电能(读取一次被清除)
#define	ATT7022_REG_NegEqT2			0x6f		//合相负向无功电能(读取一次被清除)

//------------------------------------------------------------------------
//ATT7022B command set
//------------------------------------------------------------------------
#define ATT7022_CALIDATA_CLR		0xC3		//校正数据清除命令
#define ATT7022_CALIDATA_READ		0xC6		//读校正数据命令
#define ATT7022_CALIDATA_WEN		0xC9		//校正数据写使能
#define ATT7022_SOFTRST_CMD			0xD3		//软件复位命令


#define	PHASE_A						0
#define	PHASE_B						1
#define	PHASE_C						2
#define	PHASE_T						3
#define PHASE_ADC7					16

//相位补偿校正区域
#define	PHI_FIELD1					0.1f		//相位补偿校正区域1(100mA以下)
#define	PHI_FIELD2					0.5f		//相位补偿校正区域2(500mA以下)
#define	PHI_FIELD3					1.0f		//相位补偿校正区域3(1A以下)
#define	PHI_FIELD4					2.0f		//相位补偿校正区域4(2A以下)

#define	SYS_KVALUE					8192.0f		// 2 ^ 13 值
#define	CONST_G						0.648f

#define ISTART_RATIO				0.001f				//启动电流比值(Ib * 0.001)
#define IB_VO						0.1f				//Ib时互感器的电压输出信号(0.1V)
#define UCALI_CONST					220.0f				//电压校正常数(220V处校正)
#define ICALI_MUL					1.0f				//电流校正时Ib扩的倍数





//Public Typedefs
typedef struct
{
	u16		ec;
	spi_t *	spi;
}att7022_t;

typedef struct
{
	u32 Res1;
	u32 Res2;
	u32 Iregion1;		//相位补偿区域设置1
	u32 Iregion2;		//相位补偿区域设置2
	u32 Iregion3;		//相位补偿区域设置3
	u32 Iregion4;		//相位补偿区域设置4
	u32 Pgain0[3];		//A相功率增益0
	u32 Pgain1[3];		//A相功率增益1
	u32 PhsregA[5];		//A相区域0相位校正   
	u32 PhsregB[5];		//B相区域0相位校正   
	u32 PhsregC[5];		//C相区域0相位校正   
	u32 Ugain[3];		//A相电压校正
	u32 Irechg;			//比差补偿区域设置
	u32 Istarstup;		//起动电流阀值设置   
	u32 HFConst;			//高频脉冲输出设置
	u32 Res3;
	u32 Res4;
	u32 Res5;
	u32 Res6;
	u32 Res7;
	u32 Igain[3];		//A相电流校正
	u32 FailVoltage;		// 失压阀值设置
	u32 EAddMode;		//合相能量累加模式选择      
	u32 GainAdc7;		//第七路ADC有效值校正    
	u32 GCtrlT7Adc;		//温度/第七路ADC选择控制       
	u32 EnLineFreq;		//基波测量使能控制       
	u32 EnUAngle;		//电压夹角测量使能控制     
	u32 SlectPQSU;		//基波电压功率使能控制     
	u32 ENDtIorder;		//电流相序检测使能控制       
	u32 LineFreqPg;		//基波功率校正        
	u32 Res8;
	u32 Res9;
	u32 Res10;
	u32 Res11;
	u32 Res12;
	u32 Res13;
	u32 Res14;
	u32 Res15;
	u32 Res16;
	u32 Res17;
	u32 EnHarm;			//基波测量与谐波测量切换选择   
	u32 Res18;
	u32 HFDouble;		//脉冲常数加倍选择       
	u32 UADCPga;			//电压通道ADC增益选择       
	u32 nCRC;			//以上数据的校验和
}att7022_cali_t;











//External Functions
void att7022_Init(void);
sys_res att7022_Reset(att7022_cali_t *pCali);
u32 att7022_Read(u32 nReg);
u32 att7022_GetEnergy(int nPhase, int nType);
float att7022_GetFreq(void);
float att7022_GetVoltage(int nPhase);
float att7022_GetCurrent(int nPhase);
float att7022_GetPower(u32 nReg, int nPhase);
u32 att7022_GetFlag(void);
u32 att7022_GetPowerDir(void);
float att7022_GetPFV(int nPhase);
float att7022_GetPAG(int nPhase);
float att7022_GetPVAG(int nPhase);
u32 att7022_GetQuanrant(int nPhase);
sys_res att7022_GetHarmonic(int Ch, s16* pbuf);
int att7022_GetSig(void);
u16 att7022_GetEConst(void);

void att7022_CaliUIP(att7022_cali_t *pCali);
void att7022_CaliPhase(att7022_cali_t *pCali);



#ifdef __cplusplus
}
#endif

#endif

