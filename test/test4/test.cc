// 测试日志落地功能

#include <iostream>
#include "../../source/formatter.hpp"
#include "../../source/sink.hpp"

// 日志到标准输出测试
void test1()
{
    mylog::LogMsg msg(mylog::LogLevel::value::INFO, "test.cc", 888, "测试日志正常输出到屏幕", "stdout");
    mylog::Formatter fmt;
    std::string str = fmt.format(msg);

    mylog::LogSink::ptr stdout_log = mylog::SinkFactory::create<mylog::StdoutSink>();
    stdout_log->log(str.c_str(), str.size());
}


// 日志到指定文件输出测试
void test2()
{
    mylog::LogMsg msg(mylog::LogLevel::value::INFO, "test.cc", 666, "测试日志正常输出到指定文件", "file");
    mylog::Formatter fmt;
    std::string str = fmt.format(msg);

    mylog::LogSink::ptr file_log = mylog::SinkFactory::create<mylog::FileSink>("./file/file_test.log");
    file_log->log(str.c_str(), str.size());
}


// 日志到滚动文件输出测试
// 预期效果：当单个文件写入超过1024*1024字节（1MB）时，自动切换到新文件
// 总共写入约10MB数据，应该产生多个滚动文件
void test3()
{
    mylog::LogMsg msg(mylog::LogLevel::value::DEBUG, "test.cc", 99, "测试日志正常输出到滚动文件", "Rolling File");
    mylog::Formatter fmt;
    std::string str = fmt.format(msg);

    // 创建滚动文件sink，单文件最大1MB
    mylog::RollBySizeSink::ptr RollingFile_log = mylog::SinkFactory::create<mylog::RollBySizeSink>("./RollingFile/roll_log", 1024 * 1024);
    size_t cursize = 0, count = 0;
    while (cursize < 1024 * 1024 * 10)  // 总共写入约10MB
    {
        std::string temp = std::to_string(count++) + str;
        RollingFile_log->log(temp.c_str(), temp.size());
        cursize += temp.size();  // 累加已写字节数
    }
}

int main()
{
    test1();
    test2();
    test3();
    return 0;
}