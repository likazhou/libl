/*
  This is a hacked up version of some of the sample code from RFC
  1889.  I hope there aren't any legal issues in redistributing it.

  $Id: random.c,v 1.1.1.1 2000/01/07 22:44:03 dreier Exp $
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>   /* u_long */
#include <sys/time.h>    /* gettimeofday() */
#include <unistd.h>      /* get..() */
#include <stdio.h>       /* printf() */
#include <time.h>        /* clock() */
#include <sys/utsname.h> /* uname() */
#include <glib.h>
#include "md5.h"         /* from RFC 1321 */

#define MD_CTX MD5_CTX
#define MDInit MD5Init
#define MDUpdate MD5Update
#define MDFinal MD5Final

/*
 * Generate a random 32-bit quantity.
 */

static guint32
md_32(char *string, int length)
{
  MD_CTX context;
  union {
    char   c[16];
    guint32 x[4];
  } digest;
  guint32 r;
  int i;
  
  MDInit (&context);
  MDUpdate (&context, string, length);
  MDFinal ((unsigned char *)&digest, &context);
  r = 0;
  for (i = 0; i < 3; i++) {
    r ^= digest.x[i];
  }
  return r;
}

/*
 * Return random unsigned 32-bit quantity. Use 'type' argument if you
 * need to generate several different values in close succession.
 */
guint32
random32(int type)
{
  struct {
    int     type;
    struct  timeval tv;
    clock_t cpu;
    pid_t   pid;
    guint32  hid;
    uid_t   uid;
    gid_t   gid;
    struct  utsname name;
  } s;
  
  gettimeofday(&s.tv, 0);
  uname(&s.name);
  s.type = type;
  s.cpu  = clock();
  s.pid  = getpid();
  s.hid  = gethostid();
  s.uid  = getuid();
  s.gid  = getgid();
  
  return md_32((char *)&s, sizeof(s));
}
