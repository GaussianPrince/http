#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<netdb.h>
#include<fcntl.h>
struct addrinfo *http_dns_parse(char *url)
{
	struct addrinfo *result,hint;
	if(!url)
		return 0;
	hint.ai_family=AF_UNSPEC;
	hint.ai_socktype=SOCK_STREAM;
	hint.ai_flags =AI_ALL;
	hint.ai_protocol = IPPROTO_TCP;
	if(0!=getaddrinfo(url,0,&hint,&result))
		return 0;
	return result;
}
int http_server_create(struct sockaddr_in* addr)
{
	int sock;
	if(!addr)
		return -1;
	sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock<0)
		return -1;
	if(-1==bind(sock,(struct sockaddr*)addr,sizeof(struct sockaddr_in)))
		return -1;
	return sock;
}
int http_client_create(struct sockaddr_in*addr)
{
	int sock;
	int flags;
	struct timeval tm;
	fd_set set;
	int len;
	int error;
	if(!addr)
		return -1;
	sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock<0)
		return -1;
	flags = fcntl(sock, F_GETFL, 0);  
	fcntl(sock, F_SETFL, flags|O_NONBLOCK);  
	if(connect(sock,(struct sockaddr*)addr,sizeof(struct sockaddr_in))<0){
		tm.tv_sec=3;
		tm.tv_usec=0;
		FD_ZERO(&set);
		FD_SET(sock,&set);
		if(select(sock+1,0,&set,0,&tm)<0)
			return -1;
		len=sizeof(int);
		getsockopt(sock,SOL_SOCKET,SO_ERROR,&error,&len);
		if(0!=error)
			return -1;
	}
	return sock;
}
