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

enum HttpMethod {
    GET, POST, PUT, DELETE
};

class HttpRequest {
    enum HttpMethod method;

    public:
        HttpRequest(char* request) {

        }
};

class HttpResponse {
    int responseCode;
    std::string responseMessage;

    public:

        char* getData(size_t& size) {
            
        }
};

class InternetHttpSocket {
    short sin_family;        // AF_INET always
    unsigned short sin_port; 
    std::string ip;          
    int connection_fd;       

    public:
        InternetHttpSocket(sockaddr_in& addr, int fd) {
            sin_family = addr.sin_family;
            sin_port = addr.sin_port;
            ip = inet_ntoa(addr.sin_addr);
            connection_fd = fd;
        }

        ~InternetHttpSocket() {
            close(connection_fd);
        }

        std::string getIp() {
            return ip;
        }

        HttpRequest recieve() {
            char* buffer = new char[2048];
            recv(connection_fd, (void*) buffer, 2048, 0);
            HttpRequest request(buffer);
            delete[] buffer;
            return request;
        }

        size_t snd(HttpResponse& response) {
            size_t size;
            char* data = response.getData(size);
            return send(connection_fd, (void*) data, size, 0);
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
                throw std::runtime_error("Failed to bind socket to port!");

            if (listen(socket_fd, 1) == -1)
                throw std::runtime_error("call to listen failed");
        }

        ~ServerSocket() {
            close(socket_fd);
        }

        // TODO: Handle accept returning -1 (don't throw an exception!)
        InternetHttpSocket getConnection() {
            struct sockaddr_in peeraddr;
            socklen_t peerlen = sizeof(struct sockaddr_in);
            int connection_fd = accept(socket_fd, (sockaddr*)&peeraddr, &peerlen);
            return InternetHttpSocket(peeraddr, connection_fd);
        }
};
