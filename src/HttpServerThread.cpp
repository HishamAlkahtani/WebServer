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
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
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

    std::shared_ptr<spdlog::logger> logger;

public:
    HttpRequestHandler &requestHandler;

    HttpServerThread() : requestHandler(getRequestHandler())
    {
        threadActive.store(false);
        threadNum = numOfThreads++;
        logger = spdlog::stdout_color_mt(std::string("Server Thread ") + std::to_string(threadNum));
    }

    void operator()()
    {
        logger->info("Thread started");

        while (true)
        {
            while (activeSocket.isActive)
            {
                HttpRequest request = activeSocket.recieve();
                std::unique_ptr<HttpResponse> response = requestHandler.createResponsee(request);
                activeSocket.snd(response.get());
                logger->info("\"" + request.getMethod() + " " + request.getRequestedPath() + "\" " + std::to_string(response->getResponseCode()) + " -> " + activeSocket.getIp());
            }

            if (!activeSocket.isActive)
            {
                std::unique_lock<std::mutex> lock(threadActiveMutex);
                threadActive.store(false);
                logger->trace("Thread going to sleep");
                cv.wait(lock);
                logger->trace("Thread woke up");
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