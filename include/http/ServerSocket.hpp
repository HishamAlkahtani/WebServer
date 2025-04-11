#pragma once

#include "InternetHttpSocket.hpp"

#include <spdlog/spdlog.h>

class ServerSocket
{
    int socket_fd;

    std::shared_ptr<spdlog::logger> logger;

public:
    ServerSocket(int port);

    ~ServerSocket();

    InternetHttpSocket getConnection();
};
