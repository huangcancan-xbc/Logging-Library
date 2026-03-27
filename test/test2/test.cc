// 测试日志等级类

#include <iostream>
#include "../../source/level.hpp"


int main()
{
    std::cout << mylog::LogLevel::toString(mylog::LogLevel::value::DEBUG) << std::endl;
    std::cout << mylog::LogLevel::toString(mylog::LogLevel::value::INFO) << std::endl;
    std::cout << mylog::LogLevel::toString(mylog::LogLevel::value::WARN) << std::endl;
    std::cout << mylog::LogLevel::toString(mylog::LogLevel::value::ERROR) << std::endl;
    std::cout << mylog::LogLevel::toString(mylog::LogLevel::value::FATAL) << std::endl;
    std::cout << mylog::LogLevel::toString(mylog::LogLevel::value::OFF) << std::endl;
    std::cout << mylog::LogLevel::toString(mylog::LogLevel::value::UNKNOW) << std::endl;


    return 0;
}