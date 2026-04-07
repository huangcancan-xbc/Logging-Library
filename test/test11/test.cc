// 测试（宏）全局接口测试
// 1.全局接口函数：mylog::getLogger() / mylog::rootLogger()
// 2.宏函数：DEBUG() / INFO() / WARN() / ERROR() / FATAL()


#include "../../source/mylog.h"


// 用 GlobalLoggerBuilder创建全局日志器并自动注册
void test1()
{
    // 使用全局日志器建造者创建异步日志器，build() 后自动注册到 LoggerManager
    std::unique_ptr<mylog::LoggerBuilder> builder(new mylog::GlobalLoggerBuilder());
    builder->buildLoggerName("global_async_logger");                  // 设置日志器名称
    builder->buildLoggerType(mylog::LoggerType::LOGGER_ASYNC);        // 异步日志器
    builder->buildLoggerLevel(mylog::LogLevel::value::DEBUG);         // 最低输出级别为 DEBUG
    builder->buildSink<mylog::StdoutSink>();                          // 屏幕输出
    builder->buildSink<mylog::FileSink>("./global_test.log");         // 指定文件输出
    builder->build();                                                 // 构建并注册到 LoggerManager

    std::cout << "A：全局日志器已创建并注册到LoggerManager" << std::endl;
    std::cout << std::endl;
}


// 通过 LoggerManager 按名称获取全局日志器并调用日志接口
void test2()
{
    // 从 LoggerManager获取test1中注册的global_async_logger
    mylog::Logger::ptr logger = mylog::LoggerManager::getInstance().getLogger("global_async_logger");

    logger->debug("%s", "debug test2");
    logger->info("%s", "info test2");
    logger->warn("%s", "warn test2");
    logger->error("%s", "error test2");
    logger->fatal("%s", "fatal test2");

    int count = 0;
    while(count < 5000)
    {
        logger->fatal("test: %d", count++);
    }
}



// 通过宏函数 DEBUG/INFO/WARN/ERROR/FATAL 直接调用 rootLogger()
void test3()
{
    DEBUG("%s", "debug test3");
    INFO("%s", "info test3");
    WARN("%s", "warn test3");
    ERROR("%s", "error test3");
    FATAL("%s", "fatal test3");
}


int main()
{
    test1();
    test2();
    test3();

    return 0;
}