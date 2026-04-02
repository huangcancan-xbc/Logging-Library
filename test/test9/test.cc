// 测试日志器模块 —— 建造者模式创建异步日志器


#include <iostream>
#include "../../source/logger.hpp"



// 只输出到屏幕，日志级别限制为 WARN（只有 warn/error/fatal 会输出）
void test1()
{
    std::unique_ptr<mylog::LoggerBuilder> builder(new mylog::LocalLoggerBuilder());
    builder->buildLoggerName("sync_stdout_logger");
    builder->buildLoggerType(mylog::LoggerType::LOGGER_SYNC);
    builder->buildLoggerLevel(mylog::LogLevel::value::WARN);
    builder->buildSink<mylog::StdoutSink>();
    mylog::Logger::ptr logger = builder->build();

    logger->debug(__FILE__, __LINE__, "%s", "这条不会显示");
    logger->info(__FILE__, __LINE__, "%s", "这条也不会显示");
    logger->warn(__FILE__, __LINE__, "%s", "warn test");
    logger->error(__FILE__, __LINE__, "%s", "error test");
    logger->fatal(__FILE__, __LINE__, "%s", "fatal test");
}


// 同时输出到屏幕和文件，用自定义格式（不显示线程ID和文件名）
void test2()
{
    std::unique_ptr<mylog::LoggerBuilder> builder(new mylog::LocalLoggerBuilder());
    builder->buildLoggerName("sync_file_logger");
    builder->buildLoggerType(mylog::LoggerType::LOGGER_SYNC);
    builder->buildLoggerLevel(mylog::LogLevel::value::DEBUG);
    builder->buildFormatter("[%d{%Y-%m-%d %H:%M:%S}][%t][%c][%f:%l][%p]%T%m%n");
    builder->buildSink<mylog::StdoutSink>();
    builder->buildSink<mylog::FileSink>("./sync/test.log");
    mylog::Logger::ptr logger = builder->build();

    logger->debug(__FILE__, __LINE__, "%s", "debug test");
    logger->info(__FILE__, __LINE__, "%s", "info test");
    logger->warn(__FILE__, __LINE__, "%s", "warn test");
}


// 不调用 buildFormatter，自动使用默认格式；输出到按大小滚动的文件
void test3()
{
    std::unique_ptr<mylog::LoggerBuilder> builder(new mylog::LocalLoggerBuilder());
    builder->buildLoggerName("sync_roll_logger");
    builder->buildLoggerType(mylog::LoggerType::LOGGER_SYNC);
    builder->buildLoggerLevel(mylog::LogLevel::value::ERROR);
    // 不调用 buildFormatter()，build() 会自动用默认格式
    builder->buildSink<mylog::RollBySizeSink>("./sync/roll.log", 1024);
    mylog::Logger::ptr logger = builder->build();

    size_t cursize = 0, count = 0;
    while (cursize < 1024 * 3)
    {
        logger->error(__FILE__, __LINE__, "roll test - %d", count++);
        cursize += 100;
    }
}


// 完全异步日志器（安全模式 ASYNC_SAFE）：只输出到屏幕，日志级别为 INFO
// 调用 log() 后数据写入缓冲区立刻返回，后台线程负责实际输出
void test4()
{
    std::unique_ptr<mylog::LoggerBuilder> builder(new mylog::LocalLoggerBuilder());
    builder->buildLoggerName("async_stdout_logger");
    builder->buildLoggerType(mylog::LoggerType::LOGGER_ASYNC);  // 设置为异步日志器
    builder->buildLoggerLevel(mylog::LogLevel::value::INFO);    // INFO 及以上级别才输出
    builder->buildSink<mylog::StdoutSink>();
    mylog::Logger::ptr logger = builder->build();

    logger->debug(__FILE__, __LINE__, "%s", "这条不会显示（低于INFO）");
    logger->info(__FILE__, __LINE__, "%s", "async info test");
    logger->warn(__FILE__, __LINE__, "%s", "async warn test");
    logger->error(__FILE__, __LINE__, "%s", "async error test");
    logger->fatal(__FILE__, __LINE__, "%s", "async fatal test");
}


// 完全异步日志器（非安全模式 ASYNC_UNSAFE）：输出到文件
// 特点：ASYNC_UNSAFE 模式下不加锁，性能更高但
void test5()
{
    std::unique_ptr<mylog::LoggerBuilder> builder(new mylog::LocalLoggerBuilder());
    builder->buildLoggerName("async_file_logger");
    builder->buildLoggerType(mylog::LoggerType::LOGGER_ASYNC);  // 设置为异步日志器
    builder->buildEnableUnSafeAsync();                          // 启用非安全异步模式（无锁，更快）
    builder->buildLoggerLevel(mylog::LogLevel::value::DEBUG);
    builder->buildFormatter("[%d{%Y-%m-%d %H:%M:%S}][%p]%T%m%n");
    builder->buildSink<mylog::FileSink>("./async/async_test.log");
    mylog::Logger::ptr logger = builder->build();

    logger->debug(__FILE__, __LINE__, "%s", "async debug test");
    logger->info(__FILE__, __LINE__, "%s", "async info test");
    logger->warn(__FILE__, __LINE__, "%s", "async warn test");
}


// 完全异步日志器（安全模式）：同时输出到屏幕和滚动文件
// 数据写入缓冲区后，后台线程会依次调用每个 sink 的 log() 方法
void test6()
{
    std::unique_ptr<mylog::LoggerBuilder> builder(new mylog::LocalLoggerBuilder());
    builder->buildLoggerName("async_multi_sink_logger");
    builder->buildLoggerType(mylog::LoggerType::LOGGER_ASYNC);  // 设置为异步日志器
    builder->buildLoggerLevel(mylog::LogLevel::value::WARN);    // 只输出 WARN 及以上
    builder->buildSink<mylog::StdoutSink>();
    builder->buildSink<mylog::RollBySizeSink>("./async/async_roll.log", 2048);
    mylog::Logger::ptr logger = builder->build();

    logger->debug(__FILE__, __LINE__, "%s", "不会显示");
    logger->info(__FILE__, __LINE__, "%s", "不会显示");
    logger->warn(__FILE__, __LINE__, "%s", "async warn");
    logger->error(__FILE__, __LINE__, "%s", "async error");

    size_t count = 0;
    while(count < 5000)
    {
        logger->error(__FILE__, __LINE__, "roll test - %d", count++);
    }
}



int main()
{
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    
    return 0;
}
