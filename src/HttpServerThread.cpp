#pragma once

#include <thread>
#include <iostream>
#include <fstream>
#include <ostream>
#include <unistd.h>
#include <cstring>
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include "http.hpp"
#include "HttpRequestHandler.hpp"

static int numOfThreads = 0;

class HttpServerThread
{
    int threadNum;

    InternetHttpSocket activeSocket;
    std::thread thread;
    bool threadStarted;

    std::mutex threadActiveMutex;
    std::condition_variable cv;
    // shared between server threads and load balancer thread
    std::atomic<bool> threadActive;

public:
    // this flag is just for prototyping, this should be changed later to something
    // thread safe, probably condition_variable so the thread can sleep waiting for it
    // right now without sleeping mechanisms, threads waste CPU cycles waiting for clients
    HttpRequestHandler &requestHandler;

    // DEFAULT CONSTRUCTOR DOES NOT CREATE A THREAD! OBJECTS CREATED BY
    // THE DEFAULT CONSTRUCTOR SHOULD BE DISCARDED!
    HttpServerThread() : requestHandler(getRequestHandler())
    {
        threadActive.store(false);
        threadNum = numOfThreads++;
    }

    void operator()()
    {
        while (true)
        {
            while (activeSocket.isActive)
            {
                HttpRequest request = activeSocket.recieve();
                std::cout << "Request Recieved!";
                activeSocket.snd(requestHandler.createResponsee(request));
            }

            if (!activeSocket.isActive)
            {
                std::unique_lock<std::mutex> lock(threadActiveMutex);
                threadActive.store(false);
                std::cout << "Thread #" << threadNum << " will sleep" << std::endl;
                cv.wait(lock);
                std::cout << "Thread #" << threadNum << " woke up" << std::endl;
            }
        }
    }

    void assignClient(InternetHttpSocket socket)
    {
        activeSocket = socket;
        {
            std::lock_guard<std::mutex> lock(threadActiveMutex);
            threadActive.store(activeSocket.isActive);
        }
        cv.notify_all();
    }

    void startThread()
    {
        thread = std::thread(std::ref(*this));
        threadStarted = true;
    }

    bool threadHasStarted()
    {
        return threadStarted;
    }

    bool isThreadActive()
    {
        return threadActive.load();
    }

    ~HttpServerThread()
    {
        thread.join();
    }
};