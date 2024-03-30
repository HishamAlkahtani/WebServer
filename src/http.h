#include <sys/types.h>
#include <sys/socket.h>
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

std::vector<std::string> splitHeaderLines(std::string request) {
    // include the body in the vector? the body is always the last index? 
    // but it would just be confusing, what if someone posts without a body?
    // this is probably broken, double check later.
    std::vector<std::string> result;
    size_t startOfLine = 0; // points to first char in line
    while (request.size()) {
        std::size_t endOfLine = request.find("\r\n", startOfLine); // points to \r
        std::size_t length = endOfLine - startOfLine;
        if (length > 0)
            result.push_back(request.substr(startOfLine, length));
        else
            break;
        startOfLine = endOfLine+2;
    }
    return result;
}

// only uri is parsed currently...
class HttpRequest {
    enum HttpMethod method;
    std::string uri;
    std::string rawRequest;

    public:
        HttpRequest(char* request) {
            rawRequest = std::string(request);
            for (std::string i : splitHeaderLines(rawRequest)) {
                std::cout << i << " and another line: " << std::endl; // looks good
            }
        }

        std::string getRawRequest() { // delete later!
            return rawRequest;
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

        std::string getData() {
            std::string header; 
            header += std::string("HTTP/1.1 ") + std::to_string(responseCode) + " " + responseMessage + CRLF;
            header += "Content-Length: " + std::to_string(body.length()) + CRLF;
            // TODO: add remaining headers from vector
            header += CRLF;
            std::string message = header + body;
            return message;
        }

        void addResponseHeader(std::string key, std::string value) {
            // TODO: add to vector
        }
};

class InternetHttpSocket {
    // what about connection: keep-alive
    // should it be checked and implementation should differ if it's not requestseed?
    // or should it be assumed that we will always keep-alive, does it make a difference
    // implementation wise? the client can close the connection if it wants idont carde
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
            std::string message = response.getData();
            return send(connection_fd, message.c_str(), message.length() * sizeof(char), 0);
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
