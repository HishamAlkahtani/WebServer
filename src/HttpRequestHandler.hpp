#pragma once

#include <filesystem>
#include <fstream>
#include <memory>
#include <functional>
#include <map>
#include <string>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include "http.hpp"

class HttpRequestHandler
{
    using HttpHandlerFunction = std::function<std::unique_ptr<HttpResponse>(HttpRequest &)>;
    std::map<std::string, HttpHandlerFunction> handlers;

    std::unique_ptr<HttpResponse> GET(HttpRequest &request);
    std::unique_ptr<HttpResponse> HEAD(HttpRequest &request);
    std::unique_ptr<HttpResponse> POST(HttpRequest &request);
    std::unique_ptr<HttpResponse> PUT(HttpRequest &request);
    std::unique_ptr<HttpResponse> DELETE_METHOD(HttpRequest &request); // "DELETE" is a C++ keyword
    std::unique_ptr<HttpResponse> CONNECT(HttpRequest &request);
    std::unique_ptr<HttpResponse> OPTIONS(HttpRequest &request);
    std::unique_ptr<HttpResponse> TRACE(HttpRequest &request);
    std::unique_ptr<HttpResponse> PATCH(HttpRequest &request);

    std::string workingDirectory;

    /**
     * @brief Check if file is under server root.
     * @return false if file exists and is out of bounds, true otherwise.
     *
     * If the resource is to be created, the handler function must check that the
     * created resource is under server root.
     */
    bool safePath(std::string path);

    void initWorkingDirectory();

public:
    HttpRequestHandler();

    std::unique_ptr<HttpResponse> createResponse(HttpRequest &request);
};

HttpRequestHandler &getRequestHandler();
