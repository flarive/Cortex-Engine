#include "../../include/misc/log_manager.h"



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
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::info);
    console_sink->set_pattern("[cortex] [%^%l%$] %v");

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/engine.log", true);
    file_sink->set_level(spdlog::level::info);

    logger = new spdlog::logger(LOGER_NAME, { console_sink, file_sink });
}

void engine::LogManager::info(const std::string& msg)
{
    logger->info(msg);
}

void engine::LogManager::warn(const std::string& msg)
{
    logger->warn(msg);
}

void engine::LogManager::error(const std::string& msg)
{
    logger->error(msg);
}