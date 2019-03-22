#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

//.server ip端口 端口号
int main(int argc,char* argv[]){
    //进程IP地址 端口号
    if(argc != 3){
        perror("Usage ./server [ip] [port]\n");
        return 1;
    }
    //ipv4 面向字节流 创建socket
    int new_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(new_sock < 0){
        perror("socket");
        return 2;
    }

    struct sockaddr_in server;//绑定的是服务器的IP和端口号

    server.sin_family  = AF_INET;
    server.sin_port = htons(6666);
    server.sin_addr.s_addr =INADDR_ANY;

    int ret = bind(new_sock, (struct sockaddr*)&server ,sizeof(server));
    if(ret < 0){
        perror("bind");
        return 3;
    }

    int lis = listen(new_sock, 5);
    if(lis < 0)
    {
        perror("listen");
        return 4;
    }
    
    printf("listen success...\n");

    struct sockaddr_in client;

    while(1)
    {
        socklen_t len = sizeof(client);
        int client_sock = accept(new_sock, (struct sockaddr*)&client,&len);
        if(client_sock < 0)
        {
            perror("accept");
            continue;
        }

        printf("Get connet[%s]:[%d]\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));
        char buf[BUFSIZ];
        read(client_sock,buf,sizeof(buf)-1);
        printf("%s\n",buf);

        char msg[BUFSIZ] = {0};
        const char* hello = "hello world";
        sprintf(msg,"HTTP/1.1 200 OK\nContent-Length:%lu\n\n%s",strlen(hello),hello);

        //将数据写回客户端
        ssize_t w = write(client_sock, msg, strlen(msg));  
    }

    close(new_sock);
    return 0;
}
