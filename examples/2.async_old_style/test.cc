// 异步输出 + 旧方式调用（不使用宏）

#include "../../source/logger.hpp"

// 1.标准输出
void test_stdout_async()
{
    mylog::LocalLoggerBuilder builder;
    builder.buildLoggerName("async_stdout_logger");             // 日志器名称
    builder.buildLoggerType(mylog::LoggerType::LOGGER_ASYNC);   // 异步
    builder.buildLoggerLevel(mylog::LogLevel::value::DEBUG);    // 等级限制为debug
    builder.buildFormatter("[%d{%H:%M:%S}][%c][%p]%T%m%n");     // 输出格式
    builder.buildSink<mylog::StdoutSink>();                     // 屏幕
    mylog::Logger::ptr logger = builder.build();

    logger->debug(__FILE__, __LINE__, "%s", "async stdout debug message");
    logger->info(__FILE__, __LINE__, "%s", "async stdout info message");
}

// 2.普通文件输出
void test_file_async()
{
    mylog::LocalLoggerBuilder builder;
    builder.buildLoggerName("async_file_logger");
    builder.buildLoggerType(mylog::LoggerType::LOGGER_ASYNC);
    builder.buildLoggerLevel(mylog::LogLevel::value::DEBUG);
    builder.buildFormatter("[%d{%Y-%m-%d %H:%M:%S}][%c][%p]%T%m%n");
    builder.buildSink<mylog::FileSink>("./logs/async_file.log");
    mylog::Logger::ptr logger = builder.build();

    logger->info(__FILE__, __LINE__, "job id = %d start", 1001);
    logger->error(__FILE__, __LINE__, "%s", "async file error message");
}

// 3.按大小滚动文件输出
void test_roll_by_size_async()
{
    mylog::LocalLoggerBuilder builder;
    builder.buildLoggerName("async_roll_logger");
    builder.buildLoggerType(mylog::LoggerType::LOGGER_ASYNC);
    builder.buildLoggerLevel(mylog::LogLevel::value::DEBUG);
    builder.buildFormatter("[%d{%H:%M:%S}][%c][%p]%T%m%n");
    builder.buildSink<mylog::RollBySizeSink>("./logs/async_roll.log", 1024);

    mylog::Logger::ptr logger = builder.build();
    for(int i = 0; i < 50; ++i)
    {
        logger->warn(__FILE__, __LINE__, "async rolling file message index = %d", i);
    }
}


int main()
{
    test_stdout_async();
    test_file_async();
    test_roll_by_size_async();

    return 0;
}