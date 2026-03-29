// 定义日志消息类，进行日志中间信息的存储
// 日志输出时间     日志等级    源文件名称      源代码行号      线程ID      日志正文消息    日志器名称


#pragma once
#include "level.hpp"
#include "util.hpp"
#include <iostream>
#include <string>
#include <thread>


namespace mylog
{
    struct LogMsg
    {
        time_t _ctime;              // 日志的时间戳
        LogLevel::value _level;     // 日志等级
        std::string _file;          // 源文件名称
        size_t _line;               // 代码行号
        std::thread::id _tid;       // 线程ID
        std::string _payload;       // 日志正文内容（有效载荷）
        std::string _logger;        // 日志器名称


        // 参数：日志等级、代码文件名、行号、日志正文、日志器名字
        LogMsg(LogLevel::value level, const std::string file, size_t line, const std::string msg, const std::string logger)
            : _ctime(util::Date::now()),
            _level(level),
            _file(file),
            _line(line),
            _tid(std::this_thread::get_id()),
            _payload(msg),
            _logger(logger)
        {
            
        }
    };
}
