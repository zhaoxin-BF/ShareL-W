#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
//c的头文件
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

//监听套接字创建
int create_listenfd(void)
{
    //sbla,shabila
    //创建套接字
    int fd = socket(AF_INET, SOCK_STREAM,0);
    //绑定地址
    struct sockaddr_in sin;
    bzero(&sin,sizeof(sin));//清空
    sin.sin_family=AF_INET;
    sin.sin_port=htons(80);//80是http默认端口
    sin.sin_addr.s_addr=INADDR_ANY;//监听任意网卡
    
    //绑定
    int res = bind(fd,(struct sockaddr*)&sin,sizeof(sin));
    if(res == -1)
    {
        perror("bind error!");//将错误打出来
    }
    
    //监听套接字
    listen(fd,100);

    return fd;
}

//处理客户端的请求
void handle_request(int fd)//参数是一个文件描述符
{
    //客户端会发送一个字符串请求过来
    char buffer[1024*1024] = {0};
    int nread = read(fd,buffer,sizeof(buffer));
    printf("读到的请求是\n%s\n",buffer);

    //从请求里解析出文件名来
    char filename[10] = {0};
    sscanf(buffer,"GET /%s",filename);//字符串解析
    printf("解析的文件名是%s\n",filename);

    //根据文件名获得mime类型，放入响应头中，告诉浏览器我发给你的是什么类型的文件
    char *mime = NULL;
    if(strstr(filename,".html"))
        mime="text/html";//文本类型
    else
    if(strstr(filename,".html"))
        mime="image/jpg";//图片类型，这些是规定的

    //打开文件，读取内容，构建响应，发回给客户端
    char response[1024*1024] = {0};
    sprintf(response,"HTTP/1.1 200 OK\r\n Content-Type: %s\r\n\r\n",mime);
    int handlen = strlen(response);

    int filefd = open(filename,O_RDONLY);
    int filelen = read(filefd,response+handlen,sizeof(response)-handlen);

    //发送响应头+内容
    write(fd,response,handlen+filelen);
    close(filefd);
}


int main()
{
    //1创建监听套接字，返回的是套接字的描述符
    int sockfd = create_listenfd();

    while(1)
    {
        //2接收客户端连接
        int fd = accept(sockfd,NULL,NULL);
        printf("有客户端连接！\n");

        //3处理客户端发来的请求
        handle_request(fd);//需要这里的参数fd    
    
        close(fd);
    }
    close(sockfd);

    return 0;
}


