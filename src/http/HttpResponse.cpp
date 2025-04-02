#include "http/HttpResponse.hpp"
#include "http/split.hpp"

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