#pragma once

#include <string>
#include <map>
#include <vector>

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