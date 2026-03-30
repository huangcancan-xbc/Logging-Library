// 日志器模块
//
// 这个文件是整个日志库的调度中心，用户需要调用 logger->info(...) 家族方法，功能实现：
//   1. 把用户传进来的参数（文件名、行号、内容）打包成 LogMsg 对象
//   2. 用 Formatter 把 LogMsg 格式化成一串字符串（比如 "[14:30:25][INFO] 用户登录"）
//   3. 调用 log() 函数把字符串写出去（写到屏幕、文件、或者按时间/大小切割的文件）



#include "util.hpp"
#include "level.hpp"
#include "formatter.hpp"
#include "sink.hpp"
#include <atomic>
#include <mutex>


namespace mylog
{
    // 日志器基类：把用户调用 debug/info/warn/error/fatal 的请求，组装成格式化字符串，然后调 log() 写出去
    class Logger
    {
    public:
        using ptr = std::shared_ptr<Logger>;

        // 创建 LogMsg -> 用 Formatter 格式化 -> 调 log() 写出去
        // 参数：文件名、行号、日志内容
        void debug(const std::string &file, size_t line, const std::string &fmt, ...);
        void info(const std::string &file, size_t line, const std::string &fmt, ...);
        void warn(const std::string &file, size_t line, const std::string &fmt, ...);
        void error(const std::string &file, size_t line, const std::string &fmt, ...);
        void fatal(const std::string &file, size_t line, const std::string &fmt, ...);
    
    protected:
        // 控制日志怎么写出去
        virtual void log(const char *data, size_t len) = 0;

    protected:
        std::mutex _mutex;
        std::string _logger_name;                   // 日志器名字
        std::atomic<LogLevel::value> _limit_level;  // 日志级别限制，低于这个级别的日志不输出
        Formatter::ptr formatter;                   // 格式化器，负责把 LogMsg 拼成字符串
        std::vector<LogSink::ptr> _sinks;           // 输出目标列表
    };


    // 同步日志器
    class SyncLogger : public Logger
    {
    protected:
        void log(const char *data, size_t len);
    };
}