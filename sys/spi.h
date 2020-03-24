#ifndef __SYS_SPI_H__
#define __SYS_SPI_H__


#ifdef __cplusplus
extern "C" {
#endif





#define SPI_S_IDLE				0
#define SPI_S_BUSY				1

#define SPI_SCKIDLE_HIGH		0
#define SPI_SCKIDLE_LOW			1

#define SPI_LATCH_1EDGE			0
#define SPI_LATCH_2EDGE			1

#define SPI_CSID_INVALID		0xFF


typedef const struct
{
	u32	chipid : 3,
		sckport : 3,
		sckpin : 4,
		mosiport : 3,
		mosipin : 4,
		misoport : 3,
		misopin : 4,
		nssport : 3,
		nsspin : 4,
		outmode : 1;
} t_spi_def;


typedef struct
{
	struct dev	parent;
	u8	ste;
	u8	sckmode;
	u8	latchmode;
	u8	csid;
	u32	speed;
#if SPI_SOFTWARE == 0
	t_spi_def *	def;
#endif
} spi_t;



//External Variables
extern spi_t dev_Spi[];


//External Functions
spi_t *spi_Open(int nId, size_t nTmo);
sys_res spi_Close(spi_t *p);
sys_res spi_Config(spi_t *p, int nSckMode, int nLatch, int nSpeed);
void spi_CsSel(spi_t *p, int nId);
sys_res spi_Start(spi_t *p);
sys_res spi_SendChar(spi_t *p, u8 nData);
sys_res spi_End(spi_t *p);
sys_res spi_Send(spi_t *p, const void *pData, size_t nLen);
sys_res spi_Recv(spi_t *p, void *pRec, size_t nLen);
sys_res spi_Transce(spi_t *p, const void *pCmd, size_t nCmdLen, void *pRec, size_t nRecLen);
sys_res spi_TransChar(spi_t *p, u8 nSend, void *pRec);


#ifdef __cplusplus
}
#endif

#endif

