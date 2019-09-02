#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/epoll.h>
#include<arpa/inet.h>
#include<netdb.h>
#include"http_sock.h"
void *http_request(void *argv)
{
	char buffer[1024];
	int len;
	int sock=*(int*)argv;
	len=snprintf(buffer,1024,"GET / HTTP/1.1\r\nHost: www.hackr.jp\r\n"
		"\r\n");
	while(1){
		if(send(sock,buffer,len,0)<0)
			printf("send get method failed\n");
		sleep(1);
	}
	return 0;
}
int main(int argc, char*argv[])
{
	int client_sock;
	struct addrinfo *server_ip_list,*n;
	int epsock;
	struct epoll_event  ev,events[16]; 
	int epres;
	char buffer[1024];
	int i;
	pthread_t td;
	char str[16];
	if(argc < 2){
		printf("usage: ./client <url>\n");
		return -1;
	}
	server_ip_list=http_dns_parse(argv[1]);
	if(0==server_ip_list){
		printf("get server list error\n");
		return -1;
	}
	epsock=epoll_create(1);
	if(epsock<0){
		printf("epoll_create failed\n");
		return -1;
	}
	for(n=server_ip_list;n;n=n->ai_next){
		inet_ntop(AF_INET,&((struct sockaddr_in*)server_ip_list->ai_addr)->sin_addr,str, sizeof(str));
		((struct sockaddr_in*)server_ip_list->ai_addr)->sin_port=htons(8080);
		printf(" %s  %d\n",str,ntohs(((struct sockaddr_in*)server_ip_list->ai_addr)->sin_port));
		client_sock=http_client_create((struct sockaddr_in*)server_ip_list->ai_addr);
		if(client_sock<0){
			printf("create client sock error\n");
			return -1;
		}
		if(0!=pthread_create(&td,0,http_request,&client_sock)){
			printf("create thread error\n");
			return -1;
		}
		ev.data.fd=client_sock;
		ev.events=EPOLLIN|EPOLLET;
		if(epoll_ctl(epsock,EPOLL_CTL_ADD,client_sock,&ev)<0){
			printf("epoll_ctl failed\n");
			return -1;
		}
	}
	while(1){
		printf("Entry:\r\n");
		epres=epoll_wait(epsock,events,16,-1);
		printf("\nepres:%d\n",epres);
		if(epres>0){
			for(i=0;i<epres;i++){
				while(recv(events[i].data.fd,buffer,1024,0)>0){
					printf("\n%s\n",buffer);
				}
			}
		}
	}
	freeaddrinfo(server_ip_list);
	return 0;
}