#ifndef __SMARTCARD_H__
#define __SMARTCARD_H__


#ifdef __cplusplus
extern "C" {
#endif




//Exported constants
#define SC_LE_MAX			20

#define SC_TYPE_PSAM		0x02
#define SC_TYPE_ESAM		0x00

#define SC_S_IDLE			0
#define SC_S_ACTIVE			1
#define SC_S_BUSY			2

/* SC Tree Structure -----------------------------------------------------------
                              MasterFile
                           ________|___________
                          |        |           |
                        System   UserData     Note
------------------------------------------------------------------------------*/

/* SC ADPU Command: Operation Code -------------------------------------------*/
#define SC_CLA_GSM11		0xA0

/*------------------------ Data Area Management Commands ---------------------*/
#define SC_SELECT_FILE		0xA4
#define SC_GET_RESPONCE		0xC0
#define SC_STATUS			0xF2
#define SC_UPDATE_BINARY	0xD6
#define SC_READ_BINARY		0xB0
#define SC_WRITE_BINARY		0xD0
#define SC_UPDATE_RECORD	0xDC
#define SC_READ_RECORD		0xB2

/*-------------------------- Administrative Commands -------------------------*/ 
#define SC_CREATE_FILE		0xE0

/*-------------------------- Safety Management Commands ----------------------*/
#define SC_VERIFY			0x20
#define SC_CHANGE			0x24
#define SC_DISABLE			0x26
#define SC_ENABLE			0x28
#define SC_UNBLOCK			0x2C
#define SC_EXTERNAL_AUTH	0x82
#define SC_GET_CHALLENGE	0x84


/* ATR structure - Answer To Reset -------------------------------------------*/
#define SC_SETUP_SIZE		20
#define SC_HIST_SIZE		20
typedef struct {
	u8 TS;               /* Bit Convention */
	u8 T0;               /* High nibble = Number of setup byte; low nibble = Number of historical byte */
	u8 T[SC_SETUP_SIZE];  /* Setup array */
	u8 H[SC_HIST_SIZE];   /* Historical array */
	u8 Tlength;          /* Setup array dimension */
	u8 Hlength;          /* Historical array dimension */
} sc_atr[1];

typedef struct {
	u8 cla;
	u8 ins;
	u8 p1;
	u8 p2;
} sc_cmd[1], *p_sc_cmd;

typedef struct {
	uart_t *	parent;
	u8 	ste;
	u8		ptrl;
	u8		sw1;
	u8		sw2;
	u8		sn[8];
	u8		resp[SC_LE_MAX];
} t_smartcard, *p_smartcard;



//External Functions
void sc_Init(p_smartcard sc);
void sc_Release(p_smartcard sc);
sys_res sc_Get(p_smartcard sc, int nId, size_t nTmo);
sys_res sc_GetRandom(p_smartcard sc, size_t nLen);
sys_res sc_Authority(p_smartcard sc, int nIns, int nP1, int nP2, void *pBuf, size_t nLen);
sys_res sc_EsamIntAuthority(p_smartcard sc, int nP1, int nP2, void *pBuf);


#ifdef __cplusplus
}
#endif

#endif


