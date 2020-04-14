
//Private Defines
#define GDFTS_MAGIC_INIT			0xFFFF0000

#define GDFTS_CODE_TRANS			0x04
#define GDFTS_CODE_ABORT			0x0D
#define GDFTS_CODE_INIT				0x0F

#define GDFTS_CODE_OK				0x80
#define GDFTS_CODE_ERR				0xB0


//Private Typedef
struct gdfts_header
{
	u8	pwd[8];
	u16	id;
	u8	restart;
	u16	cnt;
	u16	len;
} PACK_STRUCT_STRUCT;
typedef struct gdfts_header *p_gdfts_header;

typedef struct {
	u16	cnt;
	u16	crc;
	u32	reced;
	u32	len;
}gdfts[1];


//Private const variables
static const u8 gdfts_Pwd[] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11};


static sys_res gdfts_Response(buf b, int nCode, p_gdfts_header pH, gdfts pFts)
{

	buf_PushData(b, nCode, 1);
	buf_PushData(b, pH->id, 2);
	buf_PushData(b, pH->cnt, 2);
	buf_PushData(b, pFts->reced, 4);
	
	if (pFts->reced)
		buf_PushData(b, crc16((u8 *)upd_DataDev.tbl[0].start, pFts->reced), 2);
	else
		buf_PushData(b, 0, 2);
	
	buf_PushData(b, crc16(b->p, b->len), 2);
	
	return SYS_R_OK;
}

static sys_res gdfts_DataFormat()
{
	int i, nDev = upd_DataDev.dev;
	t_flash_blk *pBlk = &upd_DataDev.tbl[0];

	for (i = upd_DataDev.blk; i; i--, pBlk++)
	{
		if (flash_Erase(nDev, pBlk->start) != SYS_R_OK)
			break;
	}
	
	if (i)
		return SYS_R_TMO;
	
	return SYS_R_OK;
}



sys_res gdfts_Init()
{
	sys_res res;

	if (sfs_Read(upd_SfsDev, GDFTS_MAGIC_INIT, NULL, 0) > 0)
		return SYS_R_OK;
	
	if ((res = sfs_Init(upd_SfsDev)) != SYS_R_OK)
		return res;
	
	return sfs_Write(upd_SfsDev, GDFTS_MAGIC_INIT, NULL, 0);
}

sys_res gdfts_Handler(buf bIn, buf bOut)
{
	sys_res res = SYS_R_ERR;
	int nCode, nIsReset = 0;
	u8 *pData;
	adr_t adr = upd_DataDev.tbl[0].start;
	gdfts pFts = {0};
	p_gdfts_header pH;
	os_que que;

	if (bOut->len)
		buf_Release(bOut);
	
	nCode = bIn->p[0] & 0x3F;
	pH = (p_gdfts_header)&bIn->p[1];
	
	if (pH->id != GDFTS_ID_SELF)
	{
		//Transceiver
		if (os_que_Send(QUE_EVT_GDFTS_TRANS, NULL, bIn->p, bIn->len, GDFTS_TRANS_TMO) == SYS_R_OK)
		{
			if ((que = os_que_Wait(QUE_EVT_GDFTS_RESPOND, NULL, GDFTS_TRANS_TMO)) != NULL)
			{
				buf_Push(bOut, que->data->b->p, que->data->b->len);
				os_que_Release(que);
				res = SYS_R_OK;
			}
		}
		
		if (res != SYS_R_OK)
		{
			buf_PushData(bOut, nCode | GDFTS_CODE_ERR, 1);
			buf_PushData(bOut, crc16(bOut->p, bOut->len), 2);
		}
		
		return SYS_R_OK;
	}
	switch (nCode)
	{
	case GDFTS_CODE_TRANS:
		if (memcmp(gdfts_Pwd, pH->pwd, sizeof(gdfts_Pwd)) == 0)
		{
			pData = (u8 *)(pH + 1);
			if (pH->cnt)
			{
				if (sfs_Read(upd_SfsDev, pH->id, pFts) > 0)
				{
					if (pFts->cnt == pH->cnt)
					{
						if (flash_Program(upd_DataDev.dev, adr + IAP_HEADER_SIZE + pFts->reced, pData, pH->len) == SYS_R_OK)
						{
							pFts->cnt += 1;
							pFts->reced += pH->len;
							
							if (pFts->reced < pFts->len)
							{
								sfs_Write(upd_SfsDev, pH->id, pFts, sizeof(gdfts));
								
								res = SYS_R_OK;
							}
							else if (pFts->crc == crc16((u8 *)(adr + IAP_HEADER_SIZE), pFts->len))
							{
								t_iap xIap = {0};
								
								xIap.magic1 = IAP_MAGICWORD_1;
								xIap.magic2 = IAP_MAGICWORD_2;
								xIap.dest = INTFLASH_ADR_BASE + BOOTLOADER_SIZE;
								xIap.len = pFts->len;
								xIap.id = pH->id;
								xIap.crc = pFts->crc;
								
								sfs_Delete(upd_SfsDev, pH->id);
								if (flash_Program(upd_DataDev.dev, adr, &xIap, sizeof(xIap)) == SYS_R_OK)
								{
									nIsReset = 1;
									
									res = SYS_R_OK;
								}
							}
						}
					}
				}
			}
			else
			{
				res = SYS_R_OK;
				if ((pH->restart) || (sfs_Read(upd_SfsDev, pH->id, pFts) < 0))
				{
					gdfts_DataFormat();
					
					pFts->cnt = 1;
					pFts->reced = 0;
					memcpy(&pFts->len, pData, sizeof(pFts->len));
					memcpy(&pFts->crc, pData + sizeof(pFts->len), sizeof(pFts->crc));
					
					res = sfs_Write(upd_SfsDev, pH->id, pFts, sizeof(gdfts));
				}
			}
		}
		
		if (res == SYS_R_OK)
			gdfts_Response(bOut, nCode | GDFTS_CODE_OK, pH, pFts);
		else
			gdfts_Response(bOut, nCode | GDFTS_CODE_ERR, pH, pFts);
		
		break;
		
	case GDFTS_CODE_ABORT:
		if (memcmp(gdfts_Pwd, pH->pwd, sizeof(gdfts_Pwd)) == 0)
			res = sfs_Delete(upd_SfsDev, pH->id);
		
		if (res == SYS_R_OK)
			nCode |= GDFTS_CODE_OK;
		else
			nCode |= GDFTS_CODE_ERR;
		
		buf_PushData(bOut, nCode, 1);
		buf_PushData(bOut, pH->id, 2);
		buf_PushData(bOut, crc16(bOut->p, bOut->len), 2);
		break;
		
	case GDFTS_CODE_INIT:
		if (memcmp(gdfts_Pwd, pH->pwd, sizeof(gdfts_Pwd)) == 0)
		{
			if ((res = gdfts_DataFormat()) == SYS_R_OK)
			{
				if ((res = sfs_Init(upd_SfsDev)) == SYS_R_OK)
					res = sfs_Write(upd_SfsDev, GDFTS_MAGIC_INIT, NULL, 0);
			}
		}
		
		if (res == SYS_R_OK)
			nCode |= GDFTS_CODE_OK;
		else
			nCode |= GDFTS_CODE_ERR;

		buf_PushData(bOut, nCode, 1);
		buf_PushData(bOut, pH->id, 2);
		buf_PushData(bOut, crc16(bOut->p, bOut->len), 2);
		break;

	default:
		break;
	}
	
	if (nIsReset)
		return SYS_R_RESET;
	
	return SYS_R_OK;
}


