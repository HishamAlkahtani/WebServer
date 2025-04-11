#include "http/ServerSocket.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>

#include <stdexcept>
#include <cstring>

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

ServerSocket::ServerSocket(int port) : logger(spdlog::stdout_color_st("Server Socket"))
{
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
        throw std::runtime_error("Could not obtain socket file descriptor");

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(socket_fd, (sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1)
        throw std::runtime_error("Failed to bind socket to port!");

    if (listen(socket_fd, 1) == -1)
        throw std::runtime_error("call to listen failed");
}

ServerSocket::~ServerSocket()
{
    close(socket_fd);
}

InternetHttpSocket ServerSocket::getConnection()
{
    struct sockaddr_in peeraddr;
    socklen_t peerlen = sizeof(struct sockaddr_in);
    int connection_fd;

    do
    {
        connection_fd = accept(socket_fd, (sockaddr *)&peeraddr, &peerlen);
        if (connection_fd == -1)
            logger->debug("Failed to connect, accept returned -1. ERRNO: " + std::to_string(errno));
    } while (connection_fd == -1);

    return InternetHttpSocket(peeraddr, connection_fd);
}
