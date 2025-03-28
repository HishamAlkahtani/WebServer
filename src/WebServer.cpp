#include "http.hpp"
#include "LoadBalancer.cpp"
#include "Config.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <iostream>
#include <string>

#include <string.h>
#include <unistd.h>

class WebServer
{
    Config config;
    ServerSocket serverSocket;
    LoadBalancer loadBalancer;
    std::shared_ptr<spdlog::logger> logger;

public:
    WebServer() : config(Config::getConfigs()), serverSocket(config.getPort()),
                  loadBalancer(config.getMaxThreadPoolSize()),
                  logger(spdlog::stdout_color_st("Server Socket"))
    {
        spdlog::set_pattern(config.getLoggingFormat());
        logger->info(std::string("Server initiated on port ") + std::to_string(config.getPort()));
    }

    // blocking!
    void start()
    {
        logger->info("Server started accepting for requests");
        while (true)
        {
            InternetHttpSocket clientSocket = serverSocket.getConnection();
            logger->debug("New connection request from " + clientSocket.getIp());
            loadBalancer.enqueu_socket(clientSocket);
        }
    }
};

// Gets files for user requests
// 1- Checks if the path of the file is within the allowed directory
// 2- Handles reading and caching  files.
class FileManager
{
};