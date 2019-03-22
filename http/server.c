#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//监听套接字创建
int create_listenfd(void)
{
    //创建TCP连接
    int fd  = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in sin;
    bzero(&sin,sizeof(sin));
    sin.sin_family=AF_INET;
    sin.sin_port=htons(80);
    sin.sin_addr.s_addr=INADDR_ANY;

    int res=bind(fd,(struct sockaddr*)&sin,sizeof(sin));

    if(res == -1)
    {
        perror("bind");
    }

    listen(fd,100);
    return fd;
}

int main()
{
    //返回套接字描述符
    int sockfd = create_listenfd();
   //接收客户端连接
    int fd = accept(sockfd,NULL,NULL);

    printf("客户端连接成功！\n");


    close(fd);
    close(sockfd);
}



















