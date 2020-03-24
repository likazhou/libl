#ifndef __SOCKET_H__
#define __SOCKET_H__


#ifdef __cplusplus
extern "C" {
#endif



//Public Typedef
#define FD_SETSIZE    8
#define FD_SET(n, p)  ((p)->fd_bits[(n)/8] |=  (1 << ((n) & 7)))
#define FD_CLR(n, p)  ((p)->fd_bits[(n)/8] &= ~(1 << ((n) & 7)))
#define FD_ISSET(n,p) ((p)->fd_bits[(n)/8] &   (1 << ((n) & 7)))
#define FD_ZERO(p)    rt_memset((void*)(p),0,sizeof(*(p)))

typedef struct fd_set {
	unsigned char fd_bits [(FD_SETSIZE+7)/8];
} fd_set;

struct timeval {
	long    tv_sec;         /* seconds */
	long    tv_usec;        /* and microseconds */
};


//External Functions
int accept(int s, struct sockaddr *addr, socklen_t *addrlen);
int bind(int s, const struct sockaddr *name, socklen_t namelen);
int shutdown(int s, int how);
int getpeername(int s, struct sockaddr *name, socklen_t *namelen);
int getsockname(int s, struct sockaddr *name, socklen_t *namelen);
int getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen);
int setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen);
int closesocket(int s);
int connect(int s, struct sockaddr *name, socklen_t namelen);
int listen(int s, int backlog);
int recv(int s, char *mem, int len, int flags);
int recvfrom(int s, char *mem, int len, int flags, struct sockaddr *from, socklen_t *fromlen);
int send(int s, const char *dataptr, int size, int flags);
int sendto(int s, const char *dataptr, int size, int flags, struct sockaddr *to, socklen_t tolen);
int socket(int domain, int type, int protocol);
int select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout);
int ioctlsocket(int s, long cmd, unsigned long *argp);


#ifdef __cplusplus
}
#endif

#endif

