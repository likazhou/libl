#if TCPPS_TYPE == TCPPS_T_KEILTCP
#include <net/rtxip/rtxip.h>
#include <net/rtxip/sockets.h>


int accept(int s, struct sockaddr *addr, socklen_t *addrlen);

int bind(int s, const struct sockaddr *name, socklen_t namelen)
{
	struct sockaddr_in *addr = (struct sockaddr_in *)name;

	addr->sin_port = ntohs(addr->sin_port);
	return net_Bind(s, name, namelen);
}

int shutdown(int s, int how);

int getpeername (int s, struct sockaddr *name, socklen_t *namelen)
{

	return net_IsConnect(s) ? 0 : 1;
}

int getsockname (int s, struct sockaddr *name, socklen_t *namelen);

int getsockopt (int s, int level, int optname, void *optval, socklen_t *optlen);

int setsockopt (int s, int level, int optname, const void *optval, socklen_t optlen);

int closesocket(int s)
{

	return net_Close(s);
}

int connect(int s, struct sockaddr *name, socklen_t namelen)
{
	struct sockaddr_in *addr = (struct sockaddr_in *)name;

	addr->sin_port = ntohs(addr->sin_port);
	return net_Connect(s, name, namelen);
}

int listen(int s, int backlog)
{

	return net_Listen(s, backlog);
}

int recv(int s, char *mem, int len, int flags)
{
	if (len)
		return net_Recv(s, mem, len);

	return net_IsConnect(s) ? 0 : 1;
}

int recvfrom(int s, char *mem, int len, int flags, struct sockaddr *from, socklen_t *fromlen);

int send(int s, const char *dataptr, int size, int flags)
{

	return net_Send(s, (void *)dataptr, size);
}

int sendto(int s, const char *dataptr, int size, int flags, struct sockaddr *to, socklen_t tolen);

int socket(int domain, int type, int protocol)
{

	return net_Socket(domain, type, protocol);
}

int select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout);

int ioctlsocket(int s, long cmd, unsigned long *argp)
{

	return 0;
}

#endif
