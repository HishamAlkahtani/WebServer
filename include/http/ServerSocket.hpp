#pragma once

#include "InternetHttpSocket.hpp"

class ServerSocket
{
    int socket_fd;

public:
    ServerSocket(int port);

    ~ServerSocket();

    InternetHttpSocket getConnection();
};
