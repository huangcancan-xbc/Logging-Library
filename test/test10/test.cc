// 测试日志器管理器LoggerManager和全局日志器建造者GlobalLoggerBuilder
// 测试：单例模式、自动注册、按名称查找、同名处理、手动注册

#include <iostream>
#include <cassert>
#include "../../source/logger.hpp"


// LoggerManager 单例模式 & rootLogger
void test1()
{
    // 多次调用 getInstance 应返回同一对象
    mylog::LoggerManager &mgr1 = mylog::LoggerManager::getInstance();
    mylog::LoggerManager &mgr2 = mylog::LoggerManager::getInstance();
    assert(&mgr1 == &mgr2);
    std::cout << "getInstance() 返回的是同一实例！" << std::endl;

    // rootLogger应返回名为 "root" 的日志器
    mylog::Logger::ptr root = mylog::LoggerManager::getInstance().rootLogger();
    assert(root != nullptr);
    assert(root->getName() == "root");
    std::cout << "rootLogger()返回的是root，即有效的日志器！" << std::endl;

    // 用 rootLogger 写一条日志
    root->info(__FILE__, __LINE__, "%s", "默认的root日志器");
    std::cout << std::endl;
}


// GlobalLoggerBuilder自动注册到LoggerManager
void test2()
{
    // 构建前应该不存在
    assert(!mylog::LoggerManager::getInstance().hasLogger("global_sync_logger"));

    // 用 GlobalLoggerBuilder 构建一个同步日志器
    std::unique_ptr<mylog::LoggerBuilder> builder(new mylog::GlobalLoggerBuilder());
    builder->buildLoggerName("global_sync_logger");
    builder->buildLoggerType(mylog::LoggerType::LOGGER_SYNC);
    builder->buildLoggerLevel(mylog::LogLevel::value::DEBUG);
    builder->buildSink<mylog::StdoutSink>();
    mylog::Logger::ptr logger = builder->build();

    // 构建后会自动注册到 LoggerManager
    assert(mylog::LoggerManager::getInstance().hasLogger("global_sync_logger"));
    std::cout << "GlobalLoggerBuilder已被自动注册到LoggerManager" << std::endl;

    // 通过 getLogger 按名称取回，应是同一个对象
    mylog::Logger::ptr fetched = mylog::LoggerManager::getInstance().getLogger("global_sync_logger");
    assert(fetched != nullptr);
    assert(fetched->getName() == "global_sync_logger");
    assert(fetched.get() == logger.get());
    std::cout << "getLogger() 按名称返回的是同一个日志器" << std::endl;

    // 写日志
    logger->debug(__FILE__, __LINE__, "%s", "[debug] 全局日志器");
    logger->info(__FILE__, __LINE__, "%s", "[info] 全局日志器");
    std::cout << std::endl;
}


// GlobalLoggerBuilder异步模式 + 带文件输出
void test3()
{
    std::unique_ptr<mylog::LoggerBuilder> builder(new mylog::GlobalLoggerBuilder());
    builder->buildLoggerName("global_async_logger");
    builder->buildLoggerType(mylog::LoggerType::LOGGER_ASYNC);
    builder->buildLoggerLevel(mylog::LogLevel::value::INFO);
    builder->buildFormatter("[%d{%H:%M:%S}][%p]%T%m%n");
    builder->buildSink<mylog::StdoutSink>();
    mylog::Logger::ptr logger = builder->build();

    assert(mylog::LoggerManager::getInstance().hasLogger("global_async_logger"));
    std::cout << "异步GlobalLoggerBuilder自动注册成功!" << std::endl;

    logger->debug(__FILE__, __LINE__, "%s", "这条不会显示");
    logger->info(__FILE__, __LINE__, "%s", "async info");
    logger->warn(__FILE__, __LINE__, "%s", "async warn");
    logger->error(__FILE__, __LINE__, "%s", "async error");
    std::cout << std::endl;
}


// 同名日志器处理 —— addLogger对已存在的日志器直接返回
void test4()
{
    // 先创建一个名叫test_logger的日志器
    std::unique_ptr<mylog::LoggerBuilder> builder1(new mylog::GlobalLoggerBuilder());
    builder1->buildLoggerName("test_logger");
    builder1->buildLoggerType(mylog::LoggerType::LOGGER_SYNC);
    builder1->buildLoggerLevel(mylog::LogLevel::value::DEBUG);
    builder1->buildSink<mylog::StdoutSink>();
    mylog::Logger::ptr logger1 = builder1->build();

    // 再尝试创建同名的第二个日志器
    std::unique_ptr<mylog::LoggerBuilder> builder2(new mylog::GlobalLoggerBuilder());
    builder2->buildLoggerName("test_logger");
    builder2->buildLoggerType(mylog::LoggerType::LOGGER_SYNC);
    builder2->buildLoggerLevel(mylog::LogLevel::value::ERROR);
    builder2->buildSink<mylog::StdoutSink>();
    mylog::Logger::ptr logger2 = builder2->build();

    // addLogger会对同名直接返回，manager中存的应该还是第一个
    mylog::Logger::ptr fetched = mylog::LoggerManager::getInstance().getLogger("test_logger");
    assert(fetched.get() == logger1.get());
    std::cout << "同名字日志器保留旧版成功！" << std::endl;

    // logger2本身也能工作，只是没被注册到manager
    logger2->error(__FILE__, __LINE__, "%s", "这条日志来自logger2（虽然不在管理器中）");
    std::cout << std::endl;
}


// 手动注册 —— 用LocalLoggerBuilder创建后手动addLogger
void test5()
{
    // LocalLoggerBuilder不会自动注册
    std::unique_ptr<mylog::LoggerBuilder> builder(new mylog::LocalLoggerBuilder());
    builder->buildLoggerName("local_logger");
    builder->buildLoggerType(mylog::LoggerType::LOGGER_SYNC);
    builder->buildLoggerLevel(mylog::LogLevel::value::WARN);
    builder->buildSink<mylog::StdoutSink>();
    mylog::Logger::ptr logger = builder->build();

    // 构建后不应在manager中
    assert(!mylog::LoggerManager::getInstance().hasLogger("local_logger"));
    std::cout << "LocalLoggerBuilder不会自动注册正常！" << std::endl;

    // 手动添加到 manager
    mylog::LoggerManager::getInstance().addLogger(logger);
    assert(mylog::LoggerManager::getInstance().hasLogger("local_logger"));
    std::cout << "手动 addLogger() 成功！" << std::endl;

    // 通过manager取回并使用
    mylog::Logger::ptr fetched = mylog::LoggerManager::getInstance().getLogger("local_logger");
    assert(fetched.get() == logger.get());
    fetched->warn(__FILE__, __LINE__, "%s", "这是手动注册日志器的警告！");
    fetched->info(__FILE__, __LINE__, "%s", "这条不会显示（低于WARN）");
    std::cout << std::endl;
}


// getLogger查询不存在的名称会返回nullptr
void test6()
{
    mylog::Logger::ptr none = mylog::LoggerManager::getInstance().getLogger("none_test");
    assert(none == nullptr);
    std::cout << "getLogger() 对不存在的日志器名称返回了 nullptr" << std::endl;

    assert(!mylog::LoggerManager::getInstance().hasLogger("none_test"));
    std::cout << "hasLogger() 对不存在的日志器名称返回了 false" << std::endl;
    std::cout << std::endl;
}





// 模块A：创建并注册全局日志器
void test7()
{
    // 使用全局日志器建造者创建同步日志器，自动注册到 LoggerManager
    std::unique_ptr<mylog::LoggerBuilder> builder(new mylog::GlobalLoggerBuilder());
    builder->buildLoggerName("global_logger");
    builder->buildLoggerType(mylog::LoggerType::LOGGER_ASYNC);
    builder->buildLoggerLevel(mylog::LogLevel::value::DEBUG);
    builder->buildSink<mylog::StdoutSink>();
    builder->buildSink<mylog::FileSink>("./global_test.log");
    builder->build();

    std::cout << "A：全局日志器已创建并注册到LoggerManager" << std::endl;
    std::cout << std::endl;
}


// 模块B：从LoggerManager获取并使用全局日志器
void test8()
{
    // 从 LoggerManager 获取之前注册的全局日志器
    mylog::Logger::ptr logger = mylog::LoggerManager::getInstance().getLogger("global_logger");
    logger->debug(__FILE__, __LINE__, "%s", "debug test");
    logger->info(__FILE__, __LINE__, "%s", "info test");
    logger->warn(__FILE__, __LINE__, "%s", "warn test");
    logger->error(__FILE__, __LINE__, "%s", "error test");
    logger->fatal(__FILE__, __LINE__, "%s", "fatal test");

    int count = 0;
    while(count < 5000)
    {
        logger->fatal(__FILE__, __LINE__, "test: %d", count++);
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

    test7();
    test8();

    return 0;
}