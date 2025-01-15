#pragma once

#include <netinet/in.h>
#include <vector>
#include <string>
#include <memory>

#define CRLF std::string("\r\n")

// Splits string str by delimiter. Delimiter can have multiple characters
// and will be treated as one string that the function will look for.
inline std::vector<std::string> split(std::string str, std::string delimiter);

class HttpRequest
{
    std::string method;
    std::string path;
    std::string rawRequest;
    bool goodness;

public:
    HttpRequest(char *request);

    HttpRequest();

    std::string getRawRequest();

    std::string getRequestedPath();

    bool isGood();
};

class HttpResponse
{
    int responseCode;
    std::string responseMessage;
    std::string body;
    std::vector<std::string> headers;

public:
    HttpResponse(int responseCode, std::string responseMessage, std::string body);

    std::string getData();

    void addResponseHeader(std::string key, std::string value);
};

class InternetHttpSocket
{
    short sin_family;
    unsigned short sin_port;
    std::string ip;
    int connection_fd;

public:
    bool isActive;

    InternetHttpSocket(sockaddr_in &addr, int fd);

    // for convenience. doesn't create a valid socket.
    InternetHttpSocket();

    std::string getIp();

    HttpRequest recieve();

    size_t snd(std::unique_ptr<HttpResponse> response);
};

class ServerSocket
{
    int socket_fd;

public:
    ServerSocket(int port);

    ~ServerSocket();

    InternetHttpSocket getConnection();
};
