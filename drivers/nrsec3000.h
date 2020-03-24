#ifndef __NRSEC3000_H__
#define __NRSEC3000_H__


#ifdef __cplusplus
extern "C" {
#endif

//Public Typedefs
typedef struct {
	uint		ste;
	p_dev_spi	spi;
} t_nrsec3000, *p_nrsec3000;

typedef struct {
	u8 cla;
	u8 ins;
	u8 p1;
	u8 p2;
    u8 p3;
} nrsec3000_cmd[1], *p_nrsec3000_cmd;

void nrsec3000_Init(void);
sys_res nrsec3000_SendCmd(p_nrsec3000 p,const void  *cmd,u8 cmdLen);
sys_res nrsec3000_RcvINS(p_nrsec3000 p,u8 *rbuf, u8 ins,int cnt);
u16 nrsec3000_RcvLEN(p_nrsec3000 p, size_t tbufLen,u8 *rbuf);
sys_res nrsec3000_RcvData(p_nrsec3000 p, u8 *rbuf,u8 rbufLen);
sys_res nrsec3000_RcvSW(p_nrsec3000 p, u8 *rbuf,int cnt);
sys_res nrsec3000_Sam1ImportKey(p_nrsec3000 p,u8 *key);
sys_res nrsec3000_Sam1ImportIV(p_nrsec3000 p,u8 *IV);
sys_res nrsec3000_SM1Encrypt(p_nrsec3000 p,u8 *buf,u16 bufLen,u8 *Enbuf);
sys_res nrsec3000_SM1Decrypt(p_nrsec3000 p,u8 *buf,u16 bufLen,u8 *Debuf);
sys_res nrsec3000_SM2NewKey(p_nrsec3000 p,u8 keyNo);
sys_res nrsec3000_SM2ExPubKey(p_nrsec3000 p,u8 *key,u8 keyNo);
sys_res nrsec3000_SM2ExPrvKey(p_nrsec3000 p,u8 *key,u8 keyNo);
sys_res nrsec3000_SM2ImPubKey(p_nrsec3000 p,u8 *key,u8 keyNo);
sys_res nrsec3000_SM2ImPrvKey(p_nrsec3000 p,u8 *key,u8 keyNo);
sys_res nrsec3000_Hash(p_nrsec3000 p,u8 *buf,u16 bufLen,u8 *hash);
sys_res nrsec3000_SM3(p_nrsec3000 p,u8 *buf,u16 bufLen,u8 *hash,
								u8 *pubkey,u8 *pucID, size_t idLen);
sys_res nrsec3000_SM2Sign(p_nrsec3000 p,u8 *buf,u8 keyNo,u8 *sign);
sys_res nrsec3000_SM2CheckSign(p_nrsec3000 p,u8 *hash,u8 *sign,u8 keyNo);
sys_res nrsec3000_SM2Encrypt(p_nrsec3000 p,u8 *buf,u8 keyNo,u8 *Enbuf);
sys_res nrsec3000_SM2Decrypt(p_nrsec3000 p,u8 *buf,u8 keyNo,u8 *Debuf);
sys_res nrsec3000_SM2Credentials(p_nrsec3000 p,u8 type,u8 keyNo,u8 *buf,u16 bufLen,u8 *Crebuf);
sys_res nrsec3000_GetVer(p_nrsec3000 p,u8 *ver);
sys_res nrsec3000_GetRandom(p_nrsec3000 p,u8 *random,u8 len);
sys_res nrsec3000_SafetyCre(p_nrsec3000 p,u8 *buf,u8 *safetybuf);

#ifdef __cplusplus
}
#endif

#endif


