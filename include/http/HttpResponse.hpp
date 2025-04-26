#pragma once

#include "ResponseBodyChunker.hpp"

#include <string>
#include <map>

struct ChunkedTransfer
{
};

class HttpResponse
{
    int responseCode;
    std::string responseMessage;
    std::string body;
    std::map<std::string, std::string> headers;

    ResponseBodyChunker chunker;
    bool chunked;

public:
    // Uses content-length based on body
    HttpResponse(int responseCode, std::string responseMessage, std::string body);

    // Uses transfer-encoding: chunked
    HttpResponse(int responseCode, std::string responseMessage, ChunkedTransfer, std::filesystem::path path);

    std::string getData();

    int getResponseCode();

    void addResponseHeader(std::string key, std::string value);

    bool isChunked();

    ResponseBodyChunker getChunker();
};