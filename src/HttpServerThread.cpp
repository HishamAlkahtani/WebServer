#pragma once

#include <thread>
#include <iostream>
#include <fstream>
#include <ostream>
#include <unistd.h>
#include <cstring>
#include <memory>
#include <atomic>
#include "http.hpp"
#include "HttpRequestHandler.hpp"

static int numOfThreads = 0;

class HttpServerThread
{
    InternetHttpSocket activeSocket;
    std::thread thread;
    bool threadStarted;

public:
    // this flag is just for prototyping, this should be changed later to something
    // thread safe, probably condition_variable so the thread can sleep waiting for it
    // right now without sleeping mechanisms, threads waste CPU cycles waiting for clients
    std::atomic<bool> threadActive;
    HttpRequestHandler &requestHandler;

    // DEFAULT CONSTRUCTOR DOES NOT CREATE A THREAD! OBJECTS CREATED BY
    // THE DEFAULT CONSTRUCTOR SHOULD BE DISCARDED!
    HttpServerThread() : requestHandler(getRequestHandler())
    {
        threadActive.store(false);
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
                threadActive.store(false);
        }
    }

    void assignClient(InternetHttpSocket socket)
    {
        activeSocket = socket;
        threadActive.store(activeSocket.isActive);
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

    ~HttpServerThread()
    {
        thread.join();
    }
};