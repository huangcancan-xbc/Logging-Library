// 日志落地模块：负责将格式化好的日志输出到不同的方向
// 
// 抽象基类：LogSink
// 派生子类：
//   - StdoutSink      : 输出到屏幕
//   - FileSink        : 输出到单个文件
//   - RollBySizeSink  : 滚动文件（按大小切割）



#include "util.hpp"
#include <memory>
#include <fstream>


namespace mylog
{
    class LogSink
    {
    public:
        using ptr = std::shared_ptr<LogSink>;
        LogSink() {};
        virtual ~LogSink() {};

        // 参数：（格式化好的）日志信心、日志的长度
        virtual void log(const char *data, size_t len) = 0;
    };



    // 落地方向一：标准输出到屏幕
    class StdoutSink : public LogSink
    {
    public:
        void log(const char *data, size_t len);
    };

    // 落地方向二：指定文件
    class FileSink : public LogSink
    {
    public:
        // 构造时传入文件名并打开文件，将操作句柄管理起来
        FileSink(const std::string &pathname)
            : _pathname(pathname)
        {

        }

        void log(const char *data, size_t len);
    private:
        std::string _pathname;          // 文件路径
        std::ofstream _ofs;
    };

    // 落地方向三：滚动文件（以大小进行滚动）
    // 当当前文件大小超过指定大小则创建并写入新的文件
    class RollBySizeSink : public LogSink
    {
    public:
        RollBySizeSink(const std::string &basename, size_t max_size)
        {

        }

        void log(const char *data, size_t len);

    private:
        void createNewFile();       // 进行大小判断，超过最大大小则创建新文件

    private:
        std::string _basename;      // 输出当前文件名，格式：基础文件名+扩展文件名（用时间生成）的组合
        std::ofstream _ofs;         // 输出文件流
        size_t _max_fsize;          // 记录当前文件的最大大小，当文件大小超过限度就要切换文件，写入另一个文件
        size_t _cur_fsize;          // 记录当前文件已经写入的大小
    };
}