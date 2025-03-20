#pragma once

#include <iostream>
#include <spdlog/spdlog.h>

class Config
{
public:
    static Config &getConfigs();

    int getPort();

    std::size_t getMaxRequestSize();

    std::size_t getMaxThreadPoolSize();

    std::string getLoggingFormat();

    /**
     * @brief Loads config from default path or creates one if missing.
     *
     * Loads config file from (~/.config/WebServer/config.toml) or creates one with
     * defaults if not found. If file is not found or failed to parse, the instance
     * will revert to default settings.
     *
     * Take care to use Config::getConfigs() in initalizer list to avoid running this
     * constructor twice which can cause some problems.
     * */
    Config();

private:
    int port;

    std::size_t maxRequestSize;
    std::size_t maxThreadPoolSize;

    std::string loggingFormat;

    std::shared_ptr<spdlog::logger> logger;

    std::string configDirectory;
    std::string configFileFullPath;

    // TODO
    // int loggingLevel;

    /**
     * To be called when no config file is found, sets instance default config.
     */
    void setDefaults();

    /**
     * Creates config.toml at default path and stores the settings in the instance.
     */
    bool createConfigFile();
};