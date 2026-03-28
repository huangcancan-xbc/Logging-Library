// 测试日志格式化功能

#include <iostream>
#include "../../source/formatter.hpp"

// 标准格式测试
void test1()
{
    mylog::LogMsg msg(mylog::LogLevel::value::INFO, "test.cc", 888, "这是测试消息", "mylogger");
    mylog::Formatter fmt("[%d{%H:%M:%S}][%t][%c][%f:%l][%p]%T%m%n");
    std::cout << fmt.format(msg) << std::endl;
}

// 测试：只有时间和消息
void test2()
{
    mylog::LogMsg msg(mylog::LogLevel::value::DEBUG, "main.cpp", 100, "调试信息", "app");
    mylog::Formatter fmt("[%d{%H:%M:%S}] %m%n");
    std::cout << fmt.format(msg) << std::endl;
}

// 完整格式测试
void test3()
{
    mylog::LogMsg msg(mylog::LogLevel::value::ERROR, "utils.cpp", 256, "错误发生了！", "server");
    mylog::Formatter fmt("[%d{%Y-%m-%d %H:%M:%S}][%t][%c][%f:%l][%p] %m%n");
    std::cout << fmt.format(msg) << std::endl;
}

// 自定义分隔符测试
void test4()
{
    mylog::LogMsg msg(mylog::LogLevel::value::WARN, "config.json", 50, "配置警告", "config");
    mylog::Formatter fmt("%d{%H:%M:%S} | %p | %c | %m%n");
    std::cout << fmt.format(msg) << std::endl;
}

// 转义字符 %% 和非法占位符测试
void test5()
{
    mylog::LogMsg msg(mylog::LogLevel::value::INFO, "test.cc", 888, "测试消息", "root");
    mylog::Formatter fmt("%%测试%%: [%d{%H:%M:%S}] [%p] %m%n");
    std::cout << fmt.format(msg) << std::endl;
}

int main()
{
    test1();
    test2();
    test3();
    test4();
    test5();
    return 0;
}