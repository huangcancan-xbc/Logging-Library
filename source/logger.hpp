// 日志器模块
//
// 这个文件是整个日志库的调度中心，用户需要调用 logger->info(...) 家族方法，功能实现：
//   1. 把用户传进来的参数（文件名、行号、内容）打包成 LogMsg 对象
//   2. 用 Formatter 把 LogMsg 格式化成一串字符串（比如 "[14:30:25][INFO] 用户登录"）
//   3. 调用 log() 函数把字符串写出去（写到屏幕、文件、或者按时间/大小切割的文件）

// 普通日志器（SyncLogger）：收到日志→立刻写文件→写完才返回（要等）
// 异步日志器（AsyncLogger）：收到日志→扔进缓冲区就跑→后台线程慢慢写文件（不用等）


#include "util.hpp"
#include "level.hpp"
#include "formatter.hpp"
#include "sink.hpp"
#include "looper.hpp"
#include <atomic>
#include <mutex>
#include <cstdarg>
#include <unordered_map>



namespace mylog
{
    // 日志器基类：把用户调用 debug/info/warn/error/fatal 的请求，组装成格式化字符串，然后调 log() 写出去
    class Logger
    {
    public:
        using ptr = std::shared_ptr<Logger>;

        // 参数：日志器名字、日志等级限制、格式化器、管理日志落地的数组
        Logger(const std::string &logger_name, LogLevel::value level, Formatter::ptr &formatter, std::vector<LogSink::ptr> &sinks)
            : _logger_name(logger_name),
            _limit_level(level),
            _formatter(formatter),
            _sinks(sinks.begin(), sinks.end())
        {
            
        }

        // 获取日志器名称
        const std::string & getName()
        {
            return _logger_name;
        }

        // 创建 LogMsg -> 用 Formatter 格式化 -> 调 log() 写出去
        // 参数：文件名、行号、日志内容
        void debug(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            // 1.判断当前日志是否达到了输出等级
            if(LogLevel::value::DEBUG < _limit_level)
            {
                return;
            }


            // 2.对fmt格式化字符串和不定参进行字符串组织，得到日志消息的字符串
            va_list ap;             // 定义一个变量，用来访问可变参数（我的理解是指针/句柄，官方解释叫游标）
            va_start(ap, fmt);      // 可变参数前一个参数
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap); // Prerequisites/1.不定参宏函数的使用.cc ： 106-124
            if(ret == -1)
            {
                std::cout << "vasprintf failed!\n";
            }
            va_end(ap);


            // // 3.构造LogMsg对象
            // // 参数：日志等级、代码文件名、行号、日志正文、日志器名字
            // LogMsg msg(LogLevel::value::DEBUG, file, line, res, _logger_name);


            // // 4.通过格式化工具对LogMsg进行格式化，得到格式化后的字符串
            // std::stringstream ss;
            // _formatter->format(ss, msg);


            // // 5.进行日志落地
            // log(ss.str().c_str(), ss.str().size());
            // free(res);


            // 将上述3步进行封装，避免代码冗余
            serialize(LogLevel::value::DEBUG, file, line, res);
            free(res);
        }


        void info(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            if(LogLevel::value::INFO < _limit_level)
            {
                return;
            }


            va_list ap;             // 定义一个变量，用来访问可变参数（我的理解是指针/句柄，官方解释叫游标）
            va_start(ap, fmt);      // 可变参数前一个参数
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap); // Prerequisites/1.不定参宏函数的使用.cc ： 106-124
            if(ret == -1)
            {
                std::cout << "vasprintf failed!\n";
            }
            va_end(ap);


            serialize(LogLevel::value::INFO, file, line, res);

            free(res);
        }


        void warn(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            if(LogLevel::value::WARN < _limit_level)
            {
                return;
            }


            va_list ap;             // 定义一个变量，用来访问可变参数（我的理解是指针/句柄，官方解释叫游标）
            va_start(ap, fmt);      // 可变参数前一个参数
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap); // Prerequisites/1.不定参宏函数的使用.cc ： 106-124
            if(ret == -1)
            {
                std::cout << "vasprintf failed!\n";
            }
            va_end(ap);


            serialize(LogLevel::value::WARN, file, line, res);

            free(res);

        }


        void error(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            if(LogLevel::value::ERROR < _limit_level)
            {
                return;
            }


            va_list ap;             // 定义一个变量，用来访问可变参数（我的理解是指针/句柄，官方解释叫游标）
            va_start(ap, fmt);      // 可变参数前一个参数
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap); // Prerequisites/1.不定参宏函数的使用.cc ： 106-124
            if(ret == -1)
            {
                std::cout << "vasprintf failed!\n";
            }
            va_end(ap);


            serialize(LogLevel::value::ERROR, file, line, res);

            free(res);

        }


        void fatal(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            if(LogLevel::value::FATAL < _limit_level)
            {
                return;
            }


            va_list ap;             // 定义一个变量，用来访问可变参数（我的理解是指针/句柄，官方解释叫游标）
            va_start(ap, fmt);      // 可变参数前一个参数
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap); // Prerequisites/1.不定参宏函数的使用.cc ： 106-124
            if(ret == -1)
            {
                std::cout << "vasprintf failed!\n";
            }
            va_end(ap);


            serialize(LogLevel::value::FATAL, file, line, res);

            free(res);

        }
    
    protected:
        // 控制日志怎么写出去
        virtual void log(const char *data, size_t len) = 0;

        void serialize(LogLevel::value level, const std::string &file, size_t line, const char* str)
        {
            LogMsg msg(level, file, line, str, _logger_name);


            // 4.通过格式化工具对LogMsg进行格式化，得到格式化后的字符串
            std::stringstream ss;
            _formatter->format(ss, msg);


            // 5.进行日志落地
            log(ss.str().c_str(), ss.str().size());
        }

    protected:
        std::mutex _mutex;
        std::string _logger_name;                   // 日志器名字
        std::atomic<LogLevel::value> _limit_level;  // 日志级别限制，低于这个级别的日志不输出
        Formatter::ptr _formatter;                  // 格式化器，负责把 LogMsg 拼成字符串
        std::vector<LogSink::ptr> _sinks;           // 输出目标列表
    };


    // 同步日志器
    class SyncLogger : public Logger
    {
    public:
        SyncLogger(const std::string &logger_name, LogLevel::value level, Formatter::ptr &formatter, std::vector<LogSink::ptr> &sinks)
            : Logger(logger_name, level, formatter, sinks)
        {

        }

    protected:
        // 同步日志器，日志直接通过落地模块的句柄进行日志的落地
        void log(const char *data, size_t len) override
        {
            std::unique_lock<std::mutex> lock(_mutex);
            if(_sinks.empty())
            {
                return;
            }

            for(auto &sink : _sinks)
            {
                sink->log(data, len);
            }
        }
    };


    // 异步工作器
    class AsyncLogger : public Logger
    {
    public:
        AsyncLogger(const std::string &logger_name, LogLevel::value level, Formatter::ptr &formatter, 
            std::vector<LogSink::ptr> &sinks, AsyncType looper_type)
            : Logger(logger_name, level, formatter, sinks),
            _looper(std::make_shared<AsyncLooper>(std::bind(&AsyncLogger::realLog, this, std::placeholders::_1), looper_type))
        {
            
        }

        void log(const char *data, size_t len) override     // 把数据写入_looper的缓冲区,立刻返回，不写文件、不阻塞、不等待
        {
            _looper->push(data, len);
        }

    private:
        // 接收一个装满日志的缓冲区buf(由后台线程自动传过来)，把缓冲区里的数据真正写到文件/屏幕(调用_sinks)
        void realLog(Buffer &buf)                   // 设计实际落地函数，即把缓冲区的数据落地
        {
            if(_sinks.empty())
            {
                return;
            }

            for(auto &sink : _sinks)
            {
                sink->log(buf.begin(), buf.readAbleSize());
            }
        }

    private:
        AsyncLooper::ptr _looper;
    };





    // 日志器类型：同步还是异步
    enum class LoggerType
    {
        LOGGER_SYNC,    // 同步
        LOGGER_ASYNC    // 异步
    };


    // 建造者基类：用链式调用组装日志器，避免用户手动传一堆参数
    // 用法示例：
    //   LocalLoggerBuilder builder;
    //   builder.buildLoggerName("myapp");
    //   builder.buildLoggerLevel(LogLevel::value::DEBUG);
    //   builder.buildFormatter("[%d{%H:%M:%S}][%p]%m%n");
    //   builder.buildSink<StdoutSink>();
    //   builder.buildSink<FileSink>("./logs/app.log");
    //   builder.build();  // 返回日志器
    class LoggerBuilder
    {
    public:
        LoggerBuilder()
            : _logger_type(LoggerType::LOGGER_ASYNC),
            _limit_level(LogLevel::value::DEBUG),
            _looper_type(AsyncType::ASYNC_SAFE)
        {
            
        }

        void buildLoggerType(LoggerType type)           // 设置同步/异步
        {
            _logger_type = type;
        }

        void buildEnableUnSafeAsync()                   // 设置非安全状态
        {
            _looper_type = AsyncType::ASYNC_UNSAFE;
        }

        void buildLoggerName(const std::string &name)   // 设置日志器名字
        {
            _logger_name = name;
        }

        void buildLoggerLevel(LogLevel::value level)    // 设置最低输出级别
        {
            _limit_level = level;
        }

        void buildFormatter(const std::string &patten = "[%d{%H:%M:%S}][%t][%c][%f:%l][%p]%T%m%n")  // 设置日志格式模板，不传则用默认
        {
            _formatter = std::make_shared<Formatter>(patten);
        }

        // 添加输出目标（可多次调用，同时写多个地方）
        // 例如：buildSink<StdoutSink>() 或 buildSink<FileSink>("./app.log")
        template <typename SinkType, typename... Args>
        void buildSink(Args &&...args)
        {
            LogSink::ptr psink = SinkFactory::create<SinkType>(std::forward<Args>(args)...);
            _sinks.push_back(psink);
        }

        virtual Logger::ptr build() = 0;                // 最终构建出日志器对象

    protected:
        AsyncType _looper_type;                         // 安全状态 or 非安全
        LoggerType _logger_type;                        // 同步 or 异步
        std::string _logger_name;                       // 日志器名字（会出现在日志内容里）
        std::atomic<LogLevel::value> _limit_level;      // 低于这个级别的日志不输出
        Formatter::ptr _formatter;                      // 格式化器
        std::vector<LogSink::ptr> _sinks;               // 输出目标列表（屏幕、文件、滚动文件）
    };


    // 局部日志器建造者：返回日志器对象，调用者自己管理生命周期
    class LocalLoggerBuilder : public LoggerBuilder
    {
    public:
        Logger::ptr build() override
        {
            assert(_logger_name.empty() == false);               // 必须要有日志器名称
            if(_formatter.get() == nullptr)
            {
                _formatter = std::make_shared<Formatter>();
            }

            if(_sinks.empty())
            {
                buildSink<StdoutSink>();
            }

            if(_logger_type == LoggerType::LOGGER_ASYNC)
            {
                return std::make_shared<AsyncLogger>(_logger_name, _limit_level, _formatter, _sinks, _looper_type);
            }

            return std::make_shared<SyncLogger>(_logger_name, _limit_level, _formatter, _sinks);
        }
    };





    // 日志器管理器
    class LoggerManager
    {
    public:
        static LoggerManager &getInstance()                 // 单例
        {
            static LoggerManager _instance;
            return _instance;
        }

        void addLogger(Logger::ptr &logger)                 // 添加日志器进行管理
        {
            if(hasLogger(logger->getName()))
            {
                return;                                     // 如果日志器已经存在则直接返回
            }

            std::unique_lock<std::mutex> lock(_mutex);
            _loggers.insert(std::make_pair(logger->getName(), logger));
        }

        bool hasLogger(const std::string &name)             // 判断是否管理了指定名称的日志器
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto it = _loggers.find(name);
            if(it == _loggers.end())
            {
                return false;
            }

            return true;
        }

        Logger::ptr getLogger(const std::string &name)      // 获取指定名称的日志器
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto it = _loggers.find(name);
            if(it == _loggers.end())
            {
                return nullptr;
            }

            return it->second;
        }

        Logger::ptr rootLogger()                            // 获取默认日志器
        {
            return _root_logger;
        }

    private:
        LoggerManager()
        {
            std::unique_ptr<mylog::LoggerBuilder> builder(new mylog::LocalLoggerBuilder());
            builder->buildLoggerName("root");
            _root_logger = builder->build();
            _loggers.insert(std::make_pair("root", _root_logger));
        }

    private:
        std::mutex _mutex;
        Logger::ptr _root_logger;                               // 默认日志器
        std::unordered_map<std::string, Logger::ptr> _loggers;  // 所管理的日志器数组
    };
}