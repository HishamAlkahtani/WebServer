#include "http/HttpResponse.hpp"
#include "http/split.hpp"

HttpResponse::HttpResponse(int responseCode, std::string responseMessage, std::string body)
    : responseCode(responseCode), responseMessage(responseMessage), body(body)
{
    chunked = false;
}

HttpResponse::HttpResponse(int responseCode, std::string responseMessage, ChunkedTransfer, std::filesystem::path path)
    : responseCode(responseCode), responseMessage(responseMessage), chunker(path)
{
    chunked = true;
}

std::string HttpResponse::getData()
{
    std::string header;
    header += std::string("HTTP/1.1 ") + std::to_string(responseCode) + " " + responseMessage + CRLF;

    if (chunked)
        header += "Transfer-Encoding: chunked" + CRLF;
    else
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

bool HttpResponse::isChunked()
{
    return chunked;
}

ResponseBodyChunker HttpResponse::getChunker()
{
    if (chunked)
    {
        return chunker;
    }
    else
    {
        throw std::runtime_error("Cannot get chunker: Response is not chunked!");
    }
}