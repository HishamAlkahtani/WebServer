#pragma once

#include <thread>
#include <vector>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "HttpServerThread.cpp"
#include "http.hpp"

class LoadBalancer
{
    HttpServerThread *threadPool;
    int maxThreads;
    int activeThreads;

    InternetHttpSocket *clientQueue;
    int queueHead;
    int queueTail;
    int queueLength;

    std::thread loadBalancerThread;

    std::shared_ptr<spdlog::logger> logger;

public:
    LoadBalancer(int maxThreads) : maxThreads(maxThreads), logger(spdlog::stdout_color_st("Load Balancer"))
    {
        queueLength = 1024;
        activeThreads = 0;
        queueHead = 0;
        queueTail = 0;
        activeThreads = 0;

        // queue size is 1024 for now, could be configurable later
        clientQueue = new InternetHttpSocket[queueLength]();
        threadPool = new HttpServerThread[maxThreads]();
        loadBalancerThread = std::thread(std::ref(*this));
    }

    // The actual assigning of threads will be performed on it's own thread.
    // Takes clients from head of queue, and assigns them to available threads
    void operator()()
    {
        logger->info("Load balancing thread started");

        while (true)
        {
            if (queueHead == queueTail)
            {
                // This might be a bottleneck, measure performance and see if it needs
                // something more sophisticated, maybe a condition variable?
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
                continue;
            }

            bool clientAssigned = false;
            for (int i = 0; i < activeThreads && !clientAssigned; i++)
            {
                if (!threadPool[i].isThreadActive())
                {
                    threadPool[i].assignClient(clientQueue[queueHead]);
                    clientAssigned = true;
                    logger->debug(std::string("Client assigned to thread #") + std::to_string(i));
                }
            }

            if (!clientAssigned && activeThreads < maxThreads)
            {
                logger->debug("All threads are busy, starting new thread");
                threadPool[activeThreads].assignClient(clientQueue[queueHead]);
                threadPool[activeThreads++].startThread();
                clientAssigned = true;
                logger->debug(std::string("Client assigned to thread #") + std::to_string(activeThreads - 1));
            }

            if (clientAssigned)
                queueHead = (queueHead + 1) % queueLength;
        }
    }

    void
    enqueu_socket(InternetHttpSocket socket)
    {
        bool clientAdded = false;
        while (!clientAdded)
        {
            if ((queueTail + 1) % queueLength == queueHead)
            {
                // to prevent adding when queue is full...
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            else
            {
                clientQueue[queueTail] = socket;
                queueTail = (queueTail + 1) % queueLength;
                clientAdded = true;
            }
        }
    }
};
