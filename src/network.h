/* 
    Thin C++ wrapper over C linux socket API
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <stdexcept>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

class InternetSocket {
    short sin_family;        // AF_INET always
    unsigned short sin_port; 
    std::string ip;          
    int connection_fd;       

    public:
        InternetSocket(sockaddr_in& addr, int fd) {
            sin_family = addr.sin_family;
            sin_port = addr.sin_port;
            ip = inet_ntoa(addr.sin_addr);
            connection_fd = fd;
        }

        ~InternetSocket() {
            close(connection_fd);
        }

        std::string getIp() {
            return ip;
        }

        /* Reads up to size bytes, returns number of 
           bytes written to buffer */
        int recieve(void* buffer, size_t size) {
            return recv(connection_fd, buffer, size, 0);
        }

        size_t snd(void* buffer, size_t size) {
            int flag = 1;
            setsockopt(connection_fd, IPPROTO_TCP, TCP_NODELAY, (void*) &flag, sizeof(int));
            size_t bytesSent = send(connection_fd, buffer, size, 0);
            flag = 0;
            setsockopt(connection_fd, IPPROTO_TCP, TCP_NODELAY, (void*) &flag, sizeof(int));
            return bytesSent;
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
        }

        ~ServerSocket() {
            close(socket_fd);
        }

        // TODO: Handle accept returning -1 (don't throw an exception!)
        InternetSocket getConnection() {
            struct sockaddr_in peeraddr;
            socklen_t peerlen = sizeof(struct sockaddr_in);
            int connection_fd = accept(socket_fd, (sockaddr*)&peeraddr, &peerlen);
            return InternetSocket(peeraddr, connection_fd);
        }
};