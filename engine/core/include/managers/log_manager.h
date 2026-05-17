#pragma once

#include "spdlog/spdlog.h"

#include <fmt/core.h>
#include <fmt/format.h>

#include <string>
#include <memory>
#include <source_location>

//#define DISABLE_LOGGING_FILE

namespace engine
{
    class LogManager final
    {
    public:
    
        static LogManager& getInstance();

        template<typename... Args>
        void info(fmt::format_string<Args...> fmt, Args&&... args)
        {
            /*std::string msg = fmt::format(fmt, std::forward<Args>(args)...);
            logger->info(msg);*/
            logger->info(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        void trace(fmt::format_string<Args...> fmt, Args&&... args)
        {
            /*std::string msg = fmt::format(fmt, std::forward<Args>(args)...);
            logger->trace(msg);*/
            logger->trace(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        void warn(fmt::format_string<Args...> fmt, Args&&... args)
        {
            /*std::string msg = fmt::format(fmt, std::forward<Args>(args)...);
            logger->warn(msg);*/
            logger->warn(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        void warn(fmt::format_string<Args...> fmt, std::source_location loc, Args&&... args)
        {
            std::string msg = fmt::format(fmt, std::forward<Args>(args)...);
            logger->warn(msg, loc);
        }

        template<typename... Args>
        void error(fmt::format_string<Args...> fmt, Args&&... args)
        {
            /*std::string msg = fmt::format(fmt, std::forward<Args>(args)...);
            logger->error(msg);*/
            logger->error(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        void error(fmt::format_string<Args...> fmt, std::source_location loc, Args&&... args)
        {
            std::string msg = fmt::format(fmt, std::forward<Args>(args)...);
            logger->error(msg, loc);
        }
        

    private:
        LogManager();  // Constructor is private
        LogManager(const LogManager&) = delete;
        LogManager& operator=(const LogManager&) = delete;
        ~LogManager();

        void init_sinks();

        spdlog::logger* logger = nullptr;

        const std::string LOGER_NAME = "cortex";
    };

    inline auto& logger = LogManager::getInstance();
}