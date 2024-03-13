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
#include <memory>
#include <vector>
#include <cstring>

#define CRLF std::string("\r\n")

enum HttpMethod {
    GET, POST, PUT, DELETE
};

class HttpRequest {
    enum HttpMethod method;
    std::string uri;
    char* rawRequest; // temporary!!

    public:
        HttpRequest(char* request) {
            // TODO: Parse HTTP.
            rawRequest = new char[2048];
            std::memcpy(rawRequest, request, 2048);
        }

        char* getRawRequest() { // delete later!
            return rawRequest;
        }

        ~HttpRequest() {
            delete[] rawRequest;
        }
};

class HttpResponse {
    int responseCode;
    std::string responseMessage;
    std::string body;
    std::vector<std::string>  headers;

    public:
        HttpResponse(int responseCode, std::string responseMessage, std::string body) 
        : responseCode(responseCode), responseMessage(responseMessage), body(body)
        { }
        
        const char* getData(size_t& size) {
            std::string header; 
            header += "HTTP/1.1 " + std::to_string(responseCode) + " " + responseMessage + CRLF;
            header += "Content-Length: " + std::to_string(body.length()) + CRLF;
            // TODO: add remaining headers from vector
            header += CRLF;
            std::cout << "HEader! \n" << header << std::endl;
            std::string message = header + body;
            size = message.length();
            std::cout << "Sending message: \n" << message;
            const char* r = message.c_str();
            printf("PFR ! \n%s\n", r);
            return r;
        }

        void addResponseHeader(std::string key, std::string value) {
            // TODO: add to vector
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
            HttpRequest request(buffer); // change to unique pointer? oh this is starting to get messy
            delete[] buffer;
            return request;
        }

        size_t snd(HttpResponse& response) {
            size_t size;
            const char* data = response.getData(size);
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
