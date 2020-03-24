#ifndef __RTP_GLIB_H__
#define __RTP_GLIB_H__

#ifdef __cplusplus
extern "C" {
#endif


//Public Typedefs
typedef uint_t				guint;
typedef uint8_t				guint8;
typedef uint16_t			guint16;
typedef uint32_t			guint32;
typedef char				gchar;

typedef void *				gpointer;


//Public Defines
#define G_BYTE_ORDER		G_LITTLE_ENDIAN

#define g_error(fmt, err)

#define g_htonl(x)			htonl(x)
#define g_htons(x)			htons(x)
#define g_ntohl(x)			ntohl(x)
#define g_ntohs(x)			ntohs(x)

#define g_malloc(size)		mem_Malloc(size)
#define g_free(p)			mem_Free(p)

#define g_return_if_fail(expr)			{if (!(expr)) return;}
#define g_return_val_if_fail(expr, val)	{if (!(expr)) return (val);}


//External Functions
double drand48(void);

void *g_memdup(const void *data, uint_t data_len);




#ifdef __cplusplus
}
#endif

#endif

