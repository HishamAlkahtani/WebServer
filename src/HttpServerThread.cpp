#pragma once

#include <thread>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <ostream>
#include <unistd.h>
#include <cstring>
#include <memory>
#include "http.hpp"

static int numOfThreads = 0;

class HttpServerThread
{
    int threadNum;
    InternetHttpSocket activeSocket;
    std::thread thread;

    std::string workingDirectory;

    // check if file is under server root
    // returns false if file exists and is out of boudns, false otherwise
    bool safe_path(std::string path)
    {
        char *rp = realpath(path.c_str(), NULL);
        if (!rp)
            return true;
        std::string realPath(rp);
        free(rp);
        std::cout << "User wants realpath: " << realPath << std::endl;
        return true;
    }

    // find file and put it in a response (resolves request and creates response)
    std::unique_ptr<HttpResponse> createResponsee(HttpRequest request)
    {
        if (!request.isGood())
            return std::make_unique<HttpResponse>(400, "BAD REQUEST", "");

        std::string requestPath = request.getRequestedPath();
        if (!safe_path(requestPath))
            return std::make_unique<HttpResponse>(401, "UNAUTHORIZED", "You can't go there");

        if (!std::filesystem::exists(requestPath) || !std::filesystem::is_regular_file(requestPath))
            return std::make_unique<HttpResponse>(404, "NOT FOUND", "File Not Found");

        std::ifstream fl(request.getRequestedPath(), std::ifstream::binary);
        char data[8192]; // why tf did you write this so max file size is 1024?
        // i know you are prototyping and stuff buy by now you should know prototypes don't exist
        // every code you write ends up being used.
        fl.read(data, 8192);
        std::string dstring(data, fl.gcount());
        return std::make_unique<HttpResponse>(200, "OK", dstring); // <- mem leak I know
    }

    void initWorkingDirectory()
    {
        char buff[4095];
        memset(buff, 0, sizeof(char) * 4095);
        getcwd(buff, sizeof(char) * 4095);
        workingDirectory = buff;
    }

public:
    HttpServerThread(InternetHttpSocket socket) : activeSocket(socket)
    {
        threadNum = numOfThreads++;
        initWorkingDirectory();
        thread = std::thread(std::ref(*this));
    }

    void operator()()
    {
        // Do Actual HttpSession Work
        std::cout << "This is thread: " << threadNum << std::endl;

        while (activeSocket.isActive)
        {
            HttpRequest request = activeSocket.recieve();
            std::cout << "Request Recieved!";
            activeSocket.snd(createResponsee(request));
        }
    }

    ~HttpServerThread()
    {
        thread.join();
    }
};