#include "httpserver.hpp"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        std::cout<<"Usage: ./main port\n";
        return -1;
    }
    int port = atoi(argv[1]);
    HttpServer server;
    server.ServerInit(port);
    server.ServerStart();
    return 0;
}
