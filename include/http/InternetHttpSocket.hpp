#pragma once

#include <string>
#include <netinet/in.h>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "ResponseBodyChunker.hpp"

class InternetHttpSocket
{
    bool active;
    short sin_family;
    unsigned short sin_port;
    std::string ip;
    int connection_fd;

    std::size_t maxRequestSize;

    void closeSocket();

public:
    InternetHttpSocket(sockaddr_in &addr, int fd);

    // for convenience. doesn't create a valid socket.
    InternetHttpSocket();

    std::string getIp();

    HttpRequest receive();

    size_t snd(HttpResponse *response);

    size_t snd(HttpResponse *response, ResponseBodyChunker chunker);

    bool isActive();
};
