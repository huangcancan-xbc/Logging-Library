// 使用宏接口 + 时间滚动文件（扩展）
// 把自定义的 RollByTimeSink 接入日志系统


#include "../../source/mylog.h"
#include "RollingBranch.hpp"
#include "../../source/util.hpp"
#include <unistd.h>


int main()
{
    mylog::GlobalLoggerBuilder builder;
    builder.buildLoggerName("time_macro_logger");
    builder.buildLoggerType(mylog::LoggerType::LOGGER_SYNC);
    builder.buildLoggerLevel(mylog::LogLevel::value::DEBUG);
    builder.buildFormatter("[%d{%H:%M:%S}][%c][%p]%T%m%n");
    builder.buildSink<RollByTimeSink>("./logs/time_roll", TimeGap::GAP_SECOND); // 以时间（秒）进行滚动文件
    builder.build();

    mylog::Logger::ptr logger = mylog::getLogger("time_macro_logger");
    if(logger == nullptr)
    {
        return 1;
    }

    time_t old = mylog::util::Date::now();
    while(mylog::util::Date::now() < old + 5)   // 5秒钟对应5个文件
    {
        logger->info("time -> rolling message");
        usleep(50000);  // 睡50毫秒，避免日志写太大
    }

    DEBUG("time() rolling message 1");
    INFO("time() rolling message 2");
    WARN("time() rolling message 3");

    return 0;
}