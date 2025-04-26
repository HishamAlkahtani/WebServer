#include "HttpRequestHandler.hpp"
#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"
#include "http/ResponseBodyChunker.hpp"

#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>

#include <cstdlib>
#include <cstring>

#include <unistd.h>

std::unique_ptr<HttpResponse> HttpRequestHandler::GET(HttpRequest &request)
{
    std::string requestPath = request.getResourcePath();
    if (!request.resourceExists() || !std::filesystem::exists(requestPath) || !std::filesystem::is_regular_file(requestPath))
        return std::make_unique<HttpResponse>(404, "NOT FOUND", "File Not Found");

    std::filesystem::path path(requestPath);
    std::uintmax_t fileSize = std::filesystem::file_size(path);

    if (fileSize > maxResponseSize)
        return std::make_unique<HttpResponse>(500, "FILE TOO LARGE", "Requested resouce is larger than the server defined limit.");

    if (fileSize > CHUNKED_TRANSFER_THRESHOLD)
        return std::make_unique<HttpResponse>(200, "OK", ChunkedTransfer{}, requestPath);

    std::ifstream fl(requestPath, std::ifstream::binary);

    char *data = new char[fileSize + 1]();

    fl.read(data, fileSize + 1);
    std::string dstring(data, fl.gcount());

    delete[] data;

    return std::make_unique<HttpResponse>(200, "OK", dstring);
}

bool HttpRequestHandler::safePath(std::string path)
{
    if (!std::filesystem::exists(path))
        return true;
    // path should start with workingDirectory
    return path.substr(0, workingDirectory.length()) == workingDirectory;
}

void HttpRequestHandler::initWorkingDirectory()
{
    char buff[4095];
    memset(buff, 0, sizeof(char) * 4095);
    getcwd(buff, sizeof(char) * 4095);
    workingDirectory = buff;
}

HttpRequestHandler::HttpRequestHandler() : config(Config::getConfigs()), maxRequestSize(config.getMaxRequestSize()),
                                           maxResponseSize(config.getMaxResponseSize())
{
    initWorkingDirectory();

    // Registering supported method handlers
    handlers["GET"] = [this](HttpRequest &request) -> std::unique_ptr<HttpResponse>
    { return this->GET(request); };
}

std::unique_ptr<HttpResponse> HttpRequestHandler::createResponse(HttpRequest &request)
{
    if (!request.isGood())
    {
        return std::make_unique<HttpResponse>(400, "BAD REQUEST", "");
    }

    if (!safePath(request.getResourcePath()))
        return std::make_unique<HttpResponse>(401, "UNAUTHORIZED", "You can't go there");

    return handlers[request.getMethod()](request);
}

HttpRequestHandler &getRequestHandler()
{
    static HttpRequestHandler handler;
    return handler;
}
