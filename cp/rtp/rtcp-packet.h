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
  
  $Id: rtcp-packet.h,v 1.3 2001/05/07 18:55:05 dreier Exp $
*/

#ifndef _RTCP_PACKET_H
#define _RTCP_PACKET_H 1

#include "rtp.h"

#include <glib.h>
#include <netinet/in.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Rtcp_Packet_Struct *Rtcp_Packet;
typedef struct Rtcp_Compound_Struct *Rtcp_Compound;
typedef void (*Rtcp_Foreach_Func) (Rtcp_Packet packet,
                                   gpointer data);
typedef Rtp_Source *(*Rtcp_Find_Member_Func) (guint32 src);
typedef void (*Rtcp_Member_SDES_Func) (Rtp_Source *s,
                                       guint8 type,
                                       char *data,
                                       guint8 length);


Rtcp_Compound rtcp_compound_new(void);
Rtcp_Compound rtcp_compound_new_allocate(guint len);
Rtcp_Compound rtcp_compound_new_take_data(gpointer data, guint data_len);
Rtcp_Compound rtcp_compound_new_copy_data(gpointer data, guint data_len);
void rtcp_compound_free(Rtcp_Compound compound);

void rtcp_compound_foreach(Rtcp_Compound compound,
                           Rtcp_Foreach_Func func,
                           gpointer data);
void rtcp_read_sdes(Rtcp_Packet packet,
                   Rtcp_Find_Member_Func find_member,
                   Rtcp_Member_SDES_Func member_sdes);
Rtcp_Compound rtcp_compound_read(int fd, struct sockaddr *fromaddr, socklen_t *fromlen);
void rtcp_compound_send(Rtcp_Compound compound, int fd, struct sockaddr *toaddr, socklen_t tolen);
Rtcp_Packet rtcp_packet_new(void);
Rtcp_Packet rtcp_packet_new_take_data(gpointer data, guint data_len);
Rtcp_Packet rtcp_packet_new_copy_data(gpointer data, guint data_len);
void rtcp_packet_free(Rtcp_Packet packet);
void rtcp_compound_add_sr(Rtcp_Compound compound,
                          guint32 ssrc,
                          guint32 timestamp,
                          guint32 packets_sent,
                          guint32 octets_sent);
void rtcp_compound_add_sdes(Rtcp_Compound compound,
                            guint32 ssrc,
                            guint nsdes,
                            Rtcp_Sdes_Type type[],
                            char *value[],
                            gint8 length[]);
void rtcp_compound_add_app(Rtcp_Compound compound,
                           guint32 ssrc,
                           const gchar name[4],
                           gpointer data,
                           guint data_len);

guint16 rtcp_compound_get_length(Rtcp_Compound compound);
void rtcp_compound_set_length(Rtcp_Compound compound, guint16 len);
guint8 rtcp_packet_get_version(Rtcp_Packet packet);
void rtcp_packet_set_version(Rtcp_Packet packet, guint8 version);
guint8 rtcp_packet_get_padding(Rtcp_Packet packet);
void rtcp_packet_set_padding(Rtcp_Packet packet, guint8 padding);
guint8 rtcp_packet_get_count(Rtcp_Packet packet);
void rtcp_packet_set_count(Rtcp_Packet packet, guint8 count);
guint8 rtcp_packet_get_packet_type(Rtcp_Packet packet);
void rtcp_packet_set_packet_type(Rtcp_Packet packet, guint8 packet_type);
guint16 rtcp_packet_get_length(Rtcp_Packet packet);
void rtcp_packet_set_content_length(Rtcp_Packet packet, guint16 length);
gpointer rtcp_packet_get_data(Rtcp_Packet packet);
gpointer rtcp_packet_get_content(Rtcp_Packet packet);
gchar *rtcp_app_packet_get_name(Rtcp_Packet packet);
gpointer rtcp_app_packet_get_data(Rtcp_Packet packet);

#ifdef __cplusplus
}
#endif

#endif /* rtcp-packet.h */
