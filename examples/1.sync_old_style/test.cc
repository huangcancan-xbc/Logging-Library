// 同步输出 + 旧方式调用（不使用宏）

#include "../../source/logger.hpp"


// 1. 标准输出
void test_stdout_sync()
{
    mylog::LocalLoggerBuilder builder;
    builder.buildLoggerName("sync_stdout_logger");              // 日志器名字
    builder.buildLoggerType(mylog::LoggerType::LOGGER_SYNC);    // 同步
    builder.buildLoggerLevel(mylog::LogLevel::value::DEBUG);    // 等级限制为debug
    builder.buildFormatter("[%d{%H:%M:%S}][%c][%p]%T%m%n");     // 输出格式
    builder.buildSink<mylog::StdoutSink>();                     // 屏幕输出
    mylog::Logger::ptr logger = builder.build();

    logger->debug(__FILE__, __LINE__, "%s", "sync stdout debug message");
    logger->info(__FILE__, __LINE__, "%s", "sync stdout info message");
}


// 2. 普通文件输出
void test_file_sync()
{
    mylog::LocalLoggerBuilder builder;
    builder.buildLoggerName("sync_file_logger");                // 日志器名字
    builder.buildLoggerType(mylog::LoggerType::LOGGER_SYNC);    // 同步
    builder.buildLoggerLevel(mylog::LogLevel::value::DEBUG);    // 等级限制为debug
    builder.buildFormatter("[%d{%Y-%m-%d %H:%M:%S}][%c][%p]%T%m%n");    // 输出格式
    builder.buildSink<mylog::FileSink>("./logs/sync_file.log"); // 指定文件
    mylog::Logger::ptr logger = builder.build();

    logger->info(__FILE__, __LINE__, "user %s login success", "alice");
    logger->warn(__FILE__, __LINE__, "%s", "sync file warning message");
}


// 3. 按大小滚动文件输出
void test_roll_by_size_sync()
{
    mylog::LocalLoggerBuilder builder;
    builder.buildLoggerName("sync_roll_logger");
    builder.buildLoggerType(mylog::LoggerType::LOGGER_SYNC);
    builder.buildLoggerLevel(mylog::LogLevel::value::DEBUG);
    builder.buildFormatter("[%d{%H:%M:%S}][%c][%p]%T%m%n");
    builder.buildSink<mylog::RollBySizeSink>("./logs/roll_size.log", 1024); // 滚动文件限制大小1024字节即1kb
    mylog::Logger::ptr logger = builder.build();

    for(int i = 0; i < 50; ++i)
    {
        logger->info(__FILE__, __LINE__, "sync rolling file message index = %d", i);
    }
}


int main()
{
    test_stdout_sync();
    test_file_sync();
    test_roll_by_size_sync();

    return 0;
}