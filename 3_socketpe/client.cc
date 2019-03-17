#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define BUFSIZE 800
#define SERV_PORT 6666
#define SERV_IP "172.17.98.161"

using namespace std;

int main(int argc,char *argv[])
{
	struct sockaddr_in serv_addr;
	//socket_t serv_addr_len;
	int sockfd,n;
	char buf[BUFSIZE];

	sockfd = socket(AF_INET,SOCK_STREAM,0);
	
	memset(&serv_addr,0,sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET,SERV_IP,&serv_addr.sin_addr.s_addr);

	connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
	while(1)
	{
		fgets(buf,sizeof(buf),stdin);//hello world --- fgets -->"hello world\n\0"

		write(sockfd,buf,strlen(buf));

		n=read(sockfd,buf,strlen(buf));

		write(STDOUT_FILENO,buf,n);
	}
	close(sockfd);
	return 0;
	}

