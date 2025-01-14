#pragma once

#include <thread>
#include <vector>
#include "HttpServerThread.cpp"
#include "http.hpp"

// TODO: Client Queue; single producer single consumer ring buffer

class LoadBalancer
{
    std::vector<HttpServerThread *> clientThreads;
    std::thread loadBalancerThread;

    int maxThreads;

public:
    LoadBalancer(int maxThreads) : maxThreads(maxThreads)
    {
        loadBalancerThread = std::thread(std::ref(*this));
    }

    // The actual assigning of threads will be performed on it's own threads.
    void operator()()
    {
        // std::cout << "Load Balancing Thread Started!" << std::endl;
    }

    void enqueu_socket(InternetHttpSocket socket)
    {
        std::cout << "Client enqueu call" << std::endl;

        // temp implementation until queue is implemeneted new thread per request.
        // shit's leaky

        HttpServerThread *thread = new HttpServerThread(socket);
        clientThreads.push_back(thread);
    }
};
