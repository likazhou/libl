#ifndef __SYS_UART_H__
#define __SYS_UART_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <lib/buffer.h>


//Public Defines
#define UART_7D_ENABLE			0

#define UART_DQUE_RX_CHL		0x20	// not 0
#define UART_DQUE_TX_CHL		0x30

#define UART_T_INT				0
#define UART_T_TIMER			1
#define UART_T_SC16IS7X			2
#define UART_T_VK321X			3

#define UART_FUN_NORMAL			0
#define UART_FUN_SC				1
#define UART_FUN_IRDA			2

#define UART_MODE_POLL			0
#define UART_MODE_IRQ			1

#define UART_PARI_NO			0
#define UART_PARI_EVEN			1
#define UART_PARI_ODD			2

#define UART_DATA_7D			0
#define UART_DATA_8D			1

#define UART_STOP_0_5D			0
#define UART_STOP_1D			1
#define UART_STOP_1_5D			2
#define UART_STOP_2D			3

#define IRDA_MODE_PWM			0
#define IRDA_MODE_TIM			1

#define SWUART_RX_M_EINT		0
#define SWUART_RX_M_CAP			1


//Public Typedefs
typedef const struct
{
	u8	type;
	u8	id;
	u8	txport;
	u8	txpin : 7,
		txmode : 1;
	u8	rxport;
	u8	rxpin : 7,
		pinmode : 1;
	u8	fport;
	u8	fpin : 6,
		fun : 2;
} t_uart_def;

typedef struct
{
	u32	pari : 2,
		data : 4,
		stop : 2,
		baud : 24;
} uart_para_t;

typedef struct
{
	struct dev	parent;
	uart_para_t	para;
	t_uart_def *def;
} uart_t;




//External Variables
extern uart_t dev_Uart[];



//External Functions
void uart_Init(uart_t *p);
uart_t *uart_Open(int nId, size_t nTmo);
sys_res uart_Close(uart_t *p);
sys_res uart_Config(uart_t *p, int nBaud, int nPari, int nData, int nStop);
sys_res uart_Send(uart_t *p, const void *pData, size_t nLen);
sys_res uart_SendStr(uart_t *p, const char *str);
int uart_Recive(uart_t *p, buf b);
sys_res uart_RecTmo(uart_t *p, buf b, size_t nTmo);
int uart_RecLen(uart_t *p, buf b, size_t nLen, size_t nTmo);
int uart_Read(uart_t *p, void *pData, size_t nLen);
int uart_GetChar(uart_t *p);
sys_res uart_ScReset(uart_t *p, int nHL);
void uart_Maintian(void);
void uart_Reopen(void);


#ifdef __cplusplus
}
#endif

#endif

