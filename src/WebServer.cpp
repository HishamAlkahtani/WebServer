#pragma once
#include <iostream>
#include <string.h>
#include <unistd.h>
#include "http.hpp"
#include "LoadBalancer.cpp"

class WebServer
{
    ServerSocket serverSocket;
    LoadBalancer loadBalancer;

public:
    WebServer(int port, int maxThreads) : serverSocket(port), loadBalancer(maxThreads)
    {
    }

    // blocking!
    void start()
    {
        while (true)
        {
            std::cout << "New Connection" << std::endl;
            InternetHttpSocket clientSocket = serverSocket.getConnection();
            loadBalancer.enqueu_socket(clientSocket);
        }
    }
};

// Gets files for user requests
// 1- Checks if the path of the file is within the allowed directory
// 2- Handles reading and caching  files.
class FileManager
{
};