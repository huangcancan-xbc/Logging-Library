// 测试日志器模块 —— 同步日志器功能
// 核心目标：验证 SyncLogger 能否把同一条日志同时写到 控制台、普通文件、滚动文件 三个地方


#include <iostream>
#include "../../source/formatter.hpp"
#include "../../source/logger.hpp"
#include <unistd.h>


void test1()
{
    std::string logger_name = "sync_logger";            // 日志器名字

    // 日志级别阈值：设为DEBUG，即DEBUG 及以上级别的日志全部都会输出
    // 级别从低到高：DEBUG < INFO < WARN < ERROR < FATAL
    mylog::LogLevel::value limit = mylog::LogLevel::value::DEBUG;

    // [2026-03-30 17:09:58][140234567][sync_logger][test.cc:25][DEBUG] debug test
    mylog::Formatter::ptr fmt(new mylog::Formatter("[%d{%Y-%m-%d %H:%M:%S}][%t][%c][%f:%l][%p]%T%m%n"));


    // 输出目标1：终端
    mylog::LogSink::ptr stdout = mylog::SinkFactory::create<mylog::StdoutSink>();
    // 输出目标2：指定文件
    mylog::LogSink::ptr file = mylog::SinkFactory::create<mylog::FileSink>("./filelog/file.log");
    // 输出目标3：按大小滚动的文件。参数：文件路径、单文件最大大小（字节）
    mylog::LogSink::ptr roll = mylog::SinkFactory::create<mylog::RollBySizeSink>("./rolllog/roll.log", 1024);


    // 把三个输出目标打包成一个列表，传给日志器。即每打一条日志，三个地方会同时收到
    std::vector<mylog::LogSink::ptr> sinks = {stdout, file, roll};

    
    mylog::Logger::ptr logger(new mylog::SyncLogger(logger_name, limit, fmt, sinks));   // 创建同步日志器


    // 输出5条不同级别的日志：因为阈值是最低的DEBUG，所以会全部输出
    logger->debug(__FILE__, __LINE__, "%s", "debug test");
    logger->info(__FILE__, __LINE__, "%s", "info test");
    logger->warn(__FILE__, __LINE__, "%s", "warn test");
    logger->error(__FILE__, __LINE__, "%s", "error test");
    logger->fatal(__FILE__, __LINE__, "%s", "fatal test");


    // 滚动文件测试
    size_t cursize = 0, count = 0;
    while (cursize < 1024 * 5)      // 总共写入5个文件大小
    {
        logger->error(__FILE__, __LINE__, "【error : rollfile】- %d", count++);
        cursize += 100;         // 粗略估计
    }
}

int main()
{
    test1();
    return 0;
}