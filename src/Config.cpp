#include "Config.hpp"
#include "toml.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>

#include <filesystem>
#include <fstream>
#include <cstdlib>

Config &Config::getConfigs()
{
    static Config instance;
    return instance;
}

int Config::getPort()
{
    return port;
}

std::size_t Config::getMaxRequestSize()
{
    return maxRequestSize;
}

std::size_t Config::getMaxThreadPoolSize()
{
    return maxThreadPoolSize;
}

std::string Config::getLoggingFormat()
{
    return loggingFormat;
}

Config::Config() : logger(spdlog::stdout_color_st("Configuration"))
{
    std::string userHomeDirectory = getenv("HOME");
    configDirectory = userHomeDirectory + "/.config/WebServer";
    configFileFullPath = configDirectory + "/config.toml";

    if (!std::filesystem::exists(configFileFullPath))
    {
        setDefaults();
        logger->info("Config file not found, Reverting to default settings");

        if (createConfigFile())
            logger->info("Created default config file at: " + configFileFullPath);
        else
            logger->error("Failed to create default config file at " + configFileFullPath);
        return;
    }

    toml::table table;

    try
    {
        table = toml::parse_file(configFileFullPath);
    }
    catch (const toml::parse_error &err)
    {
        logger->error("Failed to parse config file");
        setDefaults();
        logger->info("Reverting to default settings");
    }

    loggingFormat = table["loggingFormat"].value<std::string>().value_or("[%Y-%m-%d %H:%M:%S.%e] [%-16n] [%^%-5l%$] %v");
    maxRequestSize = std::stoul(table["maxRequestSize"].value<std::string>().value_or("51200"));
    maxThreadPoolSize = std::stoul(table["maxThreadPoolSize"].value<std::string>().value_or("10"));
    port = std::stoi(table["port"].value<std::string>().value_or("8080"));

    logger->info("Config at " + configFileFullPath + " successfully loaded");
}

void Config::setDefaults()
{
    port = 8080;
    maxRequestSize = 51200; // 50 KiB
    maxThreadPoolSize = 10;
    loggingFormat = "[%Y-%m-%d %H:%M:%S.%e] [%-16n] [%^%-5l%$] %v";
}

bool Config::createConfigFile()
{
    toml::table table{
        {"port", std::to_string(port)},
        {"maxRequestSize", std::to_string(maxRequestSize)},
        {"maxThreadPoolSize", std::to_string(maxThreadPoolSize)},
        {"loggingFormat", loggingFormat}};

    // Create config directory by running "mkdir path"
    std::string makeDirectoryCommand("mkdir " + configDirectory);
    system(makeDirectoryCommand.c_str());

    // open config file
    std::ofstream file(configFileFullPath);

    // write current settings
    if (file.is_open())
    {
        file << table;
        file.flush();
        bool success = file.good();
        file.close();
        return success;
    }
    return false;
}