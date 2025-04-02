#include "http/ServerSocket.hpp"

#include <stdexcept>
#include <cstring>

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

ServerSocket::ServerSocket(int port)
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

// TODO: Handle accept returning -1 (don't throw an exception!)
InternetHttpSocket ServerSocket::getConnection()
{
    struct sockaddr_in peeraddr;
    socklen_t peerlen = sizeof(struct sockaddr_in);
    int connection_fd = accept(socket_fd, (sockaddr *)&peeraddr, &peerlen);
    InternetHttpSocket ret = InternetHttpSocket(peeraddr, connection_fd);
    return ret;
}
