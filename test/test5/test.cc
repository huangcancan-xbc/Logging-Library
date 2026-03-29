// 测试日志落地功能扩展——以时间为滚动文件，即每隔指定时间就切换到新的文件


#include <iostream>
#include "../../source/formatter.hpp"
#include "RollingBranch.hpp"
#include <unistd.h>


void test1()
{
    mylog::LogMsg msg(mylog::LogLevel::value::DEBUG, "test.cc", 99, "测试日志正常输出到时间滚动文件", "Rolling Time");
    mylog::Formatter fmt;
    std::string str = fmt.format(msg);

    // 创建时间滚动文件
    mylog::RollBySizeSink::ptr RollingTime_log = mylog::SinkFactory::create<RollByTimeSink>("./RollingTime/log", TimeGap::GAP_SECOUND);
    time_t old = mylog::util::Date::now();
    while(mylog::util::Date::now() < old + 5)
    {
        RollingTime_log->log(str.c_str(), str.size());
        usleep(100000);
    }
}

int main()
{
    test1();
    return 0;
}