/*----------------------------------------------------------------------------
 *      RL-ARM - A P I 
 *----------------------------------------------------------------------------
 *      Name:    RTL.H 
 *      Purpose: Application Programming Interface 
 *      Rev.:    V4.70
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2013 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#ifndef __RTL_H__
#define __RTL_H__

/* RL-ARM version number. */
#define __RL_ARM_VER    470
 
#define __task          __declspec(noreturn)
#define __used          __attribute__((used))

#ifndef NULL
 #ifdef __cplusplus
  #define NULL          0
 #else
  #define NULL          ((void *) 0)
 #endif
#endif

#ifndef EOF
 #define EOF            (-1)
#endif

#ifndef __size_t
 #define __size_t       1
 typedef unsigned int   size_t;
#endif

typedef signed char     S8;
typedef unsigned char   U8;
typedef short           S16;
typedef unsigned short  U16;
typedef int             S32;
typedef unsigned int    U32;
typedef long long       S64;
typedef unsigned long long U64;
typedef unsigned char   BIT;
typedef unsigned int    BOOL;

#ifndef __TRUE
 #define __TRUE         1
#endif
#ifndef __FALSE
 #define __FALSE        0
#endif

#ifdef __BIG_ENDIAN
 #define U32_LE(v)      (U32)(__REV(v))
 #define U16_LE(v)      (U16)(__REV(v) >> 16)
 #define U32_BE(v)      (U32)(v)
 #define U16_BE(v)      (U16)(v)
#else
 #define U32_BE(v)      (U32)(__REV(v))
 #define U16_BE(v)      (U16)(__REV(v) >> 16)
 #define U32_LE(v)      (U32)(v)
 #define U16_LE(v)      (U16)(v)
#endif
#define ntohs(v)        U16_BE(v)
#define ntohl(v)        U32_BE(v)
#define htons(v)        ntohs(v)
#define htonl(v)        ntohl(v)

/*----------------------------------------------------------------------------
 *                             RTX Kernel API
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C"  {
#endif

/* Definition of Semaphore type */
typedef U32 OS_SEM[2];

/* Definition of Mailbox type */
#define os_mbx_declare(name,cnt)    U32 name [4 + cnt]
typedef U32 OS_MBX[];

/* Definition of Mutex type */
typedef U32 OS_MUT[3];

/* Task Identification number. */
typedef U32 OS_TID;

/* Function return of system calls returning an object identification */
typedef void *OS_ID;

/* Function return of system calls indicating an event or completion state */
typedef U32 OS_RESULT;



/*----------------------------------------------------------------------------
 *                        Flash File System API
 *---------------------------------------------------------------------------*/

/* File System Type */
typedef enum _FS_TYPE {
  FS_TYPE_NONE = 0,                     /* No file system (volume unformatted)*/
  FS_TYPE_UNKNOWN,                      /* File system type is unknown       */
  FS_TYPE_FAT12,                        /* File system type is FAT12         */
  FS_TYPE_FAT16,                        /* File system type is FAT16         */
  FS_TYPE_FAT32,                        /* File system type is FAT32         */
  FS_TYPE_EFS                           /* File system type is EFS           */
} FS_TYPE;

typedef struct {                        /* RL Time format (FFS, TCPnet)      */
  U8  hr;                               /* Hours    [0..23]                  */
  U8  min;                              /* Minutes  [0..59]                  */
  U8  sec;                              /* Seconds  [0..59]                  */
  U8  day;                              /* Day      [1..31]                  */
  U8  mon;                              /* Month    [1..12]                  */
  U16 year;                             /* Year     [1980..2107]             */
} RL_TIME;

typedef struct {                        /* Search info record                */
  S8  name[256];                        /* Name                              */
  U32 size;                             /* File size in bytes                */
  U16 fileID;                           /* System Identification             */
  U8  attrib;                           /* Attributes                        */
  RL_TIME time;                         /* Create/Modify Time                */
} FINFO;

/* Drive information */
typedef struct {
  FS_TYPE fs_type;                      /* Drives file system type           */
  U64     capacity;                     /* Drives capacity in bytes          */
} Drive_INFO;

extern int finit (const char *drive);
extern int funinit (const char *drive);
extern int fdelete (const char *filename);
extern int frename (const char *oldname, const char *newname);
extern int ffind (const char *pattern, FINFO *info);
extern U64 ffree (const char *drive);
extern int fformat (const char *drive);
extern int fanalyse (const char *drive);
extern int fcheck (const char *drive);
extern int fdefrag (const char *drive);
extern int fattrib (const char *par, const char *path);
extern int fvol    (const char *drive, char *buf);
extern int finfo   (const char *drive, Drive_INFO *info);


#if TCPPS_TYPE == TCPPS_T_KEILTCP
/*----------------------------------------------------------------------------
 *                             TCPnet API
 *---------------------------------------------------------------------------*/

/* UDP Options */
#define UDP_OPT_SEND_CS    0x01   /* Calculate Checksum for UDP send frames  */
#define UDP_OPT_CHK_CS     0x02   /* Verify Checksum for received UDP frames */

/* TCP Socket Types */
#define TCP_TYPE_SERVER    0x01   /* Socket Type Server (open for listening) */
#define TCP_TYPE_CLIENT    0x02   /* Socket Type Client (initiate connect)   */
#define TCP_TYPE_DELAY_ACK 0x04   /* Socket Type Delayed Acknowledge         */
#define TCP_TYPE_FLOW_CTRL 0x08   /* Socket Type Flow Control                */
#define TCP_TYPE_KEEP_ALIVE 0x10  /* Socket Type Keep Alive                  */
#define TCP_TYPE_CLIENT_SERVER (TCP_TYPE_SERVER | TCP_TYPE_CLIENT)

/* TCP Callback Events */
#define TCP_EVT_CONREQ     0      /* Connect request received event          */
#define TCP_EVT_CONNECT    1      /* Connection established event            */
#define TCP_EVT_CLOSE      2      /* Connection was properly closed          */
#define TCP_EVT_ABORT      3      /* Connection is for some reason aborted   */
#define TCP_EVT_ACK        4      /* Previously send data acknowledged       */
#define TCP_EVT_DATA       5      /* Data received event                     */

/* TCP States */
#define TCP_STATE_FREE     0      /* Entry is free and unused                */
#define TCP_STATE_CLOSED   1      /* Entry allocated, socket still closed    */
#define TCP_STATE_LISTEN   2      /* Socket waiting for incoming connection  */
#define TCP_STATE_SYN_REC  3      /* SYN frame received                      */
#define TCP_STATE_SYN_SENT 4      /* SYN packet sent to establish a connect. */
#define TCP_STATE_FINW1    5      /* Tcp_close started FIN packet was sent   */
#define TCP_STATE_FINW2    6      /* Our FIN ack-ed, waiting for remote FIN  */
#define TCP_STATE_CLOSING  7      /* Received FIN independently of our FIN   */
#define TCP_STATE_LAST_ACK 8      /* Waiting for last ACK for our FIN        */
#define TCP_STATE_TWAIT    9      /* Timed waiting for 2MSL                  */
#define TCP_STATE_CONNECT  10     /* TCP Connection established              */

/* BSD Socket Address Family */
#define AF_UNSPEC          0      /* Unspecified                             */
#define AF_INET            1      /* Internet Address Family (UDP, TCP)      */
#define AF_NETBIOS         2      /* NetBios-style addresses                 */

/* BSD Protocol families, same as address families */
#define PF_UNSPEC          AF_UNSPEC
#define PF_INET            AF_INET
#define PF_NETBIOS         AF_NETBIOS

/* BSD Socket Type */
#define SOCK_STREAM        1      /* Stream Socket (Connection oriented)     */
#define SOCK_DGRAM         2      /* Datagram Socket (Connectionless)        */

/* BSD Socket Protocol */
#define IPPROTO_TCP        1      /* TCP Protocol                            */
#define IPPROTO_UDP        2      /* UDP Protocol                            */

/* BSD Internet Addresses */
#define INADDR_ANY     0x00000000 /* All IP addresses accepted               */
#define INADDR_NONE    0xffffffff /* No IP address accepted                  */

/* BSD Socket Return values */
#define SCK_SUCCESS         0     /* Success                                 */
#define SCK_ERROR         (-1)    /* General Error                           */
#define SCK_EINVALID      (-2)    /* Invalid socket descriptor               */
#define SCK_EINVALIDPARA  (-3)    /* Invalid parameter                       */
#define SCK_EWOULDBLOCK   (-4)    /* It would have blocked.                  */
#define SCK_EMEMNOTAVAIL  (-5)    /* Not enough memory in memory pool        */
#define SCK_ECLOSED       (-6)    /* Connection is closed or aborted         */
#define SCK_ELOCKED       (-7)    /* Socket is locked in RTX environment     */
#define SCK_ETIMEOUT      (-8)    /* Socket, Host Resolver timeout           */
#define SCK_EINPROGRESS   (-9)    /* Host Name resolving in progress         */
#define SCK_ENONAME       (-10)   /* Host Name not existing                  */

/* BSD Socket flags parameter */
#define MSG_DONTWAIT       0x01   /* Enables non-blocking operation          */
#define MSG_PEEK           0x02   /* Peeks at the incoming data              */

/* BSD Socket ioctl commands */
#define FIONBIO            1      /* Set mode (blocking/non-blocking)        */
#define FIO_DELAY_ACK      2      /* Set DELAY_ACK mode for stream socket    */
#define FIO_KEEP_ALIVE     3      /* Set KEEP_ALIVE mode for stream socket   */
#define FIO_FLOW_CTRL      4      /* Set FLOW_CTRL mode for stream socket    */

/* ICMP (ping) Callback Events */
#define ICMP_EVT_SUCCESS   0      /* Pinged Host responded                   */
#define ICMP_EVT_TIMEOUT   1      /* Timeout, no ping response received      */

/* DNS Client Callback Events */
#define DNS_EVT_SUCCESS    0      /* Host name successfully resolved         */
#define DNS_EVT_NONAME     1      /* DNS Error, no such name                 */
#define DNS_EVT_TIMEOUT    2      /* Timeout resolving host                  */
#define DNS_EVT_ERROR      3      /* Erroneous response packet               */

/* DNS 'get_host_by_name()' result codes */
#define DNS_RES_OK         0      /* Resolver successfully started           */
#define DNS_ERROR_BUSY     1      /* Resolver busy, can't process request    */
#define DNS_ERROR_LABEL    2      /* Label in Hostname not valid             */
#define DNS_ERROR_NAME     3      /* Entire Hostname not valid               */
#define DNS_ERROR_NOSRV    4      /* Prim. DNS server not specified (0.0.0.0)*/
#define DNS_ERROR_PARAM    5      /* Invalid parameter                       */

/* SMTP Client Callback Events */
#define SMTP_EVT_SUCCESS   0      /* Email successfully sent                 */
#define SMTP_EVT_TIMEOUT   1      /* Timeout sending email                   */
#define SMTP_EVT_ERROR     2      /* Error when sending email                */

/* FTP Client Commands */
#define FTPC_CMD_PUT       0      /* Puts a file on FTP server               */
#define FTPC_CMD_GET       1      /* Retrieves a file from FTP server        */
#define FTPC_CMD_APPEND    2      /* Append file on FTP server (with create) */
#define FTPC_CMD_DELETE    3      /* Deletes a file on FTP server            */
#define FTPC_CMD_LIST      4      /* Lists files stored on FTP server        */
#define FTPC_CMD_RENAME    5      /* Renames a file on FTP server            */
#define FTPC_CMD_MKDIR     6      /* Makes a directory on FTP server         */
#define FTPC_CMD_RMDIR     7      /* Removes an empty directory on FTP server*/

/* FTP Client Callback Events */
#define FTPC_EVT_SUCCESS   0      /* File operation successful               */
#define FTPC_EVT_TIMEOUT   1      /* Timeout on file operation               */
#define FTPC_EVT_LOGINFAIL 2      /* Login error, username/passw invalid     */
#define FTPC_EVT_NOACCESS  3      /* File access not allowed                 */
#define FTPC_EVT_NOTFOUND  4      /* File not found                          */
#define FTPC_EVT_NOPATH    5      /* Working directory path not found        */
#define FTPC_EVT_ERRLOCAL  6      /* Local file open error                   */
#define FTPC_EVT_ERROR     7      /* Generic FTP client error                */

/* TFTP Client Callback Events */
#define TFTPC_EVT_SUCCESS  0      /* File operation successful               */
#define TFTPC_EVT_TIMEOUT  1      /* Timeout on file operation               */
#define TFTPC_EVT_NOACCESS 2      /* File access not allowed                 */
#define TFTPC_EVT_NOTFOUND 3      /* File not found                          */
#define TFTPC_EVT_DISKFULL 4      /* Disk full (local or remote)             */
#define TFTPC_EVT_ERROR    5      /* Generic TFTP client error               */

/* ARP Cache Entry types */
#define ARP_FIXED_IP       0      /* Fixed IP adrs is refreshed after tout   */
#define ARP_TEMP_IP        1      /* Temp adrs is removed after timeout      */

/* BSD Socket typedef's */
typedef struct sockaddr {         /* << Generic Socket Address structure >>  */
  U16  sa_family;                 /* Address family                          */
  char sa_data[14];               /* Direct address (up to 14 bytes)         */
} SOCKADDR;

#ifdef __CC_ARM
#pragma push
#pragma anon_unions
#endif

typedef struct in_addr {          /* << Generic IPv4 Address structure >>    */
  union {
    struct {
      U8 s_b1,s_b2,s_b3,s_b4;     /* IP address, byte access                 */
    };
    struct {
      U16 s_w1,s_w2;              /* IP address, short int access            */
    };
    U32 s_addr;                   /* IP address in network byte order        */
  };
} IN_ADDR;

#ifdef __CC_ARM
#pragma pop
#endif

typedef struct sockaddr_in {      /* << IPv4 Socket Address structure >>     */
  S16 sin_family;                 /* Socket domain                           */
  U16 sin_port;                   /* Port                                    */
  IN_ADDR sin_addr;               /* IP address                              */
  S8  sin_zero[8];                /* reserved                                */
} SOCKADDR_IN;

typedef struct hostent {          /* << BSD Host Entry structure >>          */
  char *h_name;                   /* Official name of host                   */
  char **h_aliases;               /* Pointer to an array of alias names      */
  S16  h_addrtype;                /* Address Type: AF_INET, AF_NETBIOS       */
  S16  h_length;                  /* Length of address in bytes              */
  char **h_addr_list;             /* Pointer to an array of IPv4 addresses   */
} HOSTENT;

extern void init_TcpNet (void);
extern BOOL main_TcpNet (void);
extern void timer_tick (void);
extern U8   udp_get_socket (U8 tos, U8 opt, 
                            U16 (*listener)(U8 socket, U8 *remip, U16 port, U8 *buf, U16 len));
extern BOOL udp_release_socket (U8 socket);
extern BOOL udp_open (U8 socket, U16 locport);
extern BOOL udp_close (U8 socket);
extern BOOL udp_mcast_ttl (U8 socket, U8 ttl);
extern U8  *udp_get_buf (U16 size);
extern BOOL udp_send (U8 socket, U8 *remip, U16 remport, U8 *buf, U16 dlen);
extern U8   tcp_get_socket (U8 type, U8 tos, U16 tout,
                            U16 (*listener)(U8 socket, U8 event, U8 *buf, U16 len));
extern BOOL tcp_release_socket (U8 socket);
extern BOOL tcp_listen (U8 socket, U16 locport);
extern BOOL tcp_connect (U8 socket, U8 *remip, U16 remport, U16 locport);
extern U8  *tcp_get_buf (U16 size);
extern U16  tcp_max_dsize (U8 socket);
extern BOOL tcp_check_send (U8 socket);
extern U8   tcp_get_state (U8 socket);
extern BOOL tcp_send (U8 socket, U8 *buf, U16 dlen);
extern BOOL tcp_close (U8 socket);
extern BOOL tcp_abort (U8 socket);
extern void tcp_reset_window (U8 socket);
extern BOOL arp_cache_ip (U8 *ipadr, U8 type);
extern BOOL arp_cache_mac (U8 *hwadr);
extern void ppp_listen (const char *user, const char *passw);
extern void ppp_connect (const char *dialnum, const char *user, const char *passw);
extern void ppp_close (void);
extern BOOL ppp_is_up (void);
extern void slip_listen (void);
extern void slip_connect (const char *dialnum);
extern void slip_close (void);
extern BOOL slip_is_up (void);
extern U8   get_host_by_name (U8 *hostn, void (*cbfunc)(U8 event, U8 *host_ip));
extern BOOL smtp_connect (U8 *ipadr, U16 port, void (*cbfunc)(U8 event));
extern void dhcp_disable (void);
extern BOOL igmp_join (U8 *group_ip);
extern BOOL igmp_leave (U8 *group_ip);
extern BOOL snmp_trap (U8 *manager_ip, U8 gen_trap, U8 spec_trap, U16 *obj_list);
extern BOOL snmp_set_community (const char *community);
extern BOOL icmp_ping (U8 *remip, void (*cbfunc)(U8 event));
extern BOOL ftpc_connect (U8 *ipadr, U16 port, U8 command, void (*cbfunc)(U8 event));
extern BOOL tftpc_put (U8 *ipadr, U16 port,
                       const char *src, const char *dst, void (*cbfunc)(U8 event));
extern BOOL tftpc_get (U8 *ipadr, U16 port, 
                       const char *src, const char *dst, void (*cbfunc)(U8 event));

/* BSD Socket API */
extern int  socket (int family, int type, int protocol);
extern int  bind (int sock, const SOCKADDR *addr, int addrlen);
extern int  listen (int sock, int backlog);
extern int  accept (int sock, SOCKADDR *addr, int *addrlen);
extern int  connect (int sock, SOCKADDR *addr, int addrlen);
extern int  send (int sock, const char *buf, int len, int flags);
extern int  sendto (int sock, const char *buf, int len, int flags, SOCKADDR *to, int tolen);
extern int  recv (int sock, char *buf, int len, int flags);
extern int  recvfrom (int sock, char *buf, int len, int flags, SOCKADDR *from, int *fromlen);
extern int  closesocket (int sock);
extern int  getpeername (int sock, SOCKADDR *name, int *namelen);
extern int  getsockname (int sock, SOCKADDR *name, int *namelen);
extern int  ioctlsocket (int sock, long cmd, unsigned long *argp);
extern HOSTENT *gethostbyname (const char *name, int *err);

#endif

#ifdef __cplusplus
}
#endif

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

#endif

