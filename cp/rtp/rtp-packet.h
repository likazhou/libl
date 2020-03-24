/*
  Gnome-o-Phone - A program for internet telephony
  Copyright (C) 1999  Roland Dreier
  
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
  
  $Id: rtp-packet.h,v 1.3 2001/05/07 18:55:05 dreier Exp $
*/

#ifndef _RTP_PACKET_H
#define _RTP_PACKET_H 1

#include "glib.h"
//#include <netinet/in.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Rtp_Packet_Struct *Rtp_Packet;

Rtp_Packet rtp_packet_new_take_data(gpointer data, guint data_len);
Rtp_Packet rtp_packet_new_copy_data(gpointer data, guint data_len);
Rtp_Packet rtp_packet_new_allocate(guint payload_len,
                                   guint pad_len, guint csrc_count);
void rtp_packet_free(Rtp_Packet packet);
Rtp_Packet rtp_packet_read(int fd, struct sockaddr *fromaddr, socklen_t *fromlen);
void rtp_packet_send(Rtp_Packet packet, int fd, struct sockaddr *toaddr, socklen_t tolen);
guint8 rtp_packet_get_version(Rtp_Packet packet);
void rtp_packet_set_version(Rtp_Packet packet, guint8 version);
guint8 rtp_packet_get_padding(Rtp_Packet packet);
void rtp_packet_set_padding(Rtp_Packet packet, guint8 padding);
guint8 rtp_packet_get_csrc_count(Rtp_Packet packet);
guint8 rtp_packet_get_extension(Rtp_Packet packet);
void rtp_packet_set_extension(Rtp_Packet packet, guint8 extension);
void rtp_packet_set_csrc_count(Rtp_Packet packet, guint8 csrc_count);
guint8 rtp_packet_get_marker(Rtp_Packet packet);
void rtp_packet_set_marker(Rtp_Packet packet, guint8 marker);
guint8 rtp_packet_get_payload_type(Rtp_Packet packet);
void rtp_packet_set_payload_type(Rtp_Packet packet, guint8 payload_type);
guint16 rtp_packet_get_seq(Rtp_Packet packet);
void rtp_packet_set_seq(Rtp_Packet packet, guint16 seq);
guint32 rtp_packet_get_timestamp(Rtp_Packet packet);
void rtp_packet_set_timestamp(Rtp_Packet packet, guint32 timestamp);
guint32 rtp_packet_get_ssrc(Rtp_Packet packet);
void rtp_packet_set_ssrc(Rtp_Packet packet, guint32 ssrc);
guint rtp_packet_get_payload_len(Rtp_Packet packet);
gpointer rtp_packet_get_payload(Rtp_Packet packet);
guint rtp_packet_get_packet_len(Rtp_Packet packet);

#ifdef __cplusplus
}
#endif

#endif /* rtp-packet.h */
