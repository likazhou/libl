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
  
  $Id: rtp-audio.h,v 1.3 2000/02/27 21:03:34 dreier Exp $
*/

#ifndef _RTP_AUDIO_H
#define _RTP_AUDIO_H 1

#include <glib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  PAYLOAD_GSM = 3,
  PAYLOAD_L16_MONO = 11,
  PAYLOAD_G723_63 = 16,         /* Not standard */
  PAYLOAD_G723_53 = 17,         /* Not standard */
  PAYLOAD_TS48 = 18,            /* Not standard */
  PAYLOAD_TS41 = 19,            /* Not standard */
  PAYLOAD_G728 = 20,            /* Not standard */
  PAYLOAD_G729 = 21,            /* Not standard */
} rtp_payload_t;

#ifdef __cplusplus
}
#endif

#endif /* rtp-audio.h */
