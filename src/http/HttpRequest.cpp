#include "http/HttpRequest.hpp"
#include "http/split.hpp"

#include <string>
#include <vector>

HttpRequest::HttpRequest(char *request)
{
    goodness = true;
    rawRequest = std::string(request);
    std::vector<std::string> requestLines = split(rawRequest, CRLF);

    if (requestLines.size() == 0)
    {
        goodness = false;
        return;
    }

    std::vector<std::string> firstLine = split(requestLines[0], " ");

    if (firstLine.size() != 3)
    {
        goodness = false;
        return;
    }

    if (firstLine[0] != std::string("GET") && firstLine[0] != std::string("POST") && firstLine[0] != std::string("DELETE"))
    {
        goodness = false;
        return;
    }

    method = firstLine[0];
    originalPath = firstLine[1];

    // make the path realtive to working directory (server root)
    std::string relativePath = "." + originalPath;

    // transforming the realtive path to canonical absolute path
    char *realPath = realpath(relativePath.c_str(), NULL);

    if (!realPath)
    {
        resourceExistence = false;
    }
    else
    {
        resourceExistence = true;
        resourcePath = realPath;
        free(realPath);
    }

    // Parse the rest of the HTTP headers...
    std::size_t lastHeader = parseHeaders(requestLines);

    if ((goodness && lastHeader != -1) && (lastHeader == requestLines.size() - 2))
    {
        // TODO: Check Content-Length, set some flag if the full body has not been received (more data to recieve)
        body = requestLines[lastHeader + 1];
    }
    else if (lastHeader < requestLines.size() - 2)
    {
        goodness = false;
        return;
    }
}

std::size_t HttpRequest::parseHeaders(std::vector<std::string> &requestLines)
{
    for (std::size_t i = 1; i < requestLines.size(); i++)
    {
        std::string requestLine = requestLines[i];
        if (requestLine.length() == 0)
            return i;

        std::vector<std::string> pair = split(requestLine, ": ");
        if (pair.size() >= 2)
        {
            headers[pair[0]] = pair[1];
        }
        else
        {
            goodness = false;
            return -1;
        }
    }

    goodness = false;
    return -1;
}

HttpRequest::HttpRequest()
{
    goodness = false;
}

std::string HttpRequest::getMethod()
{
    return method;
}

std::string HttpRequest::getOriginalPath()
{
    return originalPath;
}

std::string HttpRequest::getResourcePath()
{
    return resourcePath;
}

bool HttpRequest::isGood()
{
    return goodness;
}

bool HttpRequest::resourceExists()
{
    return resourceExistence;
}
