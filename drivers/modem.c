#if MODEM_ENABLE
#include <drivers/modem.h>

//Private Defines
#define MODEM_CMD_DIAL_GPRS			"ATD*99***1#\r\n"
#define MODEM_CMD_DIAL_CDMA			"ATDT#777\r\n"


//Private Macros
#if MODEM_DEBUG_ENABLE
#define MODEM_DBGOUT				dbg_printf
#else
#define MODEM_DBGOUT(...)
#endif


/* Modem Operation */
#if MODEM_DTR_ENABLE
#define modem_Dtr(x)				sys_GpioSet(&(tbl_bspModem[0])->dtr, x)
#endif
#if MODEM_DCD_ENABLE
#define modem_Dcd()					sys_GpioGet(&(tbl_bspModem[0])->dcd)
#endif

//Private Variables
static t_modem gsm_xModem;


static void modem_Act(int nHL)
{
	t_modem_def *p = &tbl_bspModem;

	if (p->act_effect == GPIO_EFFECT_HIGH)
		nHL ^= 1;
	
	sys_GpioSet(&p->act, nHL);
}

#if MODEM_PWR_ENABLE
static void modem_Pwr(int nHL)
{
	t_modem_def *p = &tbl_bspModem;

	if (p->pwr_effect == GPIO_EFFECT_HIGH)
		nHL ^= 1;
	
	sys_GpioSet(&p->power, nHL);
}
#endif
#if MODEM_RST_ENABLE
static void modem_Rst(int nHL)
{
	t_modem_def *p = &tbl_bspModem;

	if (p->rst_effect == GPIO_EFFECT_HIGH)
		nHL ^= 1;
	
	sys_GpioSet(&p->rst, nHL);
}
#endif





static sys_res modem_SendCmd(p_modem p, const char *pCmd, const char *pRes, int nRetry)
{
	size_t nTmo;

	for (; nRetry; nRetry--)
	{
		buf_Release(p->rbuf);

		uart_SendStr(p->uart, "AT");
		uart_SendStr(p->uart, pCmd);
		uart_SendStr(p->uart, STRING_0D0A);

		os_thd_sleep(100);
		for (nTmo = 900 / OS_TICK_MS; nTmo; nTmo--)
		{
			if (uart_RecTmo(p->uart, p->rbuf, OS_TICK_MS) != SYS_R_OK)
				continue;
			
			if (buffstr(p->rbuf, pRes) != NULL)
			{
				MODEM_DBGOUT("<Modem> %s OK", pCmd);
				return SYS_R_OK;
			}
		}
	}
	
	MODEM_DBGOUT("<Modem> %s ERR", pCmd);
	
	return SYS_R_TMO;
}

static sys_res modem_InitCmd(p_modem p)
{
	int i, nTemp;
	char *pTemp, str[64];
	
	uart_Config(p->uart, MODEM_UART_BAUD, UART_PARI_NO, UART_DATA_8D, UART_STOP_1D);

#if MODEM_BAUD_ADJUST
	if (modem_SendCmd(p, "Z0", "OK\r", 30) != SYS_R_OK)
	{
		uart_Config(p->uart, MODEM_BAUD_ADJUST, UART_PARI_NO, UART_DATA_8D, UART_STOP_1D);
		if (modem_SendCmd(p, "Z0", "OK\r", 10) != SYS_R_OK)
			return SYS_R_TMO;
		
		sprintf(str, "+TCPR=%d", MODEM_UART_BAUD);
		if (modem_SendCmd(p, str, "OK\r", 5) != SYS_R_OK)
			return SYS_R_TMO;
		
		uart_Config(p->uart, MODEM_UART_BAUD, UART_PARI_NO, UART_DATA_8D, UART_STOP_1D);
	}
#else
	if (modem_SendCmd(p, "Z0", "OK\r", 30) != SYS_R_OK)
		return SYS_R_TMO;
	
#endif
	if (modem_SendCmd(p, "E0", "OK\r", 4) != SYS_R_OK)
		return SYS_R_TMO;
	
	//模块信息
	p->ver[0] = '\0';
	if (modem_SendCmd(p, "+CGMR", "OK\r", 1) == SYS_R_OK)
	{
		buf_Remove(p->rbuf, 2);
		
		if ((pTemp = buffstr(p->rbuf, ":")) != NULL)
			buf_Remove(p->rbuf, (u8 *)pTemp - p->rbuf->p);
		
		if ((pTemp = buffstr(p->rbuf, STRING_0D0A)) != NULL)
			*(pTemp - 2) = '\0';
		
		memcpy(p->ver, p->rbuf->p, 20);
	}
	
	//CPIN检测卡
	if (modem_SendCmd(p, "+CPIN?", "OK\r", 30) != SYS_R_OK)
		return SYS_R_TMO;
	
	//SIM卡串号
	p->ccid[0] = 0;
	for (i = 0; i < 3; i++)
	{
		if (modem_SendCmd(p, "+CCID", "OK\r", 1) != SYS_R_OK)
		{
			if (modem_SendCmd(p, "+ZGETICCID", "OK\r", 1) != SYS_R_OK)
				continue;
		}
		if ((pTemp = buffstr(p->rbuf, "CCID: ")) != NULL)
		{
			memcpy(p->ccid, pTemp, 20);
			break;
		}
		if ((pTemp = buffstr(p->rbuf, "CCID:")) != NULL)
		{
			memcpy(p->ccid, pTemp, 20);
			break;
		}
	}
	
	//IMEI号
	p->imei[0] = 0;
	for (i = 0; i < 3; i++)
	{
		if (modem_SendCmd(p, "+GSN", "OK\r", 1) != SYS_R_OK)
		{
			if (modem_SendCmd(p, "+CGSN", "OK\r", 1) != SYS_R_OK)
				continue;
		}
		if ((pTemp = buffstr(p->rbuf, STRING_0D0A)) != NULL)
		{
			memcpy(p->imei, pTemp, 15);
			break;
		}
	}

	//注册网络
	for (i = 0; i < 99; i++)
	{
 		if (modem_SendCmd(p, "+CREG?", "OK\r", 1) != SYS_R_OK)
			continue;
		
		if ((pTemp = buffstr(p->rbuf, ",")) == NULL)
			continue;
		
		nTemp = atoi(pTemp);
		if ((nTemp == 1) || (nTemp == 5))
			break;
		
		os_thd_sleep(1000);
	}
	if (i >= 99)
		return SYS_R_TMO;
	
	//获得信号强度
	for (i = 0; i < 20; i++)
	{
		p->signal = 0;
 		if (modem_SendCmd(p, "+CSQ", "OK\r", 1) != SYS_R_OK)
			continue;
		
		if ((pTemp = buffstr(p->rbuf, ",")) == NULL)
			continue;
		
		p->signal = atoi(pTemp - 3);
		if (p->signal && (p->signal < 32))
			break;
		
		os_thd_sleep(1000);
	}
	if (p->signal == 0)
		return SYS_R_ERR;
	
	if (modem_SendCmd(p, "+CIMI", "OK\r", 10) != SYS_R_OK)
		return SYS_R_TMO;
	
	//获得网络
	if ((pTemp = buffstr(p->rbuf, "460")) == NULL)
		return SYS_R_ERR;
	
	if (pTemp[1] == '3')
		p->type = MODEM_TYPE_CDMA;
	else
		p->type = MODEM_TYPE_GPRS;

#if MODEM_SMS_ENABLE
	//设置短信存储在SIM卡
	if (modem_SendCmd(p, "+CPMS=\"SM\",\"SM\",\"SM\"", "OK\r", 5) == SYS_R_OK)
	{
		//PDU模式
		if (modem_SendCmd(p, "+CMGF=0", "OK\r", 5) == SYS_R_OK)
		{
			//请求短消息中心号码
			if (modem_SendCmd(p, "+CSCA?", "OK\r", 5) == SYS_R_OK)
			{
				pTemp = buffstr(p->rbuf, "+86");
				if (pTemp != NULL)
				{
					memcpy(p->SCA, pTemp, 13);
					p->SCA[13] = 'F';
				}
			}
		}
	}
#endif

#if MODEM_TCP_ENABLE
	if (p->flag)
	{
		sprintf(str, "$MYNETCON=0,\"APN\",\"%s\"", p->apn);
		if (modem_SendCmd(p, str, "OK\r", 10) == SYS_R_OK)
		{
			sprintf(str, "$MYNETCON=0,\"USERPWD\",\"%s,%s\"", p->user, p->pwd);
			if (modem_SendCmd(p, str, "OK\r", 10) == SYS_R_OK)
			{
				p->mtcp = 1;
				return SYS_R_OK;
			}
		}
	}
#endif

	switch (p->type)
	{
	case MODEM_TYPE_GPRS:
		sprintf(str, "+CGDCONT=1,\"IP\",\"%s\"", p->apn);
		break;
	case MODEM_TYPE_CDMA:
		sprintf(str, "%s", "&D2");
		break;
	default:
		return SYS_R_ERR;
	}
	
	if (modem_SendCmd(p, str, "OK\r", 20) != SYS_R_OK)
		return SYS_R_TMO;

	if (p->type == MODEM_TYPE_CDMA)
	{
		os_thd_sleep(5000);
	}
	else
	{
		modem_SendCmd(p, "+CGATT=1", "OK\r", 10);
		modem_SendCmd(p, "+CGATT?", "+CGATT: 1", 30);
	}

	return SYS_R_OK;
}

#if TCPPS_TYPE == TCPPS_T_LWIP
static void modem_linkStatusCB(void *ctx, int errCode, void *arg)
{
	p_modem p = &gsm_xModem;

	switch (errCode)
	{
	case PPPERR_NONE:
		p->ste = MODEM_S_ONLINE;
		sifdefaultroute((int)ctx, 0, 0);
		p->cnt = 0;
		p->tmo = 0;
		break;
	case PPPERR_CONNECT:
	default:
		p->ste = MODEM_S_RESET;
		break;
	}
}
#endif

#if MODEM_SMS_ENABLE

//是否支持Unicode码
#define GSM_UNICODE		0

//用户信息编码方式
#define GSM_7BIT		0
#define GSM_8BIT		4
#define GSM_UCS2		8

typedef struct {
	char		TPA[16];			//目标号码或回复号码(TP-DA或TP-RA)
	char		TP_PID;				//用户信息协议标识(TP-PID)
	char		TP_DCS;				//用户信息编码方式(TP-DCS)
	char		TP_SCTS[16];		//服务时间戳字符串(TP_SCTS), 接收时用到
	char		TP_UD[160];			//原始用户信息(编码前或解码后的TP-UD)
} t_sms;


//可打印字符串转换为字节数据
//如："C8329BFD0E01" --> {0xC8, 0x32, 0x9B, 0xFD, 0x0E, 0x01}
//输入: pSrc - 源字符串指针
//     nSrcLength - 源字符串长度
//输出: pDst - 目标数据指针
//返回: 目标数据长度
int smsString2Bytes(const char *pSrc, u8 *pDst, size_t nLen)
{
	int i;

	for (i = 0; i < nLen; i += 2)
	{
		//输出高4位
		if ((*pSrc >= '0') && (*pSrc <= '9'))
			*pDst = (*pSrc - '0') << 4;
		else
			*pDst = (*pSrc - 'A' + 10) << 4;

		pSrc++;

		//输出低4位
		if ((*pSrc>='0') && (*pSrc<='9'))
			*pDst |= *pSrc - '0';
		else
			*pDst |= *pSrc - 'A' + 10;

		pSrc++;
		pDst++;
	}

	//返回目标数据长度
	return (nLen / 2);
}

//字节数据转换为可打印字符串
//如：{0xC8, 0x32, 0x9B, 0xFD, 0x0E, 0x01} --> "C8329BFD0E01" 
//输入: pSrc - 源数据指针
//     nSrcLength - 源数据长度
//输出: pDst - 目标字符串指针
//返回: 目标字符串长度
const char tab[]="0123456789ABCDEF";	//0x0-0xf的字符查找表
int smsBytes2String(const u8 *pSrc, char *pDst, size_t nLen)
{
	int i;

	for (i = 0; i < nLen; i++)
	{
		*pDst++ = tab[*pSrc >> 4];		//输出高4位
		*pDst++ = tab[*pSrc & 0x0f];	//输出低4位
		pSrc++;
	}

	//输出字符串加个结束符
	*pDst = '\0';

	//返回目标字符串长度
	return (nLen * 2);
}

//7bit编码
//输入: pSrc - 源字符串指针
//     nSrcLength - 源字符串长度
//输出: pDst - 目标编码串指针
//返回: 目标编码串长度
int smsEncode7bit(const char *pSrc, u8 *pDst, size_t nLen)
{
	int nSrc;		//源字符串的计数值
	int nDst;		//目标编码串的计数值
	int nChar;		//当前正在处理的组内字符字节的序号，范围是0-7
	u8 nLeft = 0; 	//上一字节残余的数据

	//计数值初始化
	nSrc = 0;
	nDst = 0;

	//将源串每8个字节分为一组，压缩成7个字节
	//循环该处理过程，直至源串被处理完
	//如果分组不到8字节，也能正确处理
	while (nSrc < nLen)
	{
		//取源字符串的计数值的最低3位
		nChar = nSrc & 7;

		//处理源串的每个字节
		if (nChar == 0)
		{
			//组内第一个字节，只是保存起来，待处理下一个字节时使用
			nLeft = *pSrc;
		}
		else
		{
			//组内其它字节，将其右边部分与残余数据相加，得到一个目标编码字节
			*pDst = (*pSrc << (8-nChar)) | nLeft;

			//将该字节剩下的左边部分，作为残余数据保存起来
			nLeft = *pSrc >> nChar;

			//修改目标串的指针和计数值
			pDst++;
			nDst++;
		}

		//修改源串的指针和计数值
		pSrc++;
		nSrc++;
	}

	//返回目标串长度
	return nDst;
}

//7bit解码
//输入: pSrc - 源编码串指针
//     nSrcLength - 源编码串长度
//输出: pDst - 目标字符串指针
//返回: 目标字符串长度
int smsDecode7bit(const u8 *pSrc, char *pDst, int nSrcLength)
{
	int nSrc;		//源字符串的计数值
	int nDst;		//目标解码串的计数值
	int nByte;		//当前正在处理的组内字节的序号，范围是0-6
	u8 nLeft;	//上一字节残余的数据

	//计数值初始化
	nSrc = 0;
	nDst = 0;
	
	//组内字节序号和残余数据初始化
	nByte = 0;
	nLeft = 0;

	//将源数据每7个字节分为一组，解压缩成8个字节
	//循环该处理过程，直至源数据被处理完
	//如果分组不到7字节，也能正确处理
	while (nSrc < nSrcLength)
	{
		//将源字节右边部分与残余数据相加，去掉最高位，得到一个目标解码字节
		*pDst = ((*pSrc << nByte) | nLeft) & 0x7f;

		//将该字节剩下的左边部分，作为残余数据保存起来
		nLeft = *pSrc >> (7-nByte);

		//修改目标串的指针和计数值
		pDst++;
		nDst++;

		//修改字节计数值
		nByte++;

		//到了一组的最后一个字节
		if (nByte == 7)
		{
			//额外得到一个目标解码字节
			*pDst = nLeft;

			//修改目标串的指针和计数值
			pDst++;
			nDst++;

			//组内字节序号和残余数据初始化
			nByte = 0;
			nLeft = 0;
		}

		//修改源串的指针和计数值
		pSrc++;
		nSrc++;
	}

	//输出字符串加个结束符
	*pDst = '\0';

	//返回目标串长度
	return nDst;
}

//8bit编码
//输入: pSrc - 源字符串指针
//     nSrcLength - 源字符串长度
//输出: pDst - 目标编码串指针
//返回: 目标编码串长度
int smsEncode8bit(const char *pSrc, u8 *pDst, size_t nLen)
{
	//简单复制
	memcpy(pDst, pSrc, nLen);

	return nLen;
}

//8bit解码
//输入: pSrc - 源编码串指针
//     nSrcLength -  源编码串长度
//输出: pDst -  目标字符串指针
//返回: 目标字符串长度
int smsDecode8bit(const u8* pSrc, char* pDst, size_t nLen)
{
	//简单复制
	memcpy(pDst, pSrc, nLen);

	//输出字符串加个结束符
	*(pDst + nLen) = '\0';

	return nLen;
}


#if GSM_UNICODE
/***************************************************************************** 
 * 将一个字符的UTF8编码转换成Unicode(UCS-2和UCS-4)编码. 
 * 
 * 参数: 
 *    pInput      指向输入缓冲区, 以UTF-8编码 
 *    Unic        指向输出缓冲区, 其保存的数据即是Unicode编码值, 
 *                类型为unsigned long . 
 * 
 * 返回值: 
 *    成功则返回该字符的UTF8编码所占用的字节数; 失败则返回0. 
 * 
 * 注意: 
 *     1. UTF8没有字节序问题, 但是Unicode有字节序要求; 
 *        字节序分为大端(Big Endian)和小端(Little Endian)两种; 
 *        在Intel处理器中采用小端法表示, 在此采用小端法表示. (低地址存低位) 
 ****************************************************************************/  
int enc_utf8_to_unicode_one(const unsigned char* pInput, unsigned long *Unic)  
{  
  
    // b1 表示UTF-8编码的pInput中的高字节, b2 表示次高字节, ...  
    char b1, b2, b3, b4, b5, b6;  
  
    *Unic = 0x0; // 把 *Unic 初始化为全零  
    int utfbytes = enc_get_utf8_size(*pInput);  
    unsigned char *pOutput = (unsigned char *) Unic;  
  
    switch ( utfbytes )  
    {  
        case 0:  
            *pOutput     = *pInput;  
            utfbytes    += 1;  
            break;  
        case 2:  
            b1 = *pInput;  
            b2 = *(pInput + 1);  
            if ( (b2 & 0xE0) != 0x80 )  
                return 0;  
            *pOutput     = (b1 << 6) + (b2 & 0x3F);  
            *(pOutput+1) = (b1 >> 2) & 0x07;  
            break;  
        case 3:  
            b1 = *pInput;  
            b2 = *(pInput + 1);  
            b3 = *(pInput + 2);  
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80) )  
                return 0;  
            *pOutput     = (b2 << 6) + (b3 & 0x3F);  
            *(pOutput+1) = (b1 << 4) + ((b2 >> 2) & 0x0F);  
            break;  
        case 4:  
            b1 = *pInput;  
            b2 = *(pInput + 1);  
            b3 = *(pInput + 2);  
            b4 = *(pInput + 3);  
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)  
                    || ((b4 & 0xC0) != 0x80) )  
                return 0;  
            *pOutput     = (b3 << 6) + (b4 & 0x3F);  
            *(pOutput+1) = (b2 << 4) + ((b3 >> 2) & 0x0F);  
            *(pOutput+2) = ((b1 << 2) & 0x1C)  + ((b2 >> 4) & 0x03);  
            break;  
        case 5:  
            b1 = *pInput;  
            b2 = *(pInput + 1);  
            b3 = *(pInput + 2);  
            b4 = *(pInput + 3);  
            b5 = *(pInput + 4);  
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)  
                    || ((b4 & 0xC0) != 0x80) || ((b5 & 0xC0) != 0x80) )  
                return 0;  
            *pOutput     = (b4 << 6) + (b5 & 0x3F);  
            *(pOutput+1) = (b3 << 4) + ((b4 >> 2) & 0x0F);  
            *(pOutput+2) = (b2 << 2) + ((b3 >> 4) & 0x03);  
            *(pOutput+3) = (b1 << 6);  
            break;  
        case 6:  
            b1 = *pInput;  
            b2 = *(pInput + 1);  
            b3 = *(pInput + 2);  
            b4 = *(pInput + 3);  
            b5 = *(pInput + 4);  
            b6 = *(pInput + 5);  
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)  
                    || ((b4 & 0xC0) != 0x80) || ((b5 & 0xC0) != 0x80)  
                    || ((b6 & 0xC0) != 0x80) )  
                return 0;  
            *pOutput     = (b5 << 6) + (b6 & 0x3F);  
            *(pOutput+1) = (b5 << 4) + ((b6 >> 2) & 0x0F);  
            *(pOutput+2) = (b3 << 2) + ((b4 >> 4) & 0x03);  
            *(pOutput+3) = ((b1 << 6) & 0x40) + (b2 & 0x3F);  
            break;  
        default:  
            return 0;  
            break;  
    }  
  
    return utfbytes;  
}  


/***************************************************************************** 
 * 将一个字符的Unicode(UCS-2和UCS-4)编码转换成UTF-8编码. 
 * 
 * 参数: 
 *    unic     字符的Unicode编码值 
 *    pOutput  指向输出的用于存储UTF8编码值的缓冲区的指针 
 *    outsize  pOutput缓冲的大小 
 * 
 * 返回值: 
 *    返回转换后的字符的UTF8编码所占的字节数, 如果出错则返回 0 . 
 * 
 * 注意: 
 *     1. UTF8没有字节序问题, 但是Unicode有字节序要求; 
 *        字节序分为大端(Big Endian)和小端(Little Endian)两种; 
 *        在Intel处理器中采用小端法表示, 在此采用小端法表示. (低地址存低位) 
 *     2. 请保证 pOutput 缓冲区有最少有 6 字节的空间大小! 
 ****************************************************************************/  
int enc_unicode_to_utf8_one(unsigned long unic, unsigned char *pOutput,  
        int outSize)  
{
  
    if ( unic <= 0x0000007F )  
    {  
        // * U-00000000 - U-0000007F:  0xxxxxxx  
        *pOutput     = (unic & 0x7F);  
        return 1;  
    }  
    else if ( unic >= 0x00000080 && unic <= 0x000007FF )  
    {  
        // * U-00000080 - U-000007FF:  110xxxxx 10xxxxxx  
        *(pOutput+1) = (unic & 0x3F) | 0x80;  
        *pOutput     = ((unic >> 6) & 0x1F) | 0xC0;  
        return 2;  
    }  
    else if ( unic >= 0x00000800 && unic <= 0x0000FFFF )  
    {  
        // * U-00000800 - U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx  
        *(pOutput+2) = (unic & 0x3F) | 0x80;  
        *(pOutput+1) = ((unic >>  6) & 0x3F) | 0x80;  
        *pOutput     = ((unic >> 12) & 0x0F) | 0xE0;  
        return 3;  
    }  
    else if ( unic >= 0x00010000 && unic <= 0x001FFFFF )  
    {  
        // * U-00010000 - U-001FFFFF:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx  
        *(pOutput+3) = (unic & 0x3F) | 0x80;  
        *(pOutput+2) = ((unic >>  6) & 0x3F) | 0x80;  
        *(pOutput+1) = ((unic >> 12) & 0x3F) | 0x80;  
        *pOutput     = ((unic >> 18) & 0x07) | 0xF0;  
        return 4;  
    }  
    else if ( unic >= 0x00200000 && unic <= 0x03FFFFFF )  
    {  
        // * U-00200000 - U-03FFFFFF:  111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx  
        *(pOutput+4) = (unic & 0x3F) | 0x80;  
        *(pOutput+3) = ((unic >>  6) & 0x3F) | 0x80;  
        *(pOutput+2) = ((unic >> 12) & 0x3F) | 0x80;  
        *(pOutput+1) = ((unic >> 18) & 0x3F) | 0x80;  
        *pOutput     = ((unic >> 24) & 0x03) | 0xF8;  
        return 5;  
    }  
    else if ( unic >= 0x04000000 && unic <= 0x7FFFFFFF )  
    {  
        // * U-04000000 - U-7FFFFFFF:  1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx  
        *(pOutput+5) = (unic & 0x3F) | 0x80;  
        *(pOutput+4) = ((unic >>  6) & 0x3F) | 0x80;  
        *(pOutput+3) = ((unic >> 12) & 0x3F) | 0x80;  
        *(pOutput+2) = ((unic >> 18) & 0x3F) | 0x80;  
        *(pOutput+1) = ((unic >> 24) & 0x3F) | 0x80;  
        *pOutput     = ((unic >> 30) & 0x01) | 0xFC;  
        return 6;  
    }  
  
    return 0;  
}  


#endif


//正常顺序的字符串转换为两两颠倒的字符串，若长度为奇数，补'F'凑成偶数
//如："8613851872468" --> "683158812764F8"
//输入: pSrc - 源字符串指针
//     nSrcLength - 源字符串长度
//输出: pDst - 目标字符串指针
//返回: 目标字符串长度
int smsInvertNumbers(const char *pSrc, char *pDst, int nSrcLen)
{
	int i, nDstLen;		//目标字符串长度
	char ch;			//用于保存一个字符

	//复制串长度
	nDstLen = nSrcLen;

	//两两颠倒
	for (i = 0; i < nSrcLen; i += 2)
	{
		ch = *pSrc++;		//保存先出现的字符
		*pDst++ = *pSrc++;	//复制后出现的字符
		*pDst++ = ch;		//复制先出现的字符
	}

	//源串长度是奇数吗？
	if (nSrcLen & 1)
	{
		*(pDst - 2) = 'F';	//补'F'
		nDstLen += 1;		//目标串长度加1
	}

	//输出字符串加个结束符
	*pDst = '\0';

	//返回目标字符串长度
	return nDstLen;
}

//两两颠倒的字符串转换为正常顺序的字符串
//如："683158812764F8" --> "8613851872468"
//输入: pSrc - 源字符串指针
//     nSrcLength - 源字符串长度
//输出: pDst - 目标字符串指针
//返回: 目标字符串长度
int smsSerializeNumbers(const char *pSrc, char *pDst, int nSrcLen)
{
	int i, nDstLen;		//目标字符串长度
	char ch;			//用于保存一个字符

	//复制串长度
	nDstLen = nSrcLen;

	//两两颠倒
	for (i = 0; i < nSrcLen; i += 2)
	{
		ch = *pSrc++;		//保存先出现的字符
		*pDst++ = *pSrc++;	//复制后出现的字符
		*pDst++ = ch;		//复制先出现的字符
	}

	//最后的字符是'F'吗？
	if (*(pDst-1) == 'F')
	{
		pDst--;
		nDstLen -= 1;		//目标字符串长度减1
	}

	//输出字符串加个结束符
	*pDst = '\0';

	//返回目标字符串长度
	return nDstLen;
}

//PDU编码，用于编制、发送短消息
//输入: pSrc - 源PDU参数指针
//输出: pDst - 目标PDU串指针
//返回: 目标PDU串长度
int smsEncodePdu(p_modem p, char *pDst, t_sms *pSrc, int nSrcLen)
{
	size_t nLen;			//内部用的串长度
	int nDstLen;		//目标PDU串长度
	u8 pBuf[256];	//内部用的缓冲区

	//SMSC地址信息段
	nLen = strlen(p->SCA);	//SMSC地址字符串的长度	
	pBuf[0] = (char)((nLen & 1) == 0 ? nLen : nLen + 1) / 2 + 1;	//SMSC地址信息长度
	pBuf[1] = 0x91;		//固定: 用国际格式号码
	nDstLen = smsBytes2String(pBuf, pDst, 2);		//转换2个字节到目标PDU串
	nDstLen += smsInvertNumbers(p->SCA, &pDst[nDstLen], nLen);	//转换SMSC号码到目标PDU串

	//TPDU段基本参数、目标地址等
	nLen = strlen(pSrc->TPA);	//TP-DA地址字符串的长度
	pBuf[0] = 0x11;					//是发送短信(TP-MTI=01)，TP-VP用相对格式(TP-VPF=10)
	pBuf[1] = 0;						//TP-MR=0
	pBuf[2] = (char)nLen;			//目标地址数字个数(TP-DA地址字符串真实长度)
	pBuf[3] = 0x91;					//固定: 用国际格式号码
	nDstLen += smsBytes2String(pBuf, &pDst[nDstLen], 4);		//转换4个字节到目标PDU串
	nDstLen += smsInvertNumbers(pSrc->TPA, &pDst[nDstLen], nLen);	//转换TP-DA到目标PDU串

	//TPDU段协议标识、编码方式、用户信息等
	if (pSrc->TP_DCS == GSM_UCS2)
		nLen = strlen(pSrc->TP_UD);
	else
		nLen = nSrcLen;
	
	pBuf[0] = pSrc->TP_PID;			//协议标识(TP-PID)
	pBuf[1] = pSrc->TP_DCS;			//用户信息编码方式(TP-DCS)
	pBuf[2] = 0;						//有效期(TP-VP)为5分钟
	if (pSrc->TP_DCS == GSM_7BIT)
	{
		//7-bit编码方式
		pBuf[3] = nLen;			//编码前长度
		nLen = smsEncode7bit(pSrc->TP_UD, &pBuf[4], nLen+1) + 4;	//转换TP-DA到目标PDU串
	}
#if GSM_UNICODE
	else if (pSrc->TP_DCS == GSM_UCS2)
	{
		//UCS2编码方式
		pBuf[3] = smsEncodeUcs2(pSrc->TP_UD, &pBuf[4], nLen);	//转换TP-DA到目标PDU串
		nLen = pBuf[3] + 4;		//nLength等于该段数据长度
	}
#endif
	else
	{
		//8-bit编码方式
		pBuf[3] = smsEncode8bit(pSrc->TP_UD, &pBuf[4], nLen);	//转换TP-DA到目标PDU串
		nLen = pBuf[3] + 4;		//nLength等于该段数据长度
	}
	nDstLen += smsBytes2String(pBuf, &pDst[nDstLen], nLen);		//转换该段数据到目标PDU串

	//返回目标字符串长度
	return nDstLen;
}

//PDU解码，用于接收、阅读短消息
//输入: pSrc - 源PDU串指针
//输出: pDst - 目标PDU参数指针
//返回: 用户信息串长度
int smsDecodePdu(p_modem p, const char *pSrc, t_sms *pDst)
{
	int nDstLen;		//目标PDU串长度
	u8 tmp;		//内部用的临时字节变量
	u8 pBuf[256];	//内部用的缓冲区

	//SMSC地址信息段
	smsString2Bytes(pSrc, &tmp, 2);	//取长度
	tmp = (tmp - 1) * 2;	//SMSC号码串长度
	pSrc += 4;			//指针后移，忽略了SMSC地址格式
	smsSerializeNumbers(pSrc, p->SCA, tmp);	//转换SMSC号码到目标PDU串
	pSrc += tmp;		//指针后移

	//TPDU段基本参数
	smsString2Bytes(pSrc, &tmp, 2);	//取基本参数
	pSrc += 2;		//指针后移

	//取回复号码
	smsString2Bytes(pSrc, &tmp, 2);	//取长度
	//调整奇偶性
	if (tmp & 1)
		tmp += 1;
	pSrc += 4;			//指针后移，忽略了回复地址(TP-RA)格式
	smsSerializeNumbers(pSrc, pDst->TPA, tmp);	//取TP-RA号码
	pSrc += tmp;		//指针后移

	//TPDU段协议标识、编码方式、用户信息等
	smsString2Bytes(pSrc, (u8*)&pDst->TP_PID, 2);	//取协议标识(TP-PID)
	pSrc += 2;		//指针后移
	smsString2Bytes(pSrc, (u8*)&pDst->TP_DCS, 2);	//取编码方式(TP-DCS)
	pSrc += 2;		//指针后移
	smsSerializeNumbers(pSrc, pDst->TP_SCTS, 14);		//服务时间戳字符串(TP_SCTS) 
	pSrc += 14;		//指针后移
	smsString2Bytes(pSrc, &tmp, 2);	//用户信息长度(TP-UDL)
	pSrc += 2;		//指针后移
	if (pDst->TP_DCS == GSM_7BIT)
	{
		//7-bit解码
		nDstLen = smsString2Bytes(pSrc, pBuf, tmp & 7 ? (int)tmp * 7 / 4 + 2 : (int)tmp * 7 / 4);	//格式转换
		smsDecode7bit(pBuf, pDst->TP_UD, nDstLen);	//转换到TP-DU
		nDstLen = tmp;
	}
#if GSM_UNICODE
	else if (pDst->TP_DCS == GSM_UCS2)
	{
		//UCS2解码
		nDstLen = smsString2Bytes(pSrc, pBuf, tmp * 2);			//格式转换
		nDstLen = smsDecodeUcs2(pBuf, pDst->TP_UD, nDstLen);	//转换到TP-DU
	}
#endif
	else
	{
		//8-bit解码
		nDstLen = smsString2Bytes(pSrc, pBuf, tmp * 2);			//格式转换
		nDstLen = smsDecode8bit(pBuf, pDst->TP_UD, nDstLen);	//转换到TP-DU
	}

	//返回目标字符串长度
	return nDstLen;
}


buf sms_Buf = {0};
void modem_ReadSms(p_modem p)
{
	size_t nTmo, nLen;
	int j, nIdx;
	char *pTemp, str[20];
	t_sms xSMS;

	uart_SendStr(p->uart, "AT+CMGL=4\r\n");
	
	os_thd_sleep(100);
	
	for (nTmo = 1000 / OS_TICK_MS; nTmo; nTmo--)
	{
		if (uart_RecTmo(p->uart, p->rbuf, OS_TICK_MS) != SYS_R_OK)
			continue;
		
		if ((pTemp = buffstr(p->rbuf, "+CMGL:")) == NULL)
			continue;
		
		buf_Remove(p->rbuf, pTemp - (char *)p->rbuf->p);
		os_thd_sleep(100);
		
		uart_Recive(p->uart, p->rbuf);
		nIdx = atoi((char *)p->rbuf->p);
		for (j = 0; j < 3; j++)
		{
			if ((pTemp = buffstr(p->rbuf, ",")) == NULL)
				continue;
			buf_Remove(p->rbuf, pTemp - (char *)p->rbuf->p);
		}
		
		nLen = atoi((char *)p->rbuf->p);
		if ((pTemp = buffstr(p->rbuf, STRING_0D0A)) == NULL)
			continue;
		
		buf_Remove(p->rbuf, pTemp - (char *)p->rbuf->p);
		for (; nTmo; nTmo--)
		{
			if (p->rbuf->len >= nLen)
				break;
			uart_RecTmo(p->uart, p->rbuf, OS_TICK_MS);
		}
		nLen = 0;
		if (nTmo)
		{
			nLen = smsDecodePdu(p, (char *)p->rbuf->p, &xSMS);	//PDU串解码
//			sprintf(str, "+CMGD=%d", nIdx);	//生成命令
//			modem_SendCmd(p, str, "OK\r", 10);
			break;
		}
	}
	
	if (nLen)
		buf_Push(sms_Buf, xSMS.TP_UD, nLen);
}


#endif


static int modem_IsPowerOnEnable()
{
	p_modem p = &gsm_xModem;
	int res = 1;

	if (p->retrytime)
	{
		if (p->retryed >= p->retrytime)
		{
			res = 0;
			if (p->hour != rtc_pTm()->tm_hour)
				p->retryed = 0;
		}
		else
		{
			p->hour = rtc_pTm()->tm_hour;
		}
	}
	
	if (res)
	{
		if (p->cnt > p->span)
		{
			p->cnt = 0;
			p->retryed += 1;
		}
		else
		{
			res = 0;
		}
	}
	
	return res;
}



void modem_Init()
{
	p_modem p = &gsm_xModem;
	t_modem_def *pDef = &tbl_bspModem;

	memset(p, 0, sizeof(t_modem));
	
	sys_GpioConf(&pDef->act);
#if MODEM_PWR_ENABLE
	sys_GpioConf(&pDef->power);
#endif
#if MODEM_RST_ENABLE
	sys_GpioConf(&pDef->rst);
#endif
#if MODEM_CTS_ENABLE
	sys_GpioConf(&pDef->cts);
#endif
#if MODEM_RTS_ENABLE
	sys_GpioConf(&pDef->rts);
#endif
#if MODEM_DTR_ENABLE
	sys_GpioConf(&pDef->dtr);
#endif
#if MODEM_DCD_ENABLE
	sys_GpioConf(&pDef->dcd);
#endif
	p->uart = uart_Open(MODEM_UART_ID, OS_TMO_FOREVER);
}



void modem_Run()
{
	p_modem p = &gsm_xModem;
	sys_res res;
#if MODEM_TCP_ENABLE
	char *pTemp;
#endif
	
	p->cnt += 1;

	switch (p->ste)
	{
	default:
#if MODEM_PWR_ENABLE
		modem_Pwr(0);
#endif
		modem_Act(0);
#if MODEM_RST_ENABLE
		modem_Rst(0);
#endif
#if MODEM_DTR_ENABLE
		modem_Dtr(0);
#endif
		p->ste = MODEM_S_POWEROFF;
		p->signal = 0;
#if TCPPS_TYPE == TCPPS_T_LWIP
		if (p->dialed)
		{
			pppSigHUP(0);
			pppClose(0);
		}
#endif
		p->dialed = 0;
		p->tmo = (1 + p->tmo) * 3;
		if (p->tmo > 1800)
			p->tmo = 1800;
		p->cnt = 0;
		MODEM_DBGOUT("<Modem> Power OFF %dS", p->tmo);
		break;
		
	case MODEM_S_POWEROFF:
//		if (modem_IsPowerOnEnable()) {
		if (p->cnt > p->tmo)
		{
			modem_Act(1);
			p->ste = MODEM_S_POWERON;
			p->cnt = 0;
			MODEM_DBGOUT("<Modem> Power ON");
		}
		break;
		
	case MODEM_S_POWERON:
#if MODEM_RST_ENABLE
		if (p->cnt == 3)
			modem_Rst(1);
#endif
#if MODEM_PWR_ENABLE
		if (p->cnt == 5)
			modem_Pwr(1);
#endif
#if MODEM_RST_ENABLE
		if (p->cnt == 7)
			modem_Rst(0);
		if (p->cnt == 8)
			modem_Rst(1);
#endif
		if (p->cnt == 9)
			modem_Act(0);
		if (p->cnt == 12)
			modem_Act(1);
		if (p->cnt > 16)
		{
			p->ste = MODEM_S_INIT;
			p->cnt = 0;
		}
		break;
		
	case MODEM_S_INIT:
		//Initiate modem and get type of modem
		res = modem_InitCmd(p);
		
		buf_Release(p->rbuf);
		
		if (res != SYS_R_OK)
		{
			MODEM_DBGOUT("<Modem> Init fail");
			p->ste = MODEM_S_RESET;
		}
		else
		{
			MODEM_DBGOUT("<Modem> Dial");
#if MODEM_TCP_ENABLE
			if (p->mtcp)
			{
				if (modem_SendCmd(p, "$MYNETACT=0,1", "OK\r", 10) == SYS_R_OK)
					p->ste = MODEM_S_WAITDIAL;
				else
					p->ste = MODEM_S_RESET;
			}
			else
#endif
			{
				if (p->type == MODEM_TYPE_CDMA)
					uart_SendStr(p->uart, MODEM_CMD_DIAL_CDMA);
				else
					uart_SendStr(p->uart, MODEM_CMD_DIAL_GPRS);
				
				p->dialed = 1;
				
#if TCPPS_TYPE == TCPPS_T_LWIP
				if (p->type == MODEM_TYPE_CDMA)
					pppSetAuth(PPPAUTHTYPE_ANY, p->user, p->pwd);
				else
					pppSetAuth(PPPAUTHTYPE_ANY, "", "");
				pppOpen(p, modem_linkStatusCB, 0);
#endif

#if TCPPS_TYPE == TCPPS_T_KEILTCP
				if (p->type == MODEM_TYPE_CDMA)
					ppp_connect(MODEM_CMD_DIAL_CDMA, p->user, p->pwd);
				else
					ppp_connect(MODEM_CMD_DIAL_GPRS, "", "");
#endif

				p->ste = MODEM_S_WAITDIAL;
			}
			p->cnt = 0;
		}
		break;
		
	case MODEM_S_WAITDIAL:
		//Waitting for dial
		if (p->cnt > 120)
		{
			p->ste = MODEM_S_RESET;
			break;
		}
#if MODEM_TCP_ENABLE
		if (p->mtcp)
		{
			if (modem_SendCmd(p, "$MYNETACT?", "OK\r", 1) == SYS_R_OK)
			{
				pTemp = buffstr(p->rbuf, "$MYNETACT: 0,1");
				if (pTemp != NULL)
				{
					p->ste = MODEM_S_ONLINE;
					p->cnt = 0;
					p->tmo = 0;
				}
			}
			break;
		}
#endif
#if TCPPS_TYPE == TCPPS_T_KEILTCP
		if (modem_IsOnline())
		{
			p->ste = MODEM_S_ONLINE;
			p->cnt = 0;
			p->tmo = 0;
		}
#endif
		break;

	case MODEM_S_ONLINE:
		if (p->cnt > p->idle)
		{
			p->ste = MODEM_S_RESET;
			MODEM_DBGOUT("<Modem> Idle %d tmo %d", p->idle, p->cnt);
		}
		break;
	}
#if MODEM_SMS_ENABLE
	if ((p->dialed) && (p->cnt == 30))
	{
		p->dialed = 0;
		uart_SendStr(p->uart, "+++");
		os_thd_sleep(1200);
		
		if (modem_SendCmd(p, "+CSQ", "OK\r", 3) == SYS_R_OK)
		{
			char *pTemp;
			int nTemp;
			if ((pTemp = buffstr(p->rbuf, ",")) != NULL)
			{
				nTemp = atoi(pTemp - 3);
				if ((nTemp > 0) && (nTemp < 32))
					p->signal = nTemp;
			}
			modem_ReadSms(p);

			if (modem_SendCmd(p, "O", "OK\r", 3) != SYS_R_OK)
				p->ste = MODEM_S_RESET;
		}
		p->dialed = 1;
	}
#endif
}


void modem_Config(const char *pApn, const char *pUser, const char *pPwd, 
							int nSpan, int nRetry, int nFlag)
{
	p_modem p = &gsm_xModem;
	size_t nLen;

	nLen = MIN(sizeof(p->apn) - 1, strlen(pApn));
	memcpy(p->apn, pApn, nLen);
	p->apn[nLen] = '\0';
	nLen = MIN(sizeof(p->user) - 1, strlen(pUser));
	memcpy(p->user, pUser, nLen);
	p->user[nLen] = '\0';
	nLen = MIN(sizeof(p->pwd) - 1, strlen(pPwd));
	memcpy(p->pwd, pPwd, nLen);
	p->pwd[nLen] = '\0';
	p->idle = nSpan * 3;
	p->retrytime = nRetry;
#if MODEM_TCP_ENABLE
	p->flag = nFlag;
#endif
}

int modem_RxDialed(buf b)
{
	p_modem p = &gsm_xModem;

	if (p->dialed == 0)
		return 0;

	return uart_Recive(p->uart, b);
}

int modem_IsDataMode()
{

	return gsm_xModem.dialed;
}

int modem_IsOnline()
{

#if MODEM_DCD_ENABLE
	int i;
	for (i = 0; i < 5; i++)
	{
		if (modem_Dcd() == 0)
			break;
	}
	if (i >= 5)
		return 0;
#endif
 
#if MODEM_TCP_ENABLE
	if (gsm_xModem.mtcp && (gsm_xModem.ste == MODEM_S_ONLINE))
		return 1;
#endif
	
#if TCPPS_TYPE == TCPPS_T_LWIP
	if (gsm_xModem.ste == MODEM_S_ONLINE)
		return 1;
#endif
	
#if TCPPS_TYPE == TCPPS_T_KEILTCP
	if (ppp_is_up() == __TRUE)
		return 1;
#endif
	
	return 0;
}

int modem_GetSignal()
{

	return gsm_xModem.signal;
}

int modem_GetState()
{

	return gsm_xModem.ste;
}

int modem_GetType()
{

	return gsm_xModem.type;
}

int modem_GetVersion(char *pVersion)
{
	p_modem p = &gsm_xModem;

	if (p->ver[0] == '\0')
		return 0;
	
	memcpy(pVersion, p->ver, 20);
	return 1;
}

int modem_GetCCID(char *pCCID)
{
	p_modem p = &gsm_xModem;

	if (p->ccid[0] == '\0')
		return 0;
	
	memcpy(pCCID, p->ccid, 20);
	return 1;
}

int modem_GetIMEI(char *pIMEI)
{
	p_modem p = &gsm_xModem;

	if (p->imei[0] == '\0')
		return 0;
	
	memcpy(pIMEI, p->imei, 20);
	return 1;
}


#if MODEM_FLOWCTL_ENABLE
void modem_FlowStep(int nIsTx, size_t nLen)
{
	if (nIsTx)
		gsm_xModem.flow_t += nLen;
	else
		gsm_xModem.flow_r += nLen;
}

int modem_GetFlow()
{
	int nFlow;

	nFlow = gsm_xModem.flow_r + gsm_xModem.flow_t;
	gsm_xModem.flow_r = 0;
	gsm_xModem.flow_t = 0;
	
	return nFlow;
}
int modem_GetRcvFlow()
{
	int nRcvFlow;

	nRcvFlow = gsm_xModem.flow_r;
	gsm_xModem.flow_r = 0;
	
	return nRcvFlow;
}
int modem_GetSendFlow()
{
	int nSendFlow;

	nSendFlow = gsm_xModem.flow_t;
	gsm_xModem.flow_t = 0;
	
	return nSendFlow;
}
#endif

void modem_Refresh()
{

	if (modem_IsOnline())
		gsm_xModem.cnt = 0;
}


#if MODEM_TCP_ENABLE
int modem_IsMTcp()
{
	
	return gsm_xModem.mtcp;
}

int mtcp_IsTcpCon()
{
	
	return gsm_xModem.mcon;
}

sys_res mtcp_TcpRecv(buf b)
{
	p_modem p = &gsm_xModem;
	size_t i, nLen;
	char *pTemp;

	if (modem_IsOnline() == 0)
		return SYS_R_ERR;
	
	if (uart_RecTmo(p->uart, p->rbuf, OS_TICK_MS) != SYS_R_OK)
		return SYS_R_ERR;
	
	pTemp = buffstr(p->rbuf, "$MYURCREAD:");
	if (pTemp == NULL)
		return SYS_R_ERR;

	buf_Release(p->rbuf);
	uart_SendStr(p->uart, "AT$MYNETREAD=0,1460\r\n");
	os_thd_sleep(100);
	for (i = 500 / OS_TICK_MS; i; i--)
	{
		if (uart_RecTmo(p->uart, p->rbuf, OS_TICK_MS) != SYS_R_OK)
			continue;

		pTemp = buffstr(p->rbuf, "OK\r\n");
		if (pTemp)
			break;
	}
	if (i)
	{
		pTemp = buffstr(p->rbuf, "$MYNETREAD:");
		if (pTemp)
		{
			pTemp = memfind(pTemp, 3, ",");
			if (pTemp)
			{
				nLen = atoi(pTemp);
				if (nLen > 1460)
					nLen = 1460;
				pTemp = memfind(pTemp, 6, STRING_0D0A);
				if (pTemp)
				{
					buf_Remove(p->rbuf, (u8 *)pTemp - p->rbuf->p);
					if (p->rbuf->len >= nLen)
					{
						buf_Push(b, p->rbuf->p, nLen);
						buf_Remove(p->rbuf, nLen);
#if MODEM_FLOWCTL_ENABLE
						gsm_xModem.flow_r += nLen;
#endif
						return SYS_R_OK;
					}
				}
			}
		}
	}
	
	buf_Release(p->rbuf);

	return SYS_R_ERR;
}

sys_res mtcp_TcpConnect(const u8 *pIp, int nPort)
{
	p_modem p = &gsm_xModem;
	char str[64];

	if (modem_IsOnline() == 0)
		return SYS_R_ERR;
	
	sprintf(str, "$MYNETSRV=0,0,0,0,\"%d.%d.%d.%d:%d\"", pIp[0], pIp[1], pIp[2], pIp[3], nPort);
	modem_SendCmd(p, str, "OK\r", 1);

	if (modem_SendCmd(p, "$MYNETOPEN=0", "OK\r", 20) == SYS_R_OK)
	{
		p->mcon = 1;
		buf_Release(p->rbuf);
		
		return SYS_R_OK;
	}
	
	return SYS_R_TMO;
}

sys_res mtcp_TcpSend(const void *pData, size_t nLen)
{
	p_modem p = &gsm_xModem;
	char str[20];
	size_t nTmo;

	if (modem_IsOnline() == 0)
		return SYS_R_ERR;
	
	if (p->mcon == 0)
		return SYS_R_ERR;
	
	sprintf(str, "AT$MYNETWRITE=0,%d\r\n", nLen);
	uart_SendStr(p->uart, str);
	os_thd_sleep(100);
	
	for (nTmo = 8000 / OS_TICK_MS; nTmo; nTmo--)
	{
		uart_RecTmo(p->uart, p->rbuf, OS_TICK_MS);
		if (buffstr(p->rbuf, "$MYNETWRITE:") == NULL)
			continue;
		
		uart_Send(p->uart, pData, nLen);
#if MODEM_FLOWCTL_ENABLE
		gsm_xModem.flow_t += nLen;
#endif
		return SYS_R_OK;
	}
	
	return SYS_R_TMO;
}

sys_res mtcp_TcpClose()
{
	p_modem p = &gsm_xModem;
	size_t nTmo;

	p->mcon = 0;
	uart_SendStr(p->uart, "AT$MYNETCLOSE=0\r\n");
	os_thd_sleep(100);
	
	for (nTmo = 1000 / OS_TICK_MS; nTmo; nTmo--)
	{
		if (uart_RecTmo(p->uart, p->rbuf, OS_TICK_MS) != SYS_R_OK)
			continue;
		
		if (buffstr(p->rbuf, "OK\r") != NULL)
			break;
		
		if (buffstr(p->rbuf, "ERROR\r") != NULL)
			break;
	}
	
	return SYS_R_OK;
}
#endif



#endif

