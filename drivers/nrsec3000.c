#if NRSEC3000_ENABLE
#include <drivers/nrsec3000.h>


//Private Defines


#define NRSEC3000_SPI_SPEED			33000000
#define TR_INS                      0xAA



//Private Variables
static t_nrsec3000 nrsec3000;

//Internal Functions
#if NRSEC3000_DEBUG_ENABLE
#define nrsec3000_DbgOut				dbg_printf
#else
#define nrsec3000_DbgOut(...)
#endif

#define CMD_HEAD_SIZE           5

const unsigned char crc7_table[256] = {
	0x00, 0x09, 0x12, 0x1b, 0x24, 0x2d, 0x36, 0x3f,
	0x48, 0x41, 0x5a, 0x53, 0x6c, 0x65, 0x7e, 0x77,
	0x19, 0x10, 0x0b, 0x02, 0x3d, 0x34, 0x2f, 0x26,
	0x51, 0x58, 0x43, 0x4a, 0x75, 0x7c, 0x67, 0x6e,
	0x32, 0x3b, 0x20, 0x29, 0x16, 0x1f, 0x04, 0x0d,
	0x7a, 0x73, 0x68, 0x61, 0x5e, 0x57, 0x4c, 0x45,
	0x2b, 0x22, 0x39, 0x30, 0x0f, 0x06, 0x1d, 0x14,
	0x63, 0x6a, 0x71, 0x78, 0x47, 0x4e, 0x55, 0x5c,
	0x64, 0x6d, 0x76, 0x7f, 0x40, 0x49, 0x52, 0x5b,
	0x2c, 0x25, 0x3e, 0x37, 0x08, 0x01, 0x1a, 0x13,
	0x7d, 0x74, 0x6f, 0x66, 0x59, 0x50, 0x4b, 0x42,
	0x35, 0x3c, 0x27, 0x2e, 0x11, 0x18, 0x03, 0x0a,
	0x56, 0x5f, 0x44, 0x4d, 0x72, 0x7b, 0x60, 0x69,
	0x1e, 0x17, 0x0c, 0x05, 0x3a, 0x33, 0x28, 0x21,
	0x4f, 0x46, 0x5d, 0x54, 0x6b, 0x62, 0x79, 0x70,
	0x07, 0x0e, 0x15, 0x1c, 0x23, 0x2a, 0x31, 0x38,
	0x41, 0x48, 0x53, 0x5a, 0x65, 0x6c, 0x77, 0x7e,
	0x09, 0x00, 0x1b, 0x12, 0x2d, 0x24, 0x3f, 0x36,
	0x58, 0x51, 0x4a, 0x43, 0x7c, 0x75, 0x6e, 0x67,
	0x10, 0x19, 0x02, 0x0b, 0x34, 0x3d, 0x26, 0x2f,
	0x73, 0x7a, 0x61, 0x68, 0x57, 0x5e, 0x45, 0x4c,
	0x3b, 0x32, 0x29, 0x20, 0x1f, 0x16, 0x0d, 0x04,
	0x6a, 0x63, 0x78, 0x71, 0x4e, 0x47, 0x5c, 0x55,
	0x22, 0x2b, 0x30, 0x39, 0x06, 0x0f, 0x14, 0x1d,
	0x25, 0x2c, 0x37, 0x3e, 0x01, 0x08, 0x13, 0x1a,
	0x6d, 0x64, 0x7f, 0x76, 0x49, 0x40, 0x5b, 0x52,
	0x3c, 0x35, 0x2e, 0x27, 0x18, 0x11, 0x0a, 0x03,
	0x74, 0x7d, 0x66, 0x6f, 0x50, 0x59, 0x42, 0x4b,
	0x17, 0x1e, 0x05, 0x0c, 0x33, 0x3a, 0x21, 0x28,
	0x5f, 0x56, 0x4d, 0x44, 0x7b, 0x72, 0x69, 0x60,
	0x0e, 0x07, 0x1c, 0x15, 0x2a, 0x23, 0x38, 0x31,
	0x46, 0x4f, 0x54, 0x5d, 0x62, 0x6b, 0x70, 0x79
};

unsigned char get_crc7(const unsigned char *buff, int len) 
{
      unsigned char crc7_accum = 0;
      int i;

      for (i=0;  i < len; i++) {
          crc7_accum = crc7_table[(crc7_accum << 1) ^ buff[i]];
      }
      return crc7_accum;
}

static spi_t *nrsec3000_SpiGet()
{
	spi_t *p;
	
	p = spi_Open(0, OS_TMO_FOREVER);
	spi_Config(p, SPI_SCKIDLE_HIGH, SPI_LATCH_2EDGE, NRSEC3000_SPI_SPEED);
	return p;
}

//External Functions
void nrsec3000_Init()
{
    
}
//发送命令
sys_res nrsec3000_SendCmd(p_nrsec3000 p,const void  *cmd,u8 cmdLen)
{
	sys_res res;

    /* send five command header */ 
	res = spi_Send(p->spi, cmd, cmdLen);
	nrsec3000_DbgOut("<NRSEC3000>%02x %02x %02x %02x %02x ", cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
	return res;
}
//RcvINS
sys_res nrsec3000_RcvINS(p_nrsec3000 p, u8 *rbuf, u8 ins,int cnt)
{
//	int cnt = 1000;
	/* receive ins */
	for (; cnt>0; cnt--)
	{
		spi_TransChar(p->spi, TR_INS, rbuf);
		if (rbuf[0] == ins)
			return SYS_R_OK;
    }
    return SYS_R_ERR;
}
//Rcvlen
u16 nrsec3000_RcvLEN(p_nrsec3000 p, size_t tbufLen, u8 *rbuf)
{
	/* receive length */
	u16 nLen = 0;

	if (spi_TransChar(p->spi, TR_INS, &rbuf[0])== SYS_R_OK)
	{
		switch (tbufLen)
		{
		case 1:
			return rbuf[0];
		case 2:
			if (spi_TransChar(p->spi, TR_INS, &rbuf[1]) == SYS_R_OK){
				if (rbuf[0] != 0xFF)
					nLen = rbuf[0] * 0x100 + rbuf[1];
				else
					return rbuf[1];
			}
			break;
		default:
			break;
		}
	}
	return nLen;
}

//Rcvdata
sys_res nrsec3000_RcvData(p_nrsec3000 p,u8 *rbuf,u8 rbufLen)
{
    /* receive data and crc */
	int i;
	sys_res res = SYS_R_ERR;
	
	for (i = 0; i < rbufLen; i++)
	{
		res = spi_TransChar(p->spi, TR_INS, &rbuf[i]); 
		if(res != SYS_R_OK)
			break;
	}
	return res;
}

//RcvSW
sys_res nrsec3000_RcvSW(p_nrsec3000 p,u8 *rbuf,int cnt)
{
//	int cnt = 1000;
	
	/* receive state word */
	for(; cnt > 0; cnt--)
	{
		spi_TransChar(p->spi, TR_INS, &rbuf[0]);
		if (rbuf[0] == 0x90)
		{
			spi_TransChar(p->spi, TR_INS, &rbuf[0]);
			if (rbuf[0] == 0x00)
				return SYS_R_OK;
		}
	}
	return SYS_R_ERR;
}


//SAM1 导入对称密钥key
sys_res nrsec3000_SM1ImportKey(p_nrsec3000 p,u8 *key)
{
	u8 tbuf[32];
    u8 rbuf[32];
	u8 nCRC;
	sys_res res;
	nrsec3000_cmd sm1Imkey = {0x80, 0xd4, 0x01, 0x00, 0x10};

	p->spi = nrsec3000_SpiGet();
	memset(tbuf, 0, sizeof(tbuf));
	memset(rbuf, 0, sizeof(rbuf));

	nrsec3000_SendCmd(p,sm1Imkey,5);
	res = nrsec3000_RcvINS(p,rbuf,sm1Imkey->ins,200);
	if (res == SYS_R_OK)
	{
		tbuf[0] = 0x55;
		nrsec3000_SendCmd(p,tbuf,1);
		nCRC = get_crc7(key,sm1Imkey->p3);
		nrsec3000_SendCmd(p,key,sm1Imkey->p3);
		nrsec3000_SendCmd(p,&nCRC,1);
		res = nrsec3000_RcvSW(p,rbuf,1000);
		if (res == SYS_R_OK)
		{
			sm1Imkey->p1 = 0x02;
			nrsec3000_SendCmd(p,sm1Imkey,5);
			nrsec3000_RcvINS(p,rbuf,sm1Imkey->ins,200);
			tbuf[0] = 0x55;
			nrsec3000_SendCmd(p,tbuf,1);
			memset(tbuf, 0, sizeof(tbuf));
			nCRC = get_crc7(tbuf,sm1Imkey->p3);
			nrsec3000_SendCmd(p,tbuf,sm1Imkey->p3);
			nrsec3000_SendCmd(p,&nCRC,1);
			tbuf[0] = 0xaa;
			nrsec3000_SendCmd(p,tbuf,1);
			res = nrsec3000_RcvSW(p,rbuf,1000);
		}
		
	}
	spi_Close(p->spi);
	return res;
}
//SAM1 导入初始向量 IV
sys_res nrsec3000_SM1ImportIV(p_nrsec3000 p,u8 *IV)
{
	u8 tbuf[32];
    u8 rbuf[32];
	u8 nCRC;
	sys_res res;
	nrsec3000_cmd sm1ImIV = {0x80, 0xd4, 0x04, 0x00, 0x10};

	p->spi = nrsec3000_SpiGet();
	memset(tbuf, 0, sizeof(tbuf));
	memset(rbuf, 0, sizeof(rbuf));

	nrsec3000_SendCmd(p,sm1ImIV,5);
	tbuf[0] = 0xaa;
	res = nrsec3000_RcvINS(p,rbuf,0xd4,200);
	if (res == SYS_R_OK)
	{
		tbuf[0] = 0x55;
		nrsec3000_SendCmd(p,tbuf,1);//数据起始
		nCRC = get_crc7(IV,sm1ImIV->p3);
		nrsec3000_SendCmd(p,IV,sm1ImIV->p3);
		nrsec3000_SendCmd(p,&nCRC,1);
		tbuf[0] = 0xaa;
		nrsec3000_SendCmd(p,tbuf,1);//数据结束
		res = nrsec3000_RcvSW(p,rbuf,1000);
	}
	spi_Close(p->spi);
	return res;
}

//SM1加密
sys_res nrsec3000_SM1Encrypt(p_nrsec3000 p,u8 *buf,u16 bufLen,u8 *Enbuf)
{
	u8 tbuf[32];
	u8 rbuf[32];
	u8 nCRC,nLen;
	sys_res res;
	nrsec3000_cmd sm1Endata = {0xa0, 0xe0, 0x80, 0x00, 0x00};

	p->spi = nrsec3000_SpiGet();
	memset(tbuf, 0, sizeof(tbuf));
	memset(rbuf, 0, sizeof(rbuf));

	sm1Endata->p2 = bufLen<<4;
	sm1Endata->p3 = bufLen&0xff;

	nrsec3000_SendCmd(p,sm1Endata,5);
	res = nrsec3000_RcvINS(p,rbuf,sm1Endata->ins,200);
	if (res == SYS_R_OK)
	{
		tbuf[0] = 0x55;
		nrsec3000_SendCmd(p,tbuf,1);
		nCRC = get_crc7(buf,bufLen);
		nrsec3000_SendCmd(p,buf,bufLen);
		nrsec3000_SendCmd(p,&nCRC,1);
		tbuf[0] = 0xaa;
		nrsec3000_SendCmd(p,tbuf,1);
		res = nrsec3000_RcvINS(p,rbuf,sm1Endata->ins,10000);
		if (res == SYS_R_OK)
		{
			nLen = nrsec3000_RcvLEN(p,2,rbuf);
			nrsec3000_RcvData(p,Enbuf,nLen-1);
			nrsec3000_RcvData(p,rbuf,1);
			nCRC = get_crc7(Enbuf,nLen-1);
			if(nCRC == rbuf[0] )
				res = nrsec3000_RcvSW(p,rbuf,1000);
			else
				res = SYS_R_ERR;
		}
	}
	spi_Close(p->spi);
	return res;
}

//SM1解密
sys_res nrsec3000_SM1Decrypt(p_nrsec3000 p,u8 *buf,u16 bufLen,u8 *Debuf)
{
	u8 tbuf[32];
	u8 rbuf[32];
	u8 nCRC,nLen;
	sys_res res;
	nrsec3000_cmd sm1Dedata = {0xa0, 0xe0, 0x81, 0x00, 0x00};

	p->spi = nrsec3000_SpiGet();
	memset(tbuf, 0, sizeof(tbuf));
	memset(rbuf, 0, sizeof(rbuf));

	sm1Dedata->p2 = bufLen<<4;
	sm1Dedata->p3 = bufLen&0xff;

	nrsec3000_SendCmd(p,sm1Dedata,5);
	res = nrsec3000_RcvINS(p,rbuf,sm1Dedata->ins,200);
	if(res==SYS_R_OK){
		tbuf[0] = 0x55;
		nrsec3000_SendCmd(p,tbuf,1);
		nCRC = get_crc7(buf,bufLen);
		nrsec3000_SendCmd(p,buf,bufLen);
		nrsec3000_SendCmd(p,&nCRC,1);
		tbuf[0] = 0xaa;
		res = nrsec3000_RcvINS(p,rbuf,sm1Dedata->ins,10000);
		if(res==SYS_R_OK){
			nLen = nrsec3000_RcvLEN(p,2,rbuf);
			nrsec3000_RcvData(p,Debuf,nLen-1);
			nrsec3000_RcvData(p,rbuf,1);
			nCRC = get_crc7(Debuf,nLen-1);
			if(nCRC == rbuf[0] )
				res = nrsec3000_RcvSW(p,rbuf,1000);
			else
				res = SYS_R_ERR;
		}
	}
	spi_Close(p->spi);
	return res;
}

//产生 SM2 密钥对
sys_res nrsec3000_SM2NewKey(p_nrsec3000 p,u8 keyNo)
{
    u8 rbuf[32];
	sys_res res;
	nrsec3000_cmd sm2Newkey = {0x80, 0xb2, 0x00, 0x00, 0x00};

	p->spi = nrsec3000_SpiGet();
	memset(rbuf, 0, sizeof(rbuf));
	sm2Newkey->p2 = keyNo;//p2密钥索引号

	nrsec3000_SendCmd(p,sm2Newkey,5);
	res = nrsec3000_RcvSW(p,rbuf,10000);
	spi_Close(p->spi);
	return res;
}
//导出 SM2 公钥
sys_res nrsec3000_SM2ExPubKey(p_nrsec3000 p,u8 *key,u8 keyNo)
{
    u8 rbuf[32];
	sys_res res;
	u8 nCRC,nLen;
	nrsec3000_cmd cmd_sm2ExPubKey = {0x80, 0xb8, 0x01, 0x00, 0x40};
	cmd_sm2ExPubKey->p2 = keyNo;

	p->spi = nrsec3000_SpiGet();
	memset(rbuf, 0, sizeof(rbuf));

	nrsec3000_SendCmd(p,cmd_sm2ExPubKey,5);
	res = nrsec3000_RcvINS(p,rbuf,cmd_sm2ExPubKey->ins,200);
	if(res==SYS_R_OK){
		nLen = nrsec3000_RcvLEN(p,1,rbuf);
		nrsec3000_RcvData(p,key,nLen-1);
		nrsec3000_RcvData(p,rbuf,1);
		nCRC = get_crc7(key,nLen-1);
		if(nCRC == rbuf[0] )
			res = nrsec3000_RcvSW(p,rbuf,1000);
		else
			res = SYS_R_ERR;
	}
	spi_Close(p->spi);
	return res;
}
//导出 SM2 私钥
sys_res nrsec3000_SM2ExPrvKey(p_nrsec3000 p,u8 *key,u8 keyNo)
{
    u8 rbuf[32];
	u8 nCRC,nLen;
	sys_res res;
	nrsec3000_cmd cmd_sm2ExPrvKey = {0x80, 0xb8, 0x02, 0x00, 0x20};
	cmd_sm2ExPrvKey->p2 = keyNo;

	p->spi = nrsec3000_SpiGet();
	memset(rbuf, 0, sizeof(rbuf));

	nrsec3000_SendCmd(p,cmd_sm2ExPrvKey,5);
	res = nrsec3000_RcvINS(p,rbuf,cmd_sm2ExPrvKey->ins,200);
	if(res==SYS_R_OK){
		nLen = nrsec3000_RcvLEN(p,1,rbuf);
		nrsec3000_RcvData(p,key,nLen-1);
		nrsec3000_RcvData(p,rbuf,1);
		nCRC = get_crc7(key,nLen-1);
		if(nCRC == rbuf[0] )
			res = nrsec3000_RcvSW(p,rbuf,1000);
		else
			res = SYS_R_ERR;
	}
	spi_Close(p->spi);
	return res;
}
//导入 SM2 公钥
sys_res nrsec3000_SM2ImPubKey(p_nrsec3000 p,u8 *key,u8 keyNo)
{
   	u8 tbuf[32];
    u8 rbuf[32];
	u8 nCRC;
	sys_res res;
	nrsec3000_cmd cmd_sm2ImPubKey = {0x80, 0xba, 0x01, 0x00, 0x40};

	cmd_sm2ImPubKey->p2 = keyNo;
	p->spi = nrsec3000_SpiGet();
	memset(tbuf, 0, sizeof(tbuf));
	memset(rbuf, 0, sizeof(rbuf));

	nrsec3000_SendCmd(p,cmd_sm2ImPubKey,5);
	res = nrsec3000_RcvINS(p,rbuf,cmd_sm2ImPubKey->ins,200);
	if(res==SYS_R_OK){
		tbuf[0] = 0x55;//数据发送起始
		nrsec3000_SendCmd(p,tbuf,1);
		nCRC = get_crc7(key,cmd_sm2ImPubKey->p3);
		nrsec3000_SendCmd(p,key,cmd_sm2ImPubKey->p3);
		nrsec3000_SendCmd(p,&nCRC,1);
		tbuf[0] = 0xaa;//数据发送结束
		nrsec3000_SendCmd(p,tbuf,1);
		res = nrsec3000_RcvSW(p,rbuf,1000);
	}
	spi_Close(p->spi);
	return res;
}

//导入 SM2 私钥
sys_res nrsec3000_SM2ImPrvKey(p_nrsec3000 p,u8 *key,u8 keyNo)
{
   	u8 tbuf[32];
    u8 rbuf[32];
	u8 nCRC;
	sys_res res;
	nrsec3000_cmd cmd_sm2ImPrvKey = {0x80, 0xba, 0x02, 0x00, 0x20};

	cmd_sm2ImPrvKey->p2 = keyNo;
	p->spi = nrsec3000_SpiGet();
	memset(tbuf, 0, sizeof(tbuf));
	memset(rbuf, 0, sizeof(rbuf));

	nrsec3000_SendCmd(p,cmd_sm2ImPrvKey,5);
	res = nrsec3000_RcvINS(p,rbuf,cmd_sm2ImPrvKey->ins,200);
	if(res==SYS_R_OK){
		tbuf[0] = 0x55;//数据发送起始
		nrsec3000_SendCmd(p,tbuf,1);
		nCRC = get_crc7(key,cmd_sm2ImPrvKey->p3);
		nrsec3000_SendCmd(p,key,cmd_sm2ImPrvKey->p3);
		nrsec3000_SendCmd(p,&nCRC,1);
		tbuf[0] = 0xaa;//数据发送结束
		nrsec3000_SendCmd(p,tbuf,1);
		res = nrsec3000_RcvSW(p,rbuf,1000);
	}
	spi_Close(p->spi);
	return res;
}
//SM3 Hash基础运算
sys_res nrsec3000_Hash(p_nrsec3000 p,u8 *buf,u16 bufLen,u8 *hash)
{
	u8 tbuf[32];
	u8 rbuf[32];
	u8 nCRC,nLen;
	sys_res res;
	nrsec3000_cmd sm3Hash = {0x80, 0xb5, 0x00, 0x00, 0x00};

	p->spi = nrsec3000_SpiGet();
	memset(tbuf, 0, sizeof(tbuf));
	memset(rbuf, 0, sizeof(rbuf));

	sm3Hash->p2 = bufLen<<4;
	sm3Hash->p3 = bufLen&0xff;

	nrsec3000_SendCmd(p,sm3Hash,5);
	tbuf[0] = 0xaa;
	res = nrsec3000_RcvINS(p,rbuf,sm3Hash->ins,200);
	if(res==SYS_R_OK){
		tbuf[0] = 0x55;//起始标识
		nrsec3000_SendCmd(p,tbuf,1);
		nCRC = get_crc7(buf,bufLen);
		nrsec3000_SendCmd(p,buf,bufLen);
		nrsec3000_SendCmd(p,&nCRC,1);
		tbuf[0] = 0xaa;//结束标识
		nrsec3000_SendCmd(p,tbuf,1);
		res = nrsec3000_RcvINS(p,rbuf,sm3Hash->ins,200);
		if(res==SYS_R_OK){
			nLen = nrsec3000_RcvLEN(p,1,rbuf);
			nrsec3000_RcvData(p,hash,nLen-1);
			nrsec3000_RcvData(p,rbuf,1);
			nCRC = get_crc7(hash,nLen-1);
			if(nCRC == rbuf[0] )
				res = nrsec3000_RcvSW(p,rbuf,1000);
			else
				res = SYS_R_ERR;
		}
	}
	spi_Close(p->spi);
	return res;
}
//SM3 Hash运算并附带功能
sys_res nrsec3000_SM3(p_nrsec3000 p,u8 *buf,u16 bufLen,u8 *hash,
								u8 *pubkey,u8 *pucID,size_t idLen)
{
	size_t nLen, nELen;
	sys_res res;
	u8 tbuf[1024];
	u8 rbuf[32];
	u8 abxy[32 * 4] = {
		0xFF,0xFF,0xFF,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	/* a */
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFC,
		0x28,0xE9,0xFA,0x9E,0x9D,0x9F,0x5E,0x34,0x4D,0x5A,	/* b */
		0x9E,0x4B,0xCF,0x65,0x09,0xA7,0xF3,0x97,0x89,0xF5,
		0x15,0xAB,0x8F,0x92,0xDD,0xBC,0xBD,0x41,0x4D,0x94,
		0x0E,0x93,
		0x32,0xC4,0xAE,0x2C,0x1F,0x19,0x81,0x19,0x5F,0x99,	/* x */
		0x04,0x46,0x6A,0x39,0xC9,0x94,0x8F,0xE3,0x0B,0xBF,
		0xF2,0x66,0x0B,0xE1,0x71,0x5A,0x45,0x89,0x33,0x4C,
		0x74,0xC7,
		0xBC,0x37,0x36,0xA2,0xF4,0xF6,0x77,0x9C,0x59,0xBD,	/* y */ 
		0xCE,0xE3,0x6B,0x69,0x21,0x53,0xD0,0xA9,0x87,0x7C,
		0xC6,0x2A,0x47,0x40,0x02,0xDF,0x32,0xE5,0x21,0x39,
		0xF0,0xA0
	};
	nLen = 2 + idLen + 32 * 6;
	nELen = idLen * 8;
	memcpy(tbuf, &nELen, 2);
	memcpy(&tbuf[2], pucID, idLen);
	memcpy(&tbuf[2 + idLen], abxy, 32 *4);
	memcpy(&tbuf[2 + idLen + 4 * 32], pubkey, 32);
	memcpy(&tbuf[2 + idLen + 5 * 32], pubkey+32, 32);
	res = nrsec3000_Hash(p,tbuf,bufLen,rbuf);
	if (res != SYS_R_OK)
		return res;
	nLen = bufLen + 32;
	memcpy(tbuf,rbuf,32);
	memcpy(&tbuf[32], buf, bufLen);
	res = nrsec3000_Hash(p,tbuf,nLen,hash);
	return res;
}

//SM2签名SM2 Sign 
sys_res nrsec3000_SM2Sign(p_nrsec3000 p,u8 *buf,u8 keyNo,u8 *sign)
{
	u8 tbuf[32];
	u8 rbuf[32];
	u8 nCRC,nLen;
	sys_res res;
	nrsec3000_cmd sm2Sign = {0x80, 0xb4, 0x00, 0x00, 0x20};

	p->spi = nrsec3000_SpiGet();
	memset(tbuf, 0, sizeof(tbuf));
	memset(rbuf, 0, sizeof(rbuf));
	sm2Sign->p2 = keyNo;

	nrsec3000_SendCmd(p,sm2Sign,5);
	tbuf[0] = 0xaa;
	res = nrsec3000_RcvINS(p,rbuf,sm2Sign->ins,200);
	if(res==SYS_R_OK){
		tbuf[0] = 0x55;//起始标识
		nrsec3000_SendCmd(p,tbuf,1);
		nCRC = get_crc7(buf,sm2Sign->p3);
		nrsec3000_SendCmd(p,buf,sm2Sign->p3);
		nrsec3000_SendCmd(p,&nCRC,1);
		tbuf[0] = 0xaa;//结束标识
		nrsec3000_SendCmd(p,tbuf,1);
		res = nrsec3000_RcvINS(p,rbuf,sm2Sign->ins,10000);
		if(res==SYS_R_OK){
			nLen = nrsec3000_RcvLEN(p,1,rbuf);
			nrsec3000_RcvData(p,sign,nLen-1);
			nrsec3000_RcvData(p,rbuf,1);
			nCRC = get_crc7(sign,nLen-1);
			if(nCRC == rbuf[0] )
				res = nrsec3000_RcvSW(p,rbuf,1000);
			else
				res = SYS_R_ERR;
		}
	}
	spi_Close(p->spi);
	return res;
}

//SM2验签 SM2 CheckSign
sys_res nrsec3000_SM2CheckSign(p_nrsec3000 p,u8 *hash,u8 *sign,u8 keyNo)
{
	u8 tbuf[96];
	u8 rbuf[32];
	u8 nCRC;
	sys_res res;
	nrsec3000_cmd sm2CheckSign = {0x80, 0xb6, 0x00, 0x00, 0x60};

	p->spi = nrsec3000_SpiGet();
	memset(tbuf, 0, sizeof(tbuf));
	memset(rbuf, 0, sizeof(rbuf));
	sm2CheckSign->p2 = keyNo;

	nrsec3000_SendCmd(p,sm2CheckSign,5);
	tbuf[0] = 0xaa;
	res = nrsec3000_RcvINS(p,rbuf,sm2CheckSign->ins,200);
	if(res==SYS_R_OK){
		tbuf[0] = 0x55;//起始标识
		nrsec3000_SendCmd(p,tbuf,1);
		memcpy(tbuf,hash,32);
		memcpy(tbuf,sign,64);
		nCRC = get_crc7(tbuf,sm2CheckSign->p3);
		nrsec3000_SendCmd(p,hash,0x20);
		nrsec3000_SendCmd(p,sign,0x40);
		nrsec3000_SendCmd(p,&nCRC,1);
		tbuf[0] = 0xaa;//结束标识
		nrsec3000_SendCmd(p,tbuf,1);
		os_thd_sleep(2000);
		res = nrsec3000_RcvSW(p,rbuf,20000);
	}
	spi_Close(p->spi);
	return res;
}

//SM2加密
sys_res nrsec3000_SM2Encrypt(p_nrsec3000 p,u8 *buf,u8 keyNo,u8 *Enbuf)
{
	u8 tbuf[32];
	u8 rbuf[32];
	u8 nCRC,nLen;
	sys_res res;
	nrsec3000_cmd sm2Endata = {0x80, 0xb3, 0x01, 0x00, 0x20};

	p->spi = nrsec3000_SpiGet();
	memset(tbuf, 0, sizeof(tbuf));
	memset(rbuf, 0, sizeof(rbuf));

	sm2Endata->p2 = keyNo;
	nrsec3000_SendCmd(p,sm2Endata,5);
	tbuf[0] = 0xaa;
	res = nrsec3000_RcvINS(p,tbuf,sm2Endata->ins,200);
	if(res==SYS_R_OK){
		tbuf[0] = 0x55;
		nrsec3000_SendCmd(p,tbuf,1);
		nCRC = get_crc7(buf,sm2Endata->p3);
		nrsec3000_SendCmd(p,buf,sm2Endata->p3);
		nrsec3000_SendCmd(p,&nCRC,1);
		tbuf[0] = 0xaa;
		nrsec3000_SendCmd(p,tbuf,1);
		os_thd_sleep(2000);
		res = nrsec3000_RcvINS(p,rbuf,sm2Endata->ins,20000);
		if(res==SYS_R_OK){
			nLen = nrsec3000_RcvLEN(p,1,rbuf);
			nrsec3000_RcvData(p,Enbuf,nLen-1);
			nrsec3000_RcvData(p,rbuf,1);
			nCRC = get_crc7(Enbuf,nLen-1);
			if(nCRC == rbuf[0] )
				res = nrsec3000_RcvSW(p,rbuf,1000);
			else
				res = SYS_R_ERR;
		}
	}
	spi_Close(p->spi);
	return res;
}

//SM2解密
sys_res nrsec3000_SM2Decrypt(p_nrsec3000 p,u8 *buf,u8 keyNo,u8 *Debuf)
{
	u8 tbuf[32];
	u8 rbuf[32];
	u8 nCRC,nLen;
	sys_res res;
	nrsec3000_cmd sm2Dedata = {0x80, 0xb3, 0x81, 0x00, 0x80};

	p->spi = nrsec3000_SpiGet();
	memset(tbuf, 0, sizeof(tbuf));
	memset(rbuf, 0, sizeof(rbuf));

	sm2Dedata->p2 = keyNo;
	nrsec3000_SendCmd(p,sm2Dedata,5);
	res = nrsec3000_RcvINS(p,rbuf,sm2Dedata->ins,200);
	if(res==SYS_R_OK){
		tbuf[0] = 0x55;
		nrsec3000_SendCmd(p,tbuf,1);
		nCRC = get_crc7(buf,sm2Dedata->p3);
		nrsec3000_SendCmd(p,buf,sm2Dedata->p3);
		nrsec3000_SendCmd(p,&nCRC,1);
		tbuf[0] = 0xaa;
		nrsec3000_SendCmd(p,tbuf,1);
		os_thd_sleep(2000);
		res = nrsec3000_RcvINS(p,rbuf,sm2Dedata->ins,20000);
		if(res==SYS_R_OK){
			nLen = nrsec3000_RcvLEN(p,1,rbuf);
			nrsec3000_RcvData(p,Debuf,nLen-1);
			nrsec3000_RcvData(p,rbuf,1);
			nCRC = get_crc7(Debuf,nLen-1);
			if(nCRC == rbuf[0] )
				res = nrsec3000_RcvSW(p,rbuf,1000);
			else
				res = SYS_R_ERR;
		}
	}
	spi_Close(p->spi);
	return res;
}

//生成 SM2 证书请求
sys_res nrsec3000_SM2Credentials(p_nrsec3000 p,u8 type,u8 keyNo,u8 *buf,u16 bufLen,u8 *Crebuf)
{
	u8 tbuf[32];
	u8 rbuf[32];
	u8 nCRC,nLen;
	sys_res res;
	nrsec3000_cmd sm2Credentials = {0x80, 0xb7, 0x00, 0x00, 0x00};

	p->spi = nrsec3000_SpiGet();
	memset(tbuf, 0, sizeof(tbuf));
	memset(rbuf, 0, sizeof(rbuf));
	sm2Credentials->p1 = type;
	sm2Credentials->p2 = keyNo;
	sm2Credentials->p3 = bufLen;

	nrsec3000_SendCmd(p,sm2Credentials,5);
	res = nrsec3000_RcvINS(p,rbuf,sm2Credentials->ins,200);
	if(res==SYS_R_OK){
		tbuf[0] = 0x55;
		nrsec3000_SendCmd(p,tbuf,1);
		nCRC = get_crc7(buf,sm2Credentials->p3);
		nrsec3000_SendCmd(p,buf,sm2Credentials->p3);
		nrsec3000_SendCmd(p,&nCRC,1);
		tbuf[0] = 0xaa;
		nrsec3000_SendCmd(p,tbuf,1);
		os_thd_sleep(5000);//等待运算结束
		res = nrsec3000_RcvINS(p,rbuf,sm2Credentials->ins,20000);
		if(res==SYS_R_OK){
			nLen = nrsec3000_RcvLEN(p,2,rbuf);
			nrsec3000_RcvData(p,Crebuf,nLen-1);
			nrsec3000_RcvData(p,rbuf,1);
			nCRC = get_crc7(Crebuf,nLen-1);
			if(nCRC == rbuf[0] )
				res = nrsec3000_RcvSW(p,rbuf,1000);
			else
				res = SYS_R_ERR;
		}
	}
	spi_Close(p->spi);
	return res;
}

//获取版本
sys_res nrsec3000_GetVer(p_nrsec3000 p,u8 *ver)
{
    u8 rbuf[32];
	u8 nLen,nCRC;
	sys_res res;
	nrsec3000_cmd cmd_getVer = {0x00, 0x5B, 0x00, 0x00, 0x40};

	p->spi = nrsec3000_SpiGet();
	memset(rbuf, 0, sizeof(rbuf));

	nrsec3000_SendCmd(p,cmd_getVer,5);
	res = nrsec3000_RcvINS(p,rbuf,cmd_getVer->ins,200);
	if(res==SYS_R_OK){
		nLen = nrsec3000_RcvLEN(p,1,rbuf);
		nrsec3000_RcvData(p,ver,nLen-1);
		nrsec3000_RcvData(p,rbuf,1);
		nCRC = get_crc7(ver,nLen-1);
		if(nCRC == rbuf[0] )
			res = nrsec3000_RcvSW(p,rbuf,1000);
		else
			res = SYS_R_ERR;
	}
	spi_Close(p->spi);
	return res;
}

//获取随机数
sys_res nrsec3000_GetRandom(p_nrsec3000 p,u8 *random,u8 len)
{
    u8 rbuf[32];
	u8 nLen,nCRC;
	sys_res res;
	nrsec3000_cmd cmd_getRandom = {0x00, 0x84, 0x00, 0x00, 0x00};
	
	cmd_getRandom->p3 = len;
	p->spi = nrsec3000_SpiGet();
	memset(rbuf, 0, sizeof(rbuf));

	nrsec3000_SendCmd(p,cmd_getRandom,5);
	res = nrsec3000_RcvINS(p,rbuf,cmd_getRandom->ins,200);
	if(res==SYS_R_OK){
		nLen = nrsec3000_RcvLEN(p,1,rbuf);
		nrsec3000_RcvData(p,random,nLen-1);
		nrsec3000_RcvData(p,rbuf,1);
		nCRC = get_crc7(random,nLen-1);
		if(nCRC == rbuf[0] )
			res = nrsec3000_RcvSW(p,rbuf,1000);
		else
			res = SYS_R_ERR;
	}
	spi_Close(p->spi);
	return res;
}

//安全认证
sys_res nrsec3000_SafetyCre(p_nrsec3000 p,u8 *buf,u8 *safetybuf)
{
	u8 tbuf[32];
	u8 rbuf[32];
	u8 nCRC,nLen;
	sys_res res = SYS_R_ERR;
	nrsec3000_cmd safetyCre = {0x80, 0xb3, 0x01, 0x04, 0x20};

	p->spi = nrsec3000_SpiGet();
	memset(tbuf, 0, sizeof(tbuf));
	memset(rbuf, 0, sizeof(rbuf));

	nrsec3000_SendCmd(p,safetyCre,5);
	tbuf[0] = 0xaa;
	res = nrsec3000_RcvINS(p,tbuf,safetyCre->ins,200);
	if(res==SYS_R_OK){
		tbuf[0] = 0x55;
		nrsec3000_SendCmd(p,tbuf,1);
		nCRC = get_crc7(buf,safetyCre->p3);
		nrsec3000_SendCmd(p,buf,safetyCre->p3);
		nrsec3000_SendCmd(p,&nCRC,1);
		tbuf[0] = 0xaa;
		nrsec3000_SendCmd(p,tbuf,1);
		os_thd_sleep(2000);//等待运算结束
		res = nrsec3000_RcvINS(p,rbuf,safetyCre->ins,20000);
		if(res==SYS_R_OK){
			nLen = nrsec3000_RcvLEN(p,1,rbuf);
			nrsec3000_RcvData(p,safetybuf,nLen-1);
			nrsec3000_RcvData(p,rbuf,1);
			nCRC = get_crc7(safetybuf,nLen-1);
			if(nCRC == rbuf[0] )
				res = nrsec3000_RcvSW(p,rbuf,1000);
			else
				res = SYS_R_ERR;
		}
	}
	spi_Close(p->spi);
	return res;
}

#endif

