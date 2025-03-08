#pragma once

#include <netinet/in.h>
#include <vector>
#include <map>
#include <string>
#include <memory>

#define CRLF std::string("\r\n")

/**
 * @brief Splits a string by a specified delimiter
 *
 * @param str String to split
 * @param delimiter String that marks where to split (can be multi-character)
 * @return std::vector<std::string> Vector containing the split substrings
 *
 * Splits the input string at each occurrence of the delimiter. The delimiter is not
 * included in the results. Empty strings are preserved when delimiters appear
 * consecutively or at the string boundaries.
 */
inline std::vector<std::string> split(std::string &str, std::string delimiter);

class HttpRequest
{
    std::string method;
    std::string path;
    std::string rawRequest;
    std::string body;
    std::map<std::string, std::string> headers;
    bool goodness;

    /**
     * @brief Parses HTTP headers from request lines
     *
     * @param requestLines Vector of strings containing request content split by CRLF
     * @return Index of the empty line separating headers from body (-1 if malformed)
     *
     * Processes each line after the request line until an empty line is found,
     * storing valid headers in the headers map. The body (if present) starts at
     * headerLines[return_value + 1]. Sets goodness to false if headers are malformed.
     */
    std::size_t parseHeaders(std::vector<std::string> &requestLines);

public:
    HttpRequest(char *request);

    HttpRequest();

    std::string getMethod();

    std::string getRequestedPath();

    bool isGood();
};

class HttpResponse
{
    int responseCode;
    std::string responseMessage;
    std::string body;
    // TODO: Change implementation to map to allow better customization
    std::vector<std::string> headers;

public:
    HttpResponse(int responseCode, std::string responseMessage, std::string body);

    std::string getData();

    int getResponseCode();

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

    size_t snd(HttpResponse *response);
};

class ServerSocket
{
    int socket_fd;

public:
    ServerSocket(int port);

    ~ServerSocket();

    InternetHttpSocket getConnection();
};
