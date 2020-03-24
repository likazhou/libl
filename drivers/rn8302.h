#ifndef __RN8302_H__
#define __RN8302_H__


#ifdef __cplusplus
extern "C" {
#endif





//Public Defines
#define	RN8302_REG_UA			0x0007
#define	RN8302_REG_UB			0x0008
#define	RN8302_REG_UC			0x0009
#define	RN8302_REG_USUM			0x000A
#define	RN8302_REG_IA			0x000B
#define	RN8302_REG_IB			0x000C
#define	RN8302_REG_IC			0x000D
#define	RN8302_REG_IN			0x000E

#define	RN8302_REG_ISUM			0x0010

#define	RN8302_REG_PA			0x0014
#define	RN8302_REG_PB			0x0015
#define	RN8302_REG_PC			0x0016
#define	RN8302_REG_PT			0x0017
#define	RN8302_REG_QA			0x0018
#define	RN8302_REG_QB			0x0019
#define	RN8302_REG_QC			0x001A
#define	RN8302_REG_QT			0x001B
#define	RN8302_REG_SA			0x001C
#define	RN8302_REG_SB			0x001D
#define	RN8302_REG_SC			0x001E
#define	RN8302_REG_STA			0x001F

#define RN8302_REG_PfA			0x0020
#define RN8302_REG_PfB			0x0021
#define RN8302_REG_PfC			0x0022
#define RN8302_REG_PfTA			0x0023

#define	RN8302_REG_EPA			0x0030
#define	RN8302_REG_EPB			0x0031
#define	RN8302_REG_EPC			0x0032
#define	RN8302_REG_EPT			0x0033
#define	RN8302_REG_PosEPA		0x0034
#define	RN8302_REG_PosEPB		0x0035
#define	RN8302_REG_PosEPC		0x0036
#define	RN8302_REG_PosEPT		0x0037
#define	RN8302_REG_NegEPA		0x0038
#define	RN8302_REG_NegEPB		0x0039
#define	RN8302_REG_NegEPC		0x003A
#define	RN8302_REG_NegEPT		0x003B

#define	RN8302_REG_EQA			0x003C
#define	RN8302_REG_EQB			0x003D
#define	RN8302_REG_EQC			0x003E
#define	RN8302_REG_EQT			0x003F
#define	RN8302_REG_PosEQA		0x0040
#define	RN8302_REG_PosEQB		0x0041
#define	RN8302_REG_PosEQC		0x0042
#define	RN8302_REG_PosEQT		0x0043
#define	RN8302_REG_NegEQA		0x0044
#define	RN8302_REG_NegEQB		0x0045
#define	RN8302_REG_NegEQC		0x0046
#define	RN8302_REG_NegEQT		0x0047

#define	RN8302_REG_ESA			0x0048
#define	RN8302_REG_ESB			0x0049
#define	RN8302_REG_ESC			0x004A
#define	RN8302_REG_EST			0x004B

#define RN8302_REG_YUA			0x0050
#define RN8302_REG_YUB			0x0051
#define RN8302_REG_YUC			0x0052
#define RN8302_REG_YIA			0x0053
#define RN8302_REG_YIB			0x0054
#define RN8302_REG_YIC			0x0055
#define RN8302_REG_YIN			0x0056
#define RN8302_REG_UFreq		0x0057

#define	RN8302_REG_FUA			0x0058
#define	RN8302_REG_FUB			0x0059
#define	RN8302_REG_FUC			0x005A
#define	RN8302_REG_FIA			0x005B
#define	RN8302_REG_FIB			0x005C
#define	RN8302_REG_FIC			0x005D

#define	RN8302_REG_FEPA			0x007A
#define	RN8302_REG_FEPB			0x007B
#define	RN8302_REG_FEPC			0x007C
#define	RN8302_REG_FEPT			0x007D
#define	RN8302_REG_PosFEPA		0x007E
#define	RN8302_REG_PosFEPB		0x007F
#define	RN8302_REG_PosFEPC		0x0080
#define	RN8302_REG_PosFEPT		0x0081
#define	RN8302_REG_NegFEPA		0x0082
#define	RN8302_REG_NegFEPB		0x0083
#define	RN8302_REG_NegFEPC		0x0084
#define	RN8302_REG_NegFEPT		0x0085

#define	RN8302_REG_FEQA			0x0086
#define	RN8302_REG_FEQB			0x0087
#define	RN8302_REG_FEQC			0x0088
#define	RN8302_REG_FEQT			0x0089
#define	RN8302_REG_PosFEQA		0x008A
#define	RN8302_REG_PosFEQB		0x008B
#define	RN8302_REG_PosFEQC		0x008C
#define	RN8302_REG_PosFEQT		0x008D
#define	RN8302_REG_NegFEQA		0x008E
#define	RN8302_REG_NegFEQB		0x008F
#define	RN8302_REG_NegFEQC		0x0090
#define	RN8302_REG_NegFEQT		0x0091

#define	RN8302_REG_FESA			0x0092
#define	RN8302_REG_FESB			0x0093
#define	RN8302_REG_FESC			0x0094
#define	RN8302_REG_FEST			0x0095

#define	RN8302_REG_HUA			0x0096
#define	RN8302_REG_HUB			0x0097
#define	RN8302_REG_HUC			0x0098
#define	RN8302_REG_HIA			0x0099
#define	RN8302_REG_HIB			0x009A
#define	RN8302_REG_HIC			0x009B

#define	RN8302_REG_HFCONST1		0x1000
#define	RN8302_REG_HFCONST2		0x1001
#define	RN8302_REG_IStart_PS	0x1002
#define	RN8302_REG_ZXOT			0x1005
#define	RN8302_REG_PRTH1L		0x1006
#define	RN8302_REG_PRTH1H		0x1007
#define	RN8302_REG_PRTH2L		0x1008
#define	RN8302_REG_PRTH2H		0x1009
#define	RN8302_REG_PHSUA		0x100C
#define	RN8302_REG_PHSUB		0x100D
#define	RN8302_REG_PHSUC		0x100E
#define	RN8302_REG_PHSIA		0x100F
#define	RN8302_REG_PHSIB		0x1010
#define	RN8302_REG_PHSIC		0x1011
#define	RN8302_REG_GSUA			0x1013
#define	RN8302_REG_GSUB			0x1014
#define	RN8302_REG_GSUC			0x1015
#define	RN8302_REG_GSIA			0x1016
#define	RN8302_REG_GSIB			0x1017
#define	RN8302_REG_GSIC			0x1018
#define	RN8302_REG_GSIN			0x1019
#define	RN8302_REG_DCOS_UA		0x101A
#define	RN8302_REG_DCOS_UB		0x101B
#define	RN8302_REG_DCOS_UC		0x101C
#define	RN8302_REG_GPA			0x1028
#define	RN8302_REG_GPB			0x1029
#define	RN8302_REG_GPC			0x102A
#define	RN8302_REG_GQA			0x102B
#define	RN8302_REG_GQB			0x102C
#define	RN8302_REG_GQC			0x102D
#define	RN8302_REG_GSA			0x102E
#define	RN8302_REG_GSB			0x102F
#define	RN8302_REG_GSC			0x1030
#define	RN8302_REG_PA_PHS		0x1031
#define	RN8302_REG_PB_PHS		0x1032
#define	RN8302_REG_PC_PHS		0x1033
#define	RN8302_REG_QA_PHS		0x1034
#define	RN8302_REG_QB_PHS		0x1035
#define	RN8302_REG_QC_PHS		0x1036
#define	RN8302_REG_PA_OS		0x1037
#define	RN8302_REG_PB_OS		0x1038
#define	RN8302_REG_PC_OS		0x1039

#define RN8302_REG_GFPA			0x1043
#define RN8302_REG_GFPB			0x1044
#define	RN8302_REG_GFPC			0x1045

#define	RN8302_REG_GFSA			0x1049
#define	RN8302_REG_GFSB			0x104A
#define	RN8302_REG_GFSC			0x104B

#define	RN8302_REG_CFCFG		0x1060
#define RN8302_REG_EMUCON		0x1062
#define	RN8302_REG_PQSign		0x1066
#define	RN8302_REG_CheckSum1	0x106A
#define RN8302_REG_WDISABLE		0x1080
#define	RN8302_REG_WMSW			0x1081
#define	RN8302_REG_SOFTRST		0x1082
#define	RN8302_REG_SYSSR		0x108A
#define	RN8302_REG_RData		0x108C
#define	RN8302_REG_WData		0x108D
#define	RN8302_REG_DeviceID		0x108F




//Public Typedefs
typedef struct {
	u32		crc;
	u16		ec;
	float	kv;
	float	ki;
	float	kp;
	spi_t *	spi;
}rn8302_t;

typedef struct {
	u32		mode;
	u16		HFConst;		//高频脉冲输出设置
	u16		EC;				//脉冲常数
	float	kv;
	float	ki;
	float	kp;
	u32		crc;
	s16		Ugain[3];		//电压校正
	s16		Igain[3];		//电流校正
	s8		UPhs[3];		//电压相角
	s8		IPhs[3][3];		//电流相角
}rn8302_cali_t;







//External Functions
void rn8302_Init(void);
sys_res rn8302_Reset(rn8302_cali_t *pCali);
int rn8302_Read(u16 nReg, void *pData, size_t nLen);
int rn8302_Write(u16 nReg, const void *pData, size_t nLen);
int rn8302_CheckSum(void);
u32 rn8302_GetEnergy(int nPhase, int nType);
float rn8302_GetFreq(void);
float rn8302_GetUI(int nPhase, int nType);
float rn8302_GetPower(int nPhase, int nType);
float rn8302_GetPFV(int nPhase);
float rn8302_GetPAG(int nPhase, int nType) ;
u16 rn8302_GetPowerDir(void);
void rn8302_Cali(rn8302_cali_t *pCali, float fUn, float fIb, int nIs3P3);
int rn8302_GetEConst(void);

#if 0

int rn8302_GetFlag(void);
int rn8302_GetQuanrant(int nPhase);
sys_res rn8302_GetHarmonic(int Ch, s16* pbuf);
int rn8302_GetSig(void);

void rn8302_CaliUIP(p_rn8302_cali pCali);
void rn8302_CaliPhase(p_rn8302_cali pCali);

#endif

#ifdef __cplusplus
}
#endif

#endif

