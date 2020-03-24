#ifndef __HI_DISP_H__
#define __HI_DISP_H__


#ifdef __cplusplus
extern "C" {
#endif


//Pubilc Defines
#define DISP_SDEEP_MAX		32
#define DISP_IDLE_SIZE		60

#define DISP_KEY_NULL		0	

#define DISP_SEL_NULL		0xFFFFFFFF

//列表类型
#define DISP_LIST_T_MENU	0
#define DISP_LIST_T_DISP	1
#define DISP_LIST_T_SET		2	


	

	
//Public Typedef
typedef const struct
{
	const char	*str;
	u8		type;
	u8		size;
	void *		next;
}t_disp_list;

struct disp
{
	u32		magic;
	u8		ste;		//状态
	u8		key;		//按键
	u8		pw;			//密码深度
	u8		mdeep;		//菜单深度
	u8		sdeep;		//设置深度
	u8		tmo;		//超时时间
	s16		tn;			//表序号	
	u8		hide;		//隐藏菜单
	u8		idx[4];		//索引
	char	str[DISP_SDEEP_MAX];	//设置字符串
	t_disp_list *node;
};
typedef struct disp t_disp;









//External Functions
void disp_ClrDispArea(void);
void disp_SingleData(t_disp *p, int nRow, const char *pItem, const char *pData, int nSel);
void disp_SingleData_Center(t_disp *p, int nRow, const char *pData, int nSel);
void disp_SingleData_SeparateAlign(t_disp *p, int nRow, const char *pItem, const char *pData, int nSel);

void disp_Main(t_disp *p, int nInit);

sys_res disp_InputPassword(t_disp *p, int nSel);
void disp_StatusBar(t_disp *p);
void disp_MainUI(t_disp *p);
void disp_BglOn(void);
void disp_BglOff(void);




#ifdef __cplusplus
}
#endif

#endif


