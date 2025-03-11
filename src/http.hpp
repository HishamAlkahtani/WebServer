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
    std::string originalPath;
    std::string resourcePath;
    std::string rawRequest;
    std::string body;
    std::map<std::string, std::string> headers;
    bool goodness;
    bool resourceExistence;

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

    /*
     * @brief Returns original path (like it was recieved in the request)
     *
     * @return Path as it was received in the HTTP request
     *
     * Only use this if you need to do some processing based on the original path or for logging.
     * Otherwise use getResourcePath, as this method's returned path will be an absolute
     * path that is likely outside server root.
     */
    std::string getOriginalPath();

    /*
     * @brief Returns request path relative to server root. Always use this method!
     *
     * @return absolute canonicalized path to resource under server root directory
     *
     * The returned path is the path of the resource on this machine
     * (e.g. If server root is ~/Server/ and requested path is /static/file.html
     * this function should return ~/Server/static/file.html).
     *
     * The result could be an empty string if the resouce doesn't exit!
     * (check resourceExists() if needed)
     */
    std::string getResourcePath();

    /*
     * @return validity of the HTTP request (regardless the existence of the resource)
     */
    bool isGood();

    bool resourceExists();
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
