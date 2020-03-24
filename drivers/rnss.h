#ifndef __RNSS_H__
#define __RNSS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <drivers/rdrn.h>



//Public Typedefs
struct rnss
{
	u8		ste;
	u8		tmo;
	u8		fixed;
	int		latitude;
	int		longitud;
	time_t	time;
	uart_t *uart;
	buf		rbuf;
};
typedef struct rnss rnss_t;


struct rnss_def
{
#if RNSS_RST_ENABLE
	t_gpio_def	rst;
#endif
#if RNSS_1PPS_ENABLE
	t_gpio_def	onepps;
#endif
};
typedef const struct rnss_def rnss_def_t;


//External Functions
void rnss_GetLocation(int *latitude, int *longitud);
time_t rnss_GetTimet(void);



#ifdef __cplusplus
}
#endif

#endif

