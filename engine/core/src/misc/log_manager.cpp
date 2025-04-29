#include "../../include/misc/log_manager.h"


#include "spdlog/spdlog.h"
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
    console_sink->set_level(spdlog::level::warn);
    console_sink->set_pattern("[multi_sink_example] [%^%l%$] %v");

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/engine.log", true);
    file_sink->set_level(spdlog::level::trace);

    spdlog::logger logger("multi_sink", { console_sink, file_sink });
    logger.set_level(spdlog::level::debug);
    logger.warn("this should appear in both console and file");
    logger.info("this message should not appear in the console, only in the file");

}

void engine::LogManager::info(const std::string& msg)
{
    auto logger = spdlog::get(LOGER_NAME);
    if (logger)
    {
        logger->info(msg);
    }
}

void engine::LogManager::warn(const std::string& msg)
{
    auto logger = spdlog::get(LOGER_NAME);
    if (logger)
    {
        logger->warn(msg);
    }
}

void engine::LogManager::error(const std::string& msg)
{
    auto logger = spdlog::get(LOGER_NAME);
    if (logger)
    {
        logger->error(msg);
    }
}