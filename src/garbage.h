#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <stdexcept>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

class InternetSocketAddress {
    short sin_family;
    unsigned short sin_port;
    std::string ip;
    int connection_fd;

    public:
        InternetSocketAddress(sockaddr_in& addr, int fd) {
            sin_family = addr.sin_family;
            sin_port = addr.sin_port;
            ip = inet_ntoa(addr.sin_addr);
            connection_fd = fd;
        }

        std::string getIp() {
            return ip;
        }

        int getConnectionFileDescriptor() {
            return connection_fd;
        }
};

class ServerSocket {
    
    int socket_fd;


    public:
        ServerSocket(int port) {
            socket_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (socket_fd == -1)
                throw std::runtime_error("Could not obtain socket file descriptor");
            
            struct sockaddr_in addr;
            memset(&addr, 0, sizeof(struct sockaddr_in));
            addr.sin_family = AF_INET;
            addr.sin_port = htons(port);
            addr.sin_addr.s_addr = INADDR_ANY; 

            if (bind(socket_fd, (sockaddr*)&addr, sizeof(struct sockaddr_in)) == -1)
                throw std::runtime_error("call to bind failed");

            if (listen(socket_fd, 1) == -1)
                throw std::runtime_error("call to listen failed");

            std::cout << "socket created!\n";
        }

        InternetSocketAddress getConnection() {
            struct sockaddr_in peeraddr;
            socklen_t peerlen = sizeof(struct sockaddr_in);
            int connection_fd = accept(socket_fd, (sockaddr*)&peeraddr, &peerlen);  \
            std::cout << "Connection established!\n";
            return InternetSocketAddress(peeraddr, connection_fd);
        }
};