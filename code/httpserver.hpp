#ifndef __M_HS_H__
#define __M_HS_H__
#include "threadpool.hpp"
#include "utils.hpp"
#include <string>
#include <stdio.h>
#include <errno.h>
#include <signal.h>

class HttpServer
{
    private:
        int _serv_sock;
        ThreadPool *_tp;
    private:
        static bool HttpHandler(int sock) {
            HttpHeader hdrs;
            HttpRequest req(sock);
            HttpResponse rsp(sock);

            //接收并解析http请求
            CHECK_OUT(req.RecvHttpHeader(&hdrs));
            CHECK_OUT(req.HttpHeaderParse(&hdrs));
            //对解析的头信息进行检测是否合法
            CHECK_OUT(hdrs.HeaderIsLegal());

            //判断是否是CGI请求；根据不同请求做不同响应
            if (hdrs.RequestIsCGI()) {
                CHECK_OUT(rsp.CGIHandler(&hdrs));
            }else {
                CHECK_OUT(rsp.FileHandler(&hdrs));
            }

            //响应完毕则关闭连接
            shutdown(sock, SHUT_RDWR);
            close(sock);
            return true;
        out:
            //http处理出错则返回错误信息，并关闭连接
            rsp.ErrHandler(&hdrs);
            shutdown(sock, SHUT_RDWR);
            close(sock);
            return true;
        }
    public:
        HttpServer():_serv_sock(-1), _tp(NULL) {}
        ~HttpServer() {
            if (_serv_sock != -1) {
                close(_serv_sock);
                _serv_sock = -1;
            }
            if (_tp) {
                delete _tp;
                _tp = NULL;
            }
        }
        bool ServerInit(int port) {
            signal(SIGPIPE, SIG_IGN);

            _serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (_serv_sock < 0) {
                perror("sock error");
                return -1;
            }

            int opt = 1;
            setsockopt(_serv_sock, SOL_SOCKET, SO_REUSEADDR, (void*)&opt, sizeof(int));

            sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = htons(port);
            addr.sin_addr.s_addr = INADDR_ANY;
            socklen_t len = sizeof(sockaddr_in);

            if (bind(_serv_sock, (sockaddr*)&addr, len) < 0) {
                perror("bind error");
                return false;
            }

            if (listen(_serv_sock, 5) < 0) {
                perror("listen error");
                return false;
            }

            _tp = new ThreadPool();
            if (_tp->TP_PoolInit() == false) {
                return false;
            }
            return true;
        }
        bool ServerStart() {
            sockaddr_in cli_addr;
            int cli_sock= -1;
            while(1) {
                socklen_t len = sizeof(sockaddr_in);
                cli_sock = accept(_serv_sock, (sockaddr*)&cli_addr, &len);
                if (cli_sock < 0) {
                    perror("accept new client error\n");
                    continue;
                }
                ThreadTask *tt = new ThreadTask(cli_sock, HttpHandler);
                _tp->TP_PushTask(tt);
            }
            _tp->TP_Quit();
            return true;
        }
};

#endif
