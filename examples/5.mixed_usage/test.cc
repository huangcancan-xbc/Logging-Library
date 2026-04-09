// 用户混合使用场景（宏的便利）


#include "../../source/mylog.h"

// 1. 全局同步 logger
void test_global_sync_old_style()
{
    mylog::GlobalLoggerBuilder builder;
    builder.buildLoggerName("mixed_global_sync");
    builder.buildLoggerType(mylog::LoggerType::LOGGER_SYNC);
    builder.buildLoggerLevel(mylog::LogLevel::value::DEBUG);
    builder.buildFormatter("[%d{%Y-%m-%d %H:%M:%S}][%c][%p]%T%m%n");
    builder.buildSink<mylog::StdoutSink>();
    builder.buildSink<mylog::FileSink>("./logs/mixed_global.log");
    mylog::Logger::ptr logger = builder.build();

    logger->debug("mixed example style debug");
    logger->info("mixed example style info");

    std::cout << std::endl << std::endl;
}

// 2.root logger + 宏调用
void test_root_macro()
{
    mylog::Logger::ptr logger = mylog::LoggerManager::getInstance().getLogger("mixed_global_sync");
    logger->debug("[global] mixed_global_sync debug message");
    logger->error("[global] mixed_global_sync error message");
    logger->warn("[global] mixed_global_sync warn message");

    DEBUG("mixed example root debug");
    INFO("mixed example root info");
    WARN("mixed example root warn");

    std::cout << std::endl << std::endl;
}


// 3. 局部异步 logger + 多 sink 输出
void test_local_async_multi_sink()
{
    mylog::LocalLoggerBuilder builder;
    builder.buildLoggerName("mixed_local_async");
    builder.buildLoggerType(mylog::LoggerType::LOGGER_ASYNC);
    builder.buildLoggerLevel(mylog::LogLevel::value::DEBUG);
    builder.buildSink<mylog::StdoutSink>();
    builder.buildSink<mylog::RollBySizeSink>("./logs/mixed_async_roll.log", 1024);

    mylog::Logger::ptr logger = builder.build();
    logger->info("%s", "mixed example async info");
    logger->error("%s", "mixed example async error");

    std::cout << std::endl << std::endl;
}


// ……


int main()
{
    test_global_sync_old_style();
    test_root_macro();
    test_local_async_multi_sink();

    return 0;
}