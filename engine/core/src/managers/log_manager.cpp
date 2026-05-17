#include "../../include/managers/log_manager.h"



#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/basic_file_sink.h"


engine::LogManager::LogManager()
{
    init_sinks();
}

engine::LogManager& engine::LogManager::getInstance()
{
    static LogManager instance;  // Guaranteed to be thread-safe in C++11 and later
    return instance;
}

void engine::LogManager::init_sinks()
{
#ifndef DISABLE_LOGGING_FILE
    // console + file sink
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::info);
    console_sink->set_pattern("[cortex] [%^%l%$] %v");

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/engine.log", true);
    file_sink->set_level(spdlog::level::trace);

    logger = new spdlog::logger(LOGER_NAME, { console_sink, file_sink });
    logger->set_level(spdlog::level::trace);
#else
    // console sink
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::info);
    console_sink->set_pattern("[cortex] [%^%l%$] %v");

    logger = new spdlog::logger(LOGER_NAME, { console_sink });
    logger->set_level(spdlog::level::info);
#endif
}

//void engine::LogManager::info(const std::string& msg)
//{
//    logger->info(msg);
//}

//void engine::LogManager::warn(const std::string& msg)
//{
//    logger->warn(msg);
//}

//void engine::LogManager::error(const std::string& msg)
//{
//    logger->error(msg);
//}

engine::LogManager::~LogManager()
{

}