#pragma once

#include <thread>
#include <vector>
#include "HttpServerThread.cpp"
#include "http.hpp"

class LoadBalancer
{
    HttpServerThread *threadPool;

    InternetHttpSocket *clientQueue;
    int queueHead;
    int queueTail;
    int queueLength;

    std::thread loadBalancerThread;

    int maxThreads;
    int activeThreads;

public:
    LoadBalancer(int maxThreads) : maxThreads(maxThreads)
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
        std::cout << "Load Balancing Thread Started!" << std::endl;

        while (true)
        {
            if (queueHead == queueTail)
            {
                // This might be a bottleneck, measure performance and see if it needs
                // something more sophisticated, maybe a condition variable?
                // std::cout << "LBThread sleeping because n oclients" << queueHead << queueTail << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }

            bool clientAssigned = false;
            for (int i = 0; i < activeThreads; i++)
            {
                if (!threadPool[i].isActive)
                {
                    threadPool[i].assignClient(clientQueue[queueHead]);
                    clientAssigned = true;
                    std::cout << "Client assigned to thread #" << i << std::endl;
                }
            }

            if (!clientAssigned && activeThreads < maxThreads)
            {
                threadPool[activeThreads].assignClient(clientQueue[queueHead]);
                threadPool[activeThreads++].startThread();
                clientAssigned = true;
                std::cout << "Client assigned to thread #" << activeThreads - 1 << std::endl;
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
