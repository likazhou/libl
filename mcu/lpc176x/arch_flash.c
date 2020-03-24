#if INTFLASH_ENABLE


//Private Typedefs
typedef unsigned char  	BOOLEAN;        //  布尔变量                  
typedef unsigned char  	INT8U;          //  无符号8位整型变量         
typedef signed   char  	INT8S;          //  有符号8位整型变量         
typedef unsigned short 	INT16U;         //  无符号16位整型变量        
typedef signed   short 	INT16S;         //  有符号16位整型变量        
typedef unsigned long  	INT32U;         //  无符号32位整型变量        
typedef signed   long  	INT32S;         //  有符号32位整型变量        
typedef float          	FP32;           //  单精度浮点数（32位长度）   
typedef double         	FP64;           //  双精度浮点数（64位长度）   



/* 
 *  定义CCLK值大小，单位为KHz 
 */
#define IAP_FCCLK            (MCU_CLOCK / 1000)
#define IAP_ENTER_ADR        0x1FFF1FF1                                  /* IAP入口地址定义              */

/* 
 *  定义IAP命令字
 */                                     
#define IAP_Prepare                                50               /* 选择扇区                     */
                                                                        /* 【起始扇区号、结束扇区号】   */                    
#define IAP_RAMTOFLASH                             51               /* 拷贝数据 FLASH目标地址       */
                                                                        /* RAM源地址    【、写入字节数  */
                                                                        /* 系统时钟频率】               */
#define IAP_ERASESECTOR                            52               /* 擦除扇区    【起始扇区号     */
                                                                        /* 结束扇区号、系统时钟频率】   */
#define IAP_BLANKCHK                               53               /* 查空扇区    【起始扇区号、   */
                                                                        /* 结束扇区号】                 */
#define IAP_READPARTID                             54               /* 读器件ID    【无】           */
#define IAP_BOOTCODEID                             55               /* 读Boot版本号【无】           */
#define IAP_COMPARE                                56               /* 比较命令    【Flash起始地址  */
                                                                        /* RAM起始地址、需要比较的      */
                                                                        /* 字节数】                     */

/*
 *  定义函数指针  
 */
typedef INT32U (*PIAP_Entry)(INT32U param_tab[], INT32U result_tab[]);

PIAP_Entry IAP_Entry = (PIAP_Entry)IAP_ENTER_ADR;

INT32U paramin[8];                                                     /* IAP入口参数缓冲区            */
INT32U paramout[8];                                                    /* IAP出口参数缓冲区            */

/*********************************************************************************************************
** Function name:       sectorPrepare
** Descriptions:        IAP操作扇区选择，命令代码50
** input parameters:    sec1:           起始扇区
**                      sec2:           终止扇区
** output parameters:   paramout[0]:    IAP操作状态码,IAP返回值     
** Returned value:      paramout[0]:    IAP操作状态码,IAP返回值                     
*********************************************************************************************************/
static INT32U sectorPrepare(INT8U sec1, INT8U sec2)
{  
    paramin[0] = IAP_Prepare;                                           /* 设置命令字                   */
    paramin[1] = sec1;                                                  /* 设置参数                     */
    paramin[2] = sec2;                            
    IAP_Entry(paramin, paramout);                                    /* 调用IAP服务程序              */
   
    return (paramout[0]);                                               /* 返回状态码                   */
}

/*********************************************************************************************************
** Function name:       ramCopy
** Descriptions:        复制RAM的数据到FLASH，命令代码51
** input parameters:    dst:            目标地址，即FLASH起始地址。以512字节为分界
**                      src:            源地址，即RAM地址。地址必须字对齐
**                      no:             复制字节个数，为512/1024/4096/8192
** output parameters:   paramout[0]:    IAP操作状态码,IAP返回值     
** Returned value:      paramout[0]:    IAP操作状态码,IAP返回值                     
*********************************************************************************************************/
static INT32U ramToFlash(INT32U dst, INT32U src, INT32U no)
{  
    paramin[0] = IAP_RAMTOFLASH;                                        /* 设置命令字                   */
    paramin[1] = dst;                                                   /* 设置参数                     */
    paramin[2] = src;
    paramin[3] = no;
    paramin[4] = IAP_FCCLK;
    IAP_Entry(paramin, paramout);                                    /* 调用IAP服务程序              */
    
    return (paramout[0]);                                               /* 返回状态码                   */
}

/*********************************************************************************************************
** Function name:       sectorErase
** Descriptions:        扇区擦除，命令代码52
** input parameters:    sec1            起始扇区
**                      sec2            终止扇区92
** output parameters:   paramout[0]:    IAP操作状态码,IAP返回值
** Returned value:      paramout[0]:    IAP操作状态码,IAP返回值                     
*********************************************************************************************************/
static INT32U sectorErase(INT8U sec1, INT8U sec2)
{  
    paramin[0] = IAP_ERASESECTOR;                                       /* 设置命令字                   */
    paramin[1] = sec1;                                                  /* 设置参数                     */
    paramin[2] = sec2;
    paramin[3] = IAP_FCCLK;
    IAP_Entry(paramin, paramout);                                    /* 调用IAP服务程序              */
   
    return (paramout[0]);                                               /* 返回状态码                   */
}


void arch_IntfInit()
{

}

sys_res arch_IntfErase(adr_t adr)
{
	uint nBlk;

	nBlk = adr / INTFLASH_BLK_SIZE;
	os_interrupt_Disable();
#if WDG_ENABLE
	wdg_Reload(0);
#endif
	sectorPrepare(nBlk, nBlk);
#if WDG_ENABLE
	wdg_Reload(0);
#endif
	sectorErase(nBlk, nBlk);
#if WDG_ENABLE
	wdg_Reload(0);
#endif
	os_interrupt_Enable();
	return SYS_R_OK;
}

sys_res arch_IntfProgram(adr_t adr, const void *pData, uint nLen)
{
	uint nBlk;

	nBlk = adr / INTFLASH_BLK_SIZE;
	os_interrupt_Disable();
#if WDG_ENABLE
	wdg_Reload(0);
#endif
	sectorPrepare(nBlk, nBlk);
#if WDG_ENABLE
	wdg_Reload(0);
#endif
	ramToFlash(adr, (INT32U)pData, nLen);
#if WDG_ENABLE
	wdg_Reload(0);
#endif
	os_interrupt_Enable();
	return SYS_R_OK;
}


#endif


