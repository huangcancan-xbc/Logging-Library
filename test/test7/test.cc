// 测试日志器模块 —— 建造者模式创建同步日志器


#include <iostream>
#include "../../source/logger.hpp"
#include <unistd.h>


// 只输出到屏幕，日志级别限制为 WARN（只有 warn/error/fatal 会输出）
void test1()
{
    std::unique_ptr<mylog::LoggerBuilder> builder(new mylog::LocalLoggerBuilder());
    builder->buildLoggerName("stdout_logger");
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
    builder->buildLoggerName("file_logger");
    builder->buildLoggerLevel(mylog::LogLevel::value::DEBUG);
    builder->buildFormatter("[%d{%Y-%m-%d %H:%M:%S}][%t][%c][%f:%l][%p]%T%m%n");
    builder->buildSink<mylog::StdoutSink>();
    builder->buildSink<mylog::FileSink>("./file/test.log");
    mylog::Logger::ptr logger = builder->build();

    logger->debug(__FILE__, __LINE__, "%s", "debug test");
    logger->info(__FILE__, __LINE__, "%s", "info test");
    logger->warn(__FILE__, __LINE__, "%s", "warn test");
}


// 不调用 buildFormatter，自动使用默认格式；输出到按大小滚动的文件
void test3()
{
    std::unique_ptr<mylog::LoggerBuilder> builder(new mylog::LocalLoggerBuilder());
    builder->buildLoggerName("roll_logger");
    builder->buildLoggerLevel(mylog::LogLevel::value::ERROR);
    // 不调用 buildFormatter()，build() 会自动用默认格式
    builder->buildSink<mylog::RollBySizeSink>("./rolllog/roll.log", 1024);
    mylog::Logger::ptr logger = builder->build();

    size_t cursize = 0, count = 0;
    while (cursize < 1024 * 3)
    {
        logger->error(__FILE__, __LINE__, "roll test - %d", count++);
        cursize += 100;
    }
}


int main()
{
    test1();
    test2();
    test3();
    return 0;
}
