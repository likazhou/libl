/*
  This is a hacked up version of some of the sample code from RFC
  1889.  I hope there aren't any legal issues in redistributing it.

  $Id: rtp.c,v 1.2 2000/12/02 03:42:23 dreier Exp $
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rtp.h"

#include <stdlib.h>
#include <math.h>

void
init_seq(Rtp_Source *s, guint16 seq)
{
  s->base_seq = seq - 1;
  s->max_seq = seq;
  s->bad_seq = RTP_SEQ_MOD + 1;
  s->cycles = 0;
  s->received = 0;
  s->received_prior = 0;
  s->expected_prior = 0;
  /* other initialization */
}

int
update_seq(Rtp_Source *s, guint16 seq)
{
  guint16 udelta = seq - s->max_seq;
  const int MAX_DROPOUT = 3000;
  const int MAX_MISORDER = 100;
  const int MIN_SEQUENTIAL = 2;
  
  /*
   * Source is not valid until MIN_SEQUENTIAL packets with
   * sequential sequence numbers have been received.
   */
  if (s->probation) {
    /* packet is in sequence */
    if (seq == s->max_seq + 1) {
      s->probation--;
      s->max_seq = seq;
      if (s->probation == 0) {
        init_seq(s, seq);
        s->received++;
        return 1;
      }
    } else {
      s->probation = MIN_SEQUENTIAL - 1;
      s->max_seq = seq;
    }
    return 0;
  } else if (udelta < MAX_DROPOUT) {
    /* in order, with permissible gap */
    if (seq < s->max_seq) {
      s->cycles += RTP_SEQ_MOD;
    }
    s->max_seq = seq;
  } else if (udelta <= RTP_SEQ_MOD - MAX_MISORDER) {
    /* the sequence number made a very large jump */
    if (seq == s->bad_seq) {
      /*
       * Two sequential packets -- assume that the other side
       * restarted without telling us so just re-sync
       * (i.e., pretend this was the first packet).
       */
      init_seq(s, seq);
    } else {
      s->bad_seq = (seq + 1) & (RTP_SEQ_MOD-1);
      return 0;
    }
  } else {
    /* duplicate or reordered packet */
    return 0;
  }
  s->received++;
  return 1;
}

double
rtcp_interval(int members,
              int senders,
              double rtcp_bw,
              int we_sent,
              int packet_size,
              int *avg_rtcp_size,
              int initial)
{
  /*
   * Minimum time between RTCP packets from this site (in seconds).
   * This time prevents the reports from `clumping' when sessions
   * are small and the law of large numbers isn't helping to smooth
   * out the traffic.  It also keeps the report interval from
   * becoming ridiculously small during transient outages like a
   * network partition.
   */
  double const RTCP_MIN_TIME = 5.;
  /*
   * Fraction of the RTCP bandwidth to be shared among active
   * senders.  (This fraction was chosen so that in a typical
   * session with one or two active senders, the computed report
   * time would be roughly equal to the minimum report time so that
   * we don't unnecessarily slow down receiver reports.) The
   * receiver fraction must be 1 - the sender fraction.
   */
  double const RTCP_SENDER_BW_FRACTION = 0.25;
  double const RTCP_RCVR_BW_FRACTION = (1-RTCP_SENDER_BW_FRACTION);
  /*
   * Gain (smoothing constant) for the low-pass filter that
   * estimates the average RTCP packet size (see Cadzow reference).
   */
  double const RTCP_SIZE_GAIN = (1./16.);
  
  double t;                   /* interval */
  double rtcp_min_time = RTCP_MIN_TIME;
  int n;                      /* no. of members for computation */
  
  /*
   * Very first call at application start-up uses half the min
   * delay for quicker notification while still allowing some time
   * before reporting for randomization and to learn about other
   * sources so the report interval will converge to the correct
   * interval more quickly.  The average RTCP size is initialized
   * to 128 octets which is conservative (it assumes everyone else
   * is generating SRs instead of RRs: 20 IP + 8 UDP + 52 SR + 48
   * SDES CNAME).
   */
  if (initial) {
    rtcp_min_time /= 2;
    *avg_rtcp_size = 128;
  }
  
  /*
   * If there were active senders, give them at least a minimum
   * share of the RTCP bandwidth.  Otherwise all participants share
   * the RTCP bandwidth equally.
   */
  n = members;
  if (senders > 0 && senders < members * RTCP_SENDER_BW_FRACTION) {
    if (we_sent) {
      rtcp_bw *= RTCP_SENDER_BW_FRACTION;
      n = senders;
    } else {
      rtcp_bw *= RTCP_RCVR_BW_FRACTION;
      n -= senders;
    }
  }
  
  /*
   * Update the average size estimate by the size of the report
   * packet we just sent.
   */
  *avg_rtcp_size += (packet_size - *avg_rtcp_size)*RTCP_SIZE_GAIN;
  
  /*
   * The effective number of sites times the average packet size is
   * the total number of octets sent when each site sends a report.
   * Dividing this by the effective bandwidth gives the time
   * interval over which those packets must be sent in order to
   * meet the bandwidth target, with a minimum enforced.  In that
   * time interval we send one report so this time is also our
   * average time between reports.
   */
  t = (*avg_rtcp_size) * n / rtcp_bw;
  if (t < rtcp_min_time) t = rtcp_min_time;
  
  /*
   * To avoid traffic bursts from unintended synchronization with
   * other sites, we then pick our actual next report interval as a
   * random number uniformly distributed between 0.5*t and 1.5*t.
   */
  return t * (drand48() + 0.5);
}
