/*
  This is a hacked up version of the rtp.h file from RFC 1889.  I hope 
  there aren't any legal issues in redistributing it.

  $Id: rtp.h,v 1.4 2001/05/07 19:07:05 dreier Exp $
*/

#ifndef _RTP_H
#define _RTP_H 1

//#include <sys/types.h>
#include <net/lwip/sockets.h>
#include "glib.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Current protocol version.
 */
enum {
  RTP_VERSION = 2,
  RTP_MTU = 2048
};

#define RTP_SEQ_MOD (1<<16)
#define RTP_MAX_SDES 255      /* maximum text length for SDES */

typedef enum {
  RTCP_SR   = 200,
  RTCP_RR   = 201,
  RTCP_SDES = 202,
  RTCP_BYE  = 203,
  RTCP_APP  = 204
} Rtcp_Type;

typedef enum {
  RTCP_SDES_END   = 0,
  RTCP_SDES_CNAME = 1,
  RTCP_SDES_NAME  = 2,
  RTCP_SDES_EMAIL = 3,
  RTCP_SDES_PHONE = 4,
  RTCP_SDES_LOC   = 5,
  RTCP_SDES_TOOL  = 6,
  RTCP_SDES_NOTE  = 7,
  RTCP_SDES_PRIV  = 8,
  RTCP_SDES_MAX   = 9
} Rtcp_Sdes_Type;

enum {
  RTP_HEADER_LEN = 12
};

/*
 * Big-endian mask for version, padding bit and packet type pair
 */
#define RTCP_VALID_MASK (0xc000 | 0x2000 | 0xfe)
#define RTCP_VALID_VALUE ((RTP_VERSION << 14) | RTCP_SR)

/*
 * Per-source state information
 */
typedef struct {
  /* RFC 1889 fields */
  guint16 max_seq;        /* highest seq. number seen */
  guint32 cycles;         /* shifted count of seq. number cycles */
  guint32 base_seq;       /* base seq number */
  guint32 bad_seq;        /* last 'bad' seq number + 1 */
  guint32 probation;      /* sequ. packets till source is valid */
  guint32 received;       /* packets received */
  guint32 expected_prior; /* packet expected at last interval */
  guint32 received_prior; /* packet received at last interval */
  guint32 transit;        /* relative trans time for prev pkt */
  guint32 jitter;         /* estimated jitter */
  /* librtp fields */
  guint32 ssrc;
  struct sockaddr address;
  gchar *hostname;
  guint8 sdes_len[RTCP_SDES_MAX];
  gchar *sdes_data[RTCP_SDES_MAX];
} Rtp_Source;

void init_seq(Rtp_Source *s, guint16 seq);
int update_seq(Rtp_Source *s, guint16 seq);
double rtcp_interval(int members, int senders, double rtcp_bw,
                     int we_sent, int packet_size, int *avg_rtcp_size,
                     int initial);
guint32 random32(int type);

#ifdef __cplusplus
}
#endif

#endif /* rtp.h */
