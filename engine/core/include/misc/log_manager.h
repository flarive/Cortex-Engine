#pragma once

#include "spdlog/spdlog.h"

#include <string>

namespace engine
{
    class LogManager
    {
    public:
        static LogManager& getInstance();

        void info(const std::string& msg);
        void warn(const std::string& msg);
        void error(const std::string& msg);

        

    private:
        LogManager();  // Constructor is private
        LogManager(const LogManager&) = delete;
        LogManager& operator=(const LogManager&) = delete;

        void init_sinks();

        spdlog::logger* logger = nullptr;

        const std::string LOGER_NAME = "cortex";
    };
}