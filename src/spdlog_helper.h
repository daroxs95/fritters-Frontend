#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
//#include "spdlog/sinks/basic_file_sink.h"
#include <spdlog/sinks/rotating_file_sink.h>

#ifndef APP_NAME
#define APP_NAME "no_name_app"
#endif


//this need to be done better, to improve this attempt of singleton 
/**
 * @brief Get the Multi Sink Logger object reference. This logs to file and to console.
 * 
 * @return spdlog::logger& reference to singleton Multi Sink Logger
 */
spdlog::logger& getMultiSinkLogger()
{
    static auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    //console_sink->set_level(spdlog::level::warn);
    console_sink->set_level(spdlog::level::trace);
    //console_sink->set_pattern("[multi_sink_example] [%^%l%$] %v");

    //static auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/log.txt", true);
    static auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/log.txt", 1024*1024*10, 3);
    rotating_sink->set_level(spdlog::level::trace);

    static spdlog::logger logger(APP_NAME, {console_sink, rotating_sink});

    //async but not compiling, need to look into it later, just playing right now
    // spdlog::init_thread_pool(8192, 1);
    // auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt >();
    // auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logPath, 1024*1024*10, 3);
    // std::vector<spdlog::sink_ptr> sinks {stdout_sink, rotating_sink};
    // auto logger = std::make_shared<spdlog::async_logger>("loggername", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
    // spdlog::register_logger(logger);
    // return logger;

    //usage
    // logger.set_level(spdlog::level::debug);
    // logger.warn("this should appear in both console and file");
    // logger.info("this message should not appear in the console, only in the file");

    return logger;
}