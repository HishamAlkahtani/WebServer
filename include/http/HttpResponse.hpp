#pragma once

#include <string>
#include <vector>

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
