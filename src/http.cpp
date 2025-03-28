#include "http.hpp"
#include "Config.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#include <cstdlib>
#include <cstring>
#include <stdio.h>

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

inline std::vector<std::string> split(std::string &str, std::string delimiter)
{

    std::vector<std::string> result;
    std::size_t startOfLine = 0;
    while (startOfLine < str.size())
    {
        std::size_t nextOccurrence = str.find(delimiter, startOfLine);

        std::size_t endOfLine;
        if (nextOccurrence != std::string::npos)
            endOfLine = nextOccurrence;
        else
            endOfLine = str.size();

        std::size_t length = endOfLine - startOfLine;

        result.push_back(str.substr(startOfLine, length));
        startOfLine = endOfLine + delimiter.length();
    }
    return result;
}

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

HttpResponse::HttpResponse(int responseCode, std::string responseMessage, std::string body)
    : responseCode(responseCode), responseMessage(responseMessage), body(body)
{
}

std::string HttpResponse::getData()
{
    std::string header;
    header += std::string("HTTP/1.1 ") + std::to_string(responseCode) + " " + responseMessage + CRLF;
    header += "Content-Length: " + std::to_string(body.length()) + CRLF;
    // TODO: add remaining headers from vector
    header += CRLF;
    std::string message = header + body;
    return message;
}

int HttpResponse::getResponseCode()
{
    return responseCode;
}

void HttpResponse::addResponseHeader(std::string key, std::string value)
{
    // TODO: add to vector
}

InternetHttpSocket::InternetHttpSocket(sockaddr_in &addr, int fd)
{
    sin_family = addr.sin_family;
    sin_port = addr.sin_port;
    ip = inet_ntoa(addr.sin_addr);
    connection_fd = fd;
    active = true;
    maxRequestSize = Config::getConfigs().getMaxRequestSize();
}

InternetHttpSocket::InternetHttpSocket()
{
    active = false;
    connection_fd = -1;
}

void InternetHttpSocket::closeSocket()
{
    active = false;
    close(connection_fd);
}

std::string InternetHttpSocket::getIp()
{
    return ip;
}

HttpRequest InternetHttpSocket::receive()
{
    if (!active)
    {
        return HttpRequest();
    }

    std::vector<char> receiveBuffer(2048);
    std::size_t offset = 0;

    /* Keep trying to receive until we have a valid request. If the request is not fully
       received, increase buffer size and receive the remaining part of the request. */
    while (receiveBuffer.size() < maxRequestSize)
    {
        // offset is how many bytes have been received already
        char *buffer = receiveBuffer.data() + offset;
        ssize_t length = recv(connection_fd, (void *)buffer, receiveBuffer.size() - offset, 0);

        // recv error
        if (length == -1)
        {
            std::cout << "Failed to recieve. errno: " << errno << std::endl;
            if (errno == 9)
            {
                std::cout << "File Descriptor Not Vaild! Closing socket with file descriptor: " << connection_fd << std::endl;
                closeSocket();
            }

            return HttpRequest();
        }
        // no messages available & peer has performed an orderly shutdown
        else if (length == 0)
        {
            closeSocket();
            return HttpRequest();
        }

        HttpRequest request(receiveBuffer.data());

        if (request.isGood())
            return request;

        offset += length;
        receiveBuffer.resize(receiveBuffer.size() * 2);
    }

    // If a valid HttpRequest can not be received, terminate connection.
    closeSocket();
    return HttpRequest();
}

size_t InternetHttpSocket::snd(HttpResponse *response)
{
    if (!active)
        return -1;

    std::string message = response->getData();
    return send(connection_fd, message.c_str(), message.length() * sizeof(char), 0); // is this causing the segfaults?
}

bool InternetHttpSocket::isActive()
{
    return active;
}

ServerSocket::ServerSocket(int port)
{
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
        throw std::runtime_error("Could not obtain socket file descriptor");

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(socket_fd, (sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1)
        throw std::runtime_error("Failed to bind socket to port!");

    if (listen(socket_fd, 1) == -1)
        throw std::runtime_error("call to listen failed");
}

ServerSocket::~ServerSocket()
{
    close(socket_fd);
}

// TODO: Handle accept returning -1 (don't throw an exception!)
InternetHttpSocket ServerSocket::getConnection()
{
    struct sockaddr_in peeraddr;
    socklen_t peerlen = sizeof(struct sockaddr_in);
    int connection_fd = accept(socket_fd, (sockaddr *)&peeraddr, &peerlen);
    InternetHttpSocket ret = InternetHttpSocket(peeraddr, connection_fd);
    return ret;
}
