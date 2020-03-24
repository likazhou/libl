/*
  Librtp - a library for the RTP/RTCP protocol
  Copyright (C) 2000  Roland Dreier
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
  
  $Id: rtcp-packet.c,v 1.3 2001/05/07 18:55:05 dreier Exp $
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rtcp-packet.h"
#include "rtp.h"

#include <glib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>

struct Rtcp_Packet_Struct {
  gpointer data;
  guint data_len;
  gboolean free_data;
};

struct Rtcp_Compound_Struct {
  gpointer data;
  guint data_len;
  guint max_len;
};

typedef struct Rtcp_Common_Header *Rtcp_Common_Header;
typedef struct Rtcp_RR_Header *Rtcp_RR_Header;
typedef struct Rtcp_SR_Header *Rtcp_SR_Header;
typedef struct Rtcp_SDES_Header *Rtcp_SDES_Header;
typedef struct Rtcp_BYE_Header *Rtcp_BYE_Header;
typedef struct Rtcp_APP_Header *Rtcp_APP_Header;
typedef struct SDES_Item *SDES_Item;
typedef struct Rtcp_RR *Rtcp_RR;

struct Rtcp_Common_Header {
#if G_BYTE_ORDER == G_LITTLE_ENDIAN
  unsigned int count:5;         /* varies by packet type */
  unsigned int padding:1;       /* padding flag */
  unsigned int version:2;       /* protocol version */
#elif G_BYTE_ORDER == G_BIG_ENDIAN
  unsigned int version:2;       /* protocol version */
  unsigned int padding:1;       /* padding flag */
  unsigned int count:5;     /* varies by packet type */
#else
#error "G_BYTE_ORDER should be big or little endian."
#endif
  unsigned int packet_type:8;   /* RTCP packet type */
  guint16 length;               /* pkt len in words, w/o this word */
};

struct Rtcp_RR {
  guint32 ssrc;                 /* data source being reported */
  unsigned int fraction:8;      /* fraction lost since last SR/RR */
  int lost:24;                  /* cumul. no. pkts lost (signed!) */
  guint32 last_seq;             /* extended last seq. no. received */
  guint32 jitter;               /* interarrival jitter */
  guint32 lsr;                  /* last SR packet from this source */
  guint32 dlsr;                 /* delay since last SR packet */
};

struct Rtcp_SR_Header {
  guint32 ssrc;                 /* sender generating this report */
  guint32 ntp_sec;              /* NTP timestamp */
  guint32 ntp_frac;
  guint32 rtp_ts;               /* RTP timestamp */
  guint32 psent;                /* packets sent */
  guint32 osent;                /* octets sent */
  struct Rtcp_RR rr[1];         /* variable-length list */
};

struct Rtcp_RR_Header {
  guint32 ssrc;                 /* receiver generating this report */
  struct Rtcp_RR rr[1];         /* variable-length list */
};

struct SDES_Item {
  guint8 type;                  /* type of item (Rtcp_Sdes_Type) */
  guint8 length;                /* length of item (in octets) */
  gchar data[1];                /* text, not null-terminated */
};

struct Rtcp_SDES_Header {
  guint32 src;                  /* first SSRC/CSRC */
  struct SDES_Item item[1];     /* list of SDES items */
};
    
struct Rtcp_BYE_Header {
  guint32 src[1];               /* list of sources */
  gchar data[1];                /* reason for leaving */
};

struct Rtcp_APP_Header {
  guint32 ssrc;                 /* source */
  gchar name[4];                /* name */
  gchar data[1];                /* application data */
};

/*
  Functions for allocating and freeing packets
*/

Rtcp_Compound
rtcp_compound_new(void)
{
  Rtcp_Compound compound;

  compound = g_malloc(sizeof *compound);

  compound -> data = NULL;
  compound -> data_len = 0;
  compound -> max_len = 0;

  return compound;
}

Rtcp_Compound
rtcp_compound_new_allocate(guint len)
{
  Rtcp_Compound compound;

  compound = g_malloc(sizeof *compound);

  compound -> max_len = len;
  compound -> data_len = 0;
  compound -> data = g_malloc(len);

  return compound;
}

Rtcp_Compound
rtcp_compound_new_take_data(gpointer data, guint data_len)
{
  Rtcp_Compound compound;

  g_return_val_if_fail(data_len < RTP_MTU, NULL);

  compound = g_malloc(sizeof *compound);

  compound -> data = data;
  compound -> data_len = data_len;
  compound -> max_len = data_len;

  return compound;
}

Rtcp_Compound
rtcp_compound_new_copy_data(gpointer data, guint data_len)
{
  Rtcp_Compound compound;

  g_return_val_if_fail(data_len < RTP_MTU, NULL);

  compound = g_malloc(sizeof *compound);

  compound -> data = g_memdup(data, data_len);
  compound -> data_len = data_len;
  compound -> max_len = data_len;

  return compound;
}

void
rtcp_compound_free(Rtcp_Compound compound)
{
  g_return_if_fail(compound != NULL);

  if (compound -> data != NULL) {
    g_free(compound -> data);
  }
  g_free(compound);
}

Rtcp_Compound
rtcp_compound_read(int fd, struct sockaddr *fromaddr, socklen_t *fromlen)
{
  int packlen;
  gpointer buf;

  buf = g_malloc(RTP_MTU);

  packlen = recvfrom(fd, buf, RTP_MTU, 0, fromaddr, fromlen);

  if (packlen < 0) {
    g_error("rtcp_compound_read: recvfrom: %s", strerror(errno));
    exit(1);
  }

  return rtcp_compound_new_take_data(buf, packlen);
}

void
rtcp_compound_send(Rtcp_Compound compound, int fd, struct sockaddr *toaddr, socklen_t tolen)
{
  g_return_if_fail(compound != NULL);

  sendto(fd, (void *) compound -> data,
         compound -> data_len, 0,
	 toaddr, tolen);
}

Rtcp_Packet
rtcp_packet_new(void)
{
  Rtcp_Packet packet;

  packet = g_malloc(sizeof *packet);

  packet -> free_data = FALSE;
  packet -> data = NULL;
  packet -> data_len = 0;

  return packet;
}

Rtcp_Packet
rtcp_packet_new_take_data(gpointer data, guint data_len)
{
  Rtcp_Packet packet;

  g_return_val_if_fail(data_len < RTP_MTU, NULL);

  packet = g_malloc(sizeof *packet);

  packet -> free_data = TRUE;
  packet -> data = data;
  packet -> data_len = data_len;

  return packet;
}

Rtcp_Packet
rtcp_packet_new_copy_data(gpointer data, guint data_len)
{
  Rtcp_Packet packet;

  g_return_val_if_fail(data_len < RTP_MTU, NULL);

  packet = g_malloc(sizeof *packet);

  packet -> free_data = TRUE;
  packet -> data = g_memdup(data, data_len);
  packet -> data_len = data_len;

  return packet;
}

void
rtcp_packet_free(Rtcp_Packet packet)
{
  g_return_if_fail(packet != NULL);

  if (packet -> free_data) {
    g_free(packet -> data);
  }

  g_free(packet);
}

/*
  Functions for processing packets
*/

void rtcp_compound_foreach(Rtcp_Compound compound,
                           Rtcp_Foreach_Func func,
                           gpointer data)
{
  Rtcp_Packet packet;

  g_return_if_fail(compound != NULL);

  packet = rtcp_packet_new();
  
  packet -> data = compound -> data;
  packet -> data_len = rtcp_packet_get_length(packet);

  while ((gint8 *) packet -> data
         - (gint8 *) compound -> data < compound -> data_len) {
    func(packet, data);

    packet -> data = (gpointer) ((gint8 *) packet -> data
                                 + packet -> data_len);
    packet -> data_len = rtcp_packet_get_length(packet);
  }

  rtcp_packet_free(packet);
}

void
rtcp_read_sdes(Rtcp_Packet packet,
               Rtcp_Find_Member_Func find_member,
               Rtcp_Member_SDES_Func member_sdes)
{
  int count = rtcp_packet_get_count(packet);
  Rtcp_SDES_Header sd;
  SDES_Item rsp;
  SDES_Item end = (SDES_Item) ((gint8 *) rtcp_packet_get_data(packet)
                               + rtcp_packet_get_length(packet));
  Rtp_Source *source;
  
  sd = (Rtcp_SDES_Header) rtcp_packet_get_content(packet);

  while (--count >= 0) {
    rsp = &sd -> item[0];
    if (rsp >= end) break;

    source = find_member(g_ntohl(sd -> src));
    
    while (rsp -> type != RTCP_SDES_END) {
      member_sdes(source, rsp -> type, rsp -> data, rsp -> length);
      rsp = (SDES_Item) ((gint8 *) rsp + rsp -> length + 2);
      if (rsp >= end) {
        break;
      }
    }
    sd = (Rtcp_SDES_Header)
      ((guint32 *) sd + (((gint8 *) rsp - (gint8 *) sd) / 4) + 1);
  }

  if (count >= 0) {
    g_warning("Trouble parsing an RTCP SDES packet.");
  }
}

void
rtcp_compound_add_sr(Rtcp_Compound compound,
                     guint32 ssrc,
                     guint32 timestamp,
                     guint32 packets_sent,
                     guint32 octets_sent)
{
  Rtcp_Packet packet;
  Rtcp_SR_Header header;
  struct timeval now;

  g_return_if_fail(compound != NULL);

  g_return_if_fail(compound -> max_len - compound -> data_len
                   > sizeof (struct Rtcp_Common_Header) + 24);

  packet = rtcp_packet_new();

  packet -> data = (gint8 *) compound -> data
    + rtcp_compound_get_length(compound);

  rtcp_packet_set_version(packet, RTP_VERSION);
  rtcp_packet_set_padding(packet, 0);
  rtcp_packet_set_count(packet, 0); /* FIX */
  rtcp_packet_set_packet_type(packet, RTCP_SR);
  rtcp_packet_set_content_length(packet, 24); /* FIX */
  
  header = (Rtcp_SR_Header) rtcp_packet_get_content(packet);
  header -> ssrc = g_htonl(ssrc);

  if (gettimeofday(&now, 0) < 0) {
    g_error("send_rtcp_sr: gettimeofday: %s", strerror(errno));
    exit(1);
  }
  header -> ntp_sec = g_htonl((guint32) now.tv_sec);
  header -> ntp_frac = g_htonl((guint32) now.tv_usec * 1.0e6);
  
  header -> rtp_ts = g_htonl(timestamp);
  header -> psent = g_htonl(packets_sent);
  header -> osent = g_htonl(octets_sent);

  rtcp_compound_set_length(compound,
                           rtcp_compound_get_length(compound)
                           + rtcp_packet_get_length(packet));

  rtcp_packet_free(packet);
}

void
rtcp_compound_add_sdes(Rtcp_Compound compound,
                       guint32 ssrc,
                       guint nsdes,
                       Rtcp_Sdes_Type type[],
                       char *value[],
                       gint8 length[])
{
  Rtcp_Packet packet;
  Rtcp_SDES_Header header;
  SDES_Item item;
  int i;
  int pad;
  gint8 *padloc;

  g_return_if_fail(compound != NULL);

  /* FIX: should check if there's space left in the compound here */

  packet = rtcp_packet_new();

  packet -> data = (gint8 *) compound -> data
    + rtcp_compound_get_length(compound);

  rtcp_packet_set_version(packet, RTP_VERSION);
  rtcp_packet_set_padding(packet, 0);
  rtcp_packet_set_count(packet, nsdes);
  rtcp_packet_set_packet_type(packet, RTCP_SDES);

  header = (Rtcp_SDES_Header) rtcp_packet_get_content(packet);
  header -> src = g_htonl(ssrc);

  item = &header -> item[0];

  for (i = 0; i < nsdes; i++) {
    item -> type = type[i];
    item -> length = length[i];
    g_memmove(item -> data, value[i], length[i]);
    item = (SDES_Item) &item -> data[length[i]];
  }

  padloc = (gint8 *) item;
  for (pad = 4 - ((gint8 *) item - (gint8 *) header) % 4;
       pad > 0; pad--) {
    *padloc++ = RTCP_SDES_END;
  }

  rtcp_packet_set_content_length(packet, padloc - (gint8 *) header);

  rtcp_compound_set_length(compound,
                           rtcp_compound_get_length(compound)
                           + rtcp_packet_get_length(packet));

  rtcp_packet_free(packet);
}

void
rtcp_compound_add_app(Rtcp_Compound compound,
                      guint32 ssrc,
                      const gchar name[4],
                      gpointer data,
                      guint data_len)
{
  Rtcp_Packet packet;
  Rtcp_APP_Header header;
  int padlen;
  int i;

  g_return_if_fail(compound != NULL);

  padlen = 4 - data_len % 4;

  g_return_if_fail(compound -> max_len - compound -> data_len
                   > sizeof (struct Rtcp_Common_Header)
                   + 4 + data_len + padlen);

  packet = rtcp_packet_new();

  packet -> data = (gint8 *) compound -> data
    + rtcp_compound_get_length(compound);

  rtcp_packet_set_version(packet, RTP_VERSION);
  rtcp_packet_set_padding(packet, 0);
  rtcp_packet_set_count(packet, 0);
  rtcp_packet_set_packet_type(packet, RTCP_APP);
  rtcp_packet_set_content_length(packet, 4 + data_len + padlen);

  header = (Rtcp_APP_Header) rtcp_packet_get_content(packet);
  header -> ssrc = g_htonl(ssrc);

  for (i = 0; i < 4; i++) {
    header -> name[i] = name[i];
  }

  g_memmove(header -> data, data, data_len);

  for (i = 0; i < padlen; i++) {
    header -> data[data_len + i] = 0;
  }

  rtcp_compound_set_length(compound,
                           rtcp_compound_get_length(compound)
                           + rtcp_packet_get_length(packet));

  rtcp_packet_free(packet);
}

/*
  Functions for getting and setting packet fields
*/

guint16
rtcp_compound_get_length(Rtcp_Compound compound)
{
  g_return_val_if_fail(compound != NULL, 0);

  return compound -> data_len;
}

void
rtcp_compound_set_length(Rtcp_Compound compound, guint16 len)
{
  g_return_if_fail(compound != NULL);

  compound -> data_len = len;
}

guint8
rtcp_packet_get_version(Rtcp_Packet packet)
{
  g_return_val_if_fail(packet != NULL, 0);

  return ((Rtcp_Common_Header) packet -> data) -> version;
}

void
rtcp_packet_set_version(Rtcp_Packet packet, guint8 version)
{
  g_return_if_fail(packet != NULL);
  g_return_if_fail(version < 0x04);

  ((Rtcp_Common_Header) packet -> data) -> version = version;
}

guint8
rtcp_packet_get_padding(Rtcp_Packet packet)
{
  g_return_val_if_fail(packet != NULL, 0);

  return ((Rtcp_Common_Header) packet -> data) -> padding;
}

void
rtcp_packet_set_padding(Rtcp_Packet packet, guint8 padding)
{
  g_return_if_fail(packet != NULL);
  g_return_if_fail(padding < 0x02);

  ((Rtcp_Common_Header) packet -> data) -> padding = padding;
}

guint8
rtcp_packet_get_count(Rtcp_Packet packet)
{
  g_return_val_if_fail(packet != NULL, 0);

  return ((Rtcp_Common_Header) packet -> data) -> count;
}

void
rtcp_packet_set_count(Rtcp_Packet packet, guint8 count)
{
  g_return_if_fail(packet != NULL);
  g_return_if_fail(count < 0x20);

  ((Rtcp_Common_Header) packet -> data) -> count = count;
}

guint8
rtcp_packet_get_packet_type(Rtcp_Packet packet)
{
  g_return_val_if_fail(packet != NULL, 0);

  return ((Rtcp_Common_Header) packet -> data) -> packet_type;
}

void
rtcp_packet_set_packet_type(Rtcp_Packet packet, guint8 packet_type)
{
  g_return_if_fail(packet != NULL);

  ((Rtcp_Common_Header) packet -> data) -> packet_type = packet_type;
}

guint16
rtcp_packet_get_length(Rtcp_Packet packet)
{
  g_return_val_if_fail(packet != NULL, 0);

  return 4 + 4 * g_ntohs(((Rtcp_Common_Header) packet -> data) -> length);
}

void
rtcp_packet_set_content_length(Rtcp_Packet packet, guint16 length)
{
  g_return_if_fail(packet != NULL);
  g_assert(length % 4 == 0);

  ((Rtcp_Common_Header) packet -> data) -> length =
    g_htons(length / 4);
}

gpointer
rtcp_packet_get_data(Rtcp_Packet packet)
{
  g_return_val_if_fail(packet != NULL, NULL);

  return packet -> data;
}

gpointer
rtcp_packet_get_content(Rtcp_Packet packet)
{
  g_return_val_if_fail(packet != NULL, NULL);

  return (gint8 *) packet -> data + sizeof (struct Rtcp_Common_Header);
}

gchar *
rtcp_app_packet_get_name(Rtcp_Packet packet)
{
  g_return_val_if_fail(packet != NULL, NULL);
  g_return_val_if_fail(rtcp_packet_get_packet_type(packet)
                       == RTCP_APP, NULL);

  return (gchar *) &((Rtcp_APP_Header)
                     rtcp_packet_get_content(packet)) -> name;
}

gpointer
rtcp_app_packet_get_data(Rtcp_Packet packet)
{
  g_return_val_if_fail(packet != NULL, NULL);
  g_return_val_if_fail(rtcp_packet_get_packet_type(packet)
                       == RTCP_APP, NULL);

  return (gchar *) &((Rtcp_APP_Header)
                     rtcp_packet_get_content(packet)) -> data;
}
