// 同步和异步交叉使用，并统一采用宏接口


#include "../../source/mylog.h"


// 1. 同步 logger 使用对象宏接口
void test_sync_with_macro()
{
    mylog::LocalLoggerBuilder builder;
    builder.buildLoggerName("macro_sync_logger");
    builder.buildLoggerType(mylog::LoggerType::LOGGER_SYNC);        // 同步
    builder.buildLoggerLevel(mylog::LogLevel::value::DEBUG);
    builder.buildSink<mylog::StdoutSink>();
    builder.buildSink<mylog::FileSink>("./macro_sync.log");
    mylog::Logger::ptr logger = builder.build();

    logger->debug("macro -> sync debug message");
    logger->info("macro -> sync info message");
    logger->warn("macro -> sync warn message");

    DEBUG("macro() sync debug message");
    INFO("macro() sync info message");
    WARN("macro() sync warn message");
}


// 2. 异步 logger 使用对象宏接口
void test_async_with_macro()
{
    mylog::LocalLoggerBuilder builder;
    builder.buildLoggerName("macro_async_logger");
    builder.buildLoggerType(mylog::LoggerType::LOGGER_ASYNC);       // 异步
    builder.buildLoggerLevel(mylog::LogLevel::value::DEBUG);
    builder.buildSink<mylog::StdoutSink>();
    builder.buildSink<mylog::RollBySizeSink>("./macro_async_roll.log", 1024);
    mylog::Logger::ptr logger = builder.build();

    logger->info("macro -> async info message");
    logger->error("macro -> async error message");
    logger->fatal("macro -> async fatal message");

    INFO("macro() async info message");
    ERROR("macro() async error message");
    FATAL("macro() async fatal message");
}

int main()
{
    test_sync_with_macro();
    test_async_with_macro();

    return 0;
}