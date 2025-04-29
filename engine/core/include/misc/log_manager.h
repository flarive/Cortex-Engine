#pragma once

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

        const std::string LOGER_NAME = "multi_sink";
    };
}