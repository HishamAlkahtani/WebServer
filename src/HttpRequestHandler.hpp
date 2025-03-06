#pragma once

#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include "http.hpp"

class HttpRequestHandler
{
    std::string workingDirectory;

    // Check if file is under server root.
    // Returns false if file exists and is out of bounds, true otherwise.
    bool safePath(std::string path);

    void initWorkingDirectory();

public:
    HttpRequestHandler();

    std::unique_ptr<HttpResponse> createResponsee(HttpRequest request);
};

HttpRequestHandler &getRequestHandler();
