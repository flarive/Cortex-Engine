#pragma once

#include "spdlog/spdlog.h"

#include <fmt/core.h>
#include <fmt/format.h>

#include <string>
#include <memory>
#include <source_location>

namespace engine
{
    class LogManager
    {
    public:
        static LogManager& getInstance();

        //void info(const std::string& msg);

        template<typename... Args>
        void info(fmt::format_string<Args...> fmt, Args&&... args)
        {
            std::string msg = fmt::format(fmt, std::forward<Args>(args)...);
            logger->info(msg);
        }

        template<typename... Args>
        void warn(fmt::format_string<Args...> fmt,
            Args&&... args) {
            std::string msg = fmt::format(fmt, std::forward<Args>(args)...);
            logger->warn(msg);
        }

        template<typename... Args>
        void warn(fmt::format_string<Args...> fmt,
            std::source_location loc,
            Args&&... args) {
            std::string msg = fmt::format(fmt, std::forward<Args>(args)...);
            logger->warn(msg, loc);
        }

        //void warn(const std::string& msg);
        //void error(const std::string& msg);

        template<typename... Args>
        void error(fmt::format_string<Args...> fmt,
            Args&&... args) {
            std::string msg = fmt::format(fmt, std::forward<Args>(args)...);
            logger->error(msg);
        }

        template<typename... Args>
        void error(fmt::format_string<Args...> fmt,
            std::source_location loc,
            Args&&... args) {
            std::string msg = fmt::format(fmt, std::forward<Args>(args)...);
            logger->error(msg, loc);
        }
        

    private:
        LogManager();  // Constructor is private
        LogManager(const LogManager&) = delete;
        LogManager& operator=(const LogManager&) = delete;

        void init_sinks();

        spdlog::logger* logger = nullptr;

        const std::string LOGER_NAME = "cortex";
    };

    inline auto& logger = LogManager::getInstance();
}