#include "../../include/misc/log_manager.h"


#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"

engine::LogManager::LogManager()
{
    stdout_example();
    rotating_example();
}

void engine::LogManager::stdout_example()
{
    // create a color multi-threaded logger
    auto console = spdlog::stdout_color_mt("console");
    auto err_logger = spdlog::stderr_color_mt("stderr");
    spdlog::get("console")->info("loggers can be retrieved from a global registry using the spdlog::get(logger_name)");
}

void engine::LogManager::rotating_example()
{
    // Create a file rotating logger with 5 MB size max and 3 rotated files
    auto max_size = 1048576 * 5;
    auto max_files = 3;
    auto logger = spdlog::rotating_logger_mt("some_logger_name", "logs/rotating.txt", max_size, max_files);
}

void engine::LogManager::info(const std::string& msg)
{
    spdlog::info(msg);
}

//spdlog::info("Welcome to spdlog!");
//spdlog::error("Some error message with arg: {}", 1);
//
//spdlog::warn("Easy padding in numbers like {:08d}", 12);
//spdlog::critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
//spdlog::info("Support for floats {:03.2f}", 1.23456);
//spdlog::info("Positional args are {1} {0}..", "too", "supported");
//spdlog::info("{:<30}", "left aligned");
//
//spdlog::set_level(spdlog::level::debug); // Set global log level to debug
//spdlog::debug("This message should be displayed..");
//
//// change log pattern
//spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
//
//// Compile time log levels
//// Note that this does not change the current log level, it will only
//// remove (depending on SPDLOG_ACTIVE_LEVEL) the call on the release code.
//SPDLOG_TRACE("Some trace message with param {}", 42);
//SPDLOG_DEBUG("Some debug message");
//
//
//stdout_example();
