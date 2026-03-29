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
#include <cassert>
#include <sstream>
#include <sys/time.h>   // gettimeofday
#include <iomanip>



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
        void log(const char *data, size_t len)
        {
            // 把内存里的数据原样吐出来，不格式化，不看\0 结束符
            // 参数：数据地址、数据长度
            std::cout.write(data, len);
        }
    };



    // 落地方向二：指定文件
    class FileSink : public LogSink
    {
    public:
        // 构造时传入文件名并打开文件，将操作句柄管理起来
        FileSink(const std::string &pathname)
            : _pathname(pathname)
        {
            // 1.创建日志文件所在路径
            util::File::createDirectory(util::File::getPath(pathname));

            // 2.创建并打开日志文件
            _ofs.open(_pathname, std::ios::binary | std::ios::app);
            assert(_ofs.is_open());
        }

        void log(const char *data, size_t len)
        {
            _ofs.write(data, len);
            assert(_ofs.good());    // 检查文件操作有没有出错
        }

    private:
        std::string _pathname;          // 文件路径
        std::ofstream _ofs;
    };



    // 落地方向三：滚动文件（以大小进行滚动）
    // 当当前文件大小超过指定大小则创建并写入新的文件
    class RollBySizeSink : public LogSink
    {
    public:
        // 参数：基础的文件名、单文件最大容量大小（单位字节）
        RollBySizeSink(const std::string &basename, size_t max_size)
            : _basename(basename),
            _max_fsize(max_size),
            _cur_fsize(0),
            _count(0)
        {
            std::string filename = createNewFile();
            // 1.创建日志文件所在路径
            util::File::createDirectory(util::File::getPath(filename));

            // 2.创建并打开日志文件
            _ofs.open(filename, std::ios::binary | std::ios::app);
            assert(_ofs.is_open());
        }

        void log(const char *data, size_t len)
        {
            // 先判断文件不能继续写入
            if(_cur_fsize >= _max_fsize)
            {
                _ofs.close();               // 先关闭当前文件！
                std::string pathname = createNewFile();
                _cur_fsize = 0;             // 将当前文件大小置为0
                _ofs.open(pathname, std::ios::binary | std::ios::app);
                assert(_ofs.is_open());
            }

            _ofs.write(data, len);
            _cur_fsize += len;              // 注意及时更新当前文件大小
            assert(_ofs.good());            // 检查文件操作有没有出错
        }

    private:
        std::string createNewFile()         // 进行大小判断，超过最大大小则创建新文件
        {
            time_t t = util::Date::now();   // 获取系统时间

            // localtime_r能将时间戳分解成年、月、日、时、分、秒等（线程安全）
            // 参数：指向time_t时间戳的指针、struct tm 结构体，成功返回struct tm 结构体指针，失败为nullptr
            struct tm lt;
            localtime_r(&t, &lt);

            std::stringstream filename;     // 创建一个流用于存放以时间格式的日志文件名
            // 这里在思考是用宏、私有变量还是就这样，先放着吧，感觉改起来虽然要改7处，但是自定义程度高
            filename << _basename << "_";
            filename << lt.tm_year + 1900 << "-";       // 年：tm_year 从 1900 开始计数
            filename << std::setfill('0') << std::setw(2) << (lt.tm_mon + 1) << "-";           // 月：tm_mon 从 0 开始，0 表示 1 月
            filename << lt.tm_mday << " ";
            filename << lt.tm_hour << ":";
            filename << lt.tm_min << ":";
            filename << lt.tm_sec << "-";
            filename << (_count++);
            filename << ".log";

            return filename.str();
        }

    private:
        std::string _basename;      // 输出当前文件名，格式：基础文件名+扩展文件名（用时间生成）的组合
        std::ofstream _ofs;         // 输出文件流
        size_t _max_fsize;          // 记录当前文件的最大大小，当文件大小超过限度就要切换文件，写入另一个文件
        size_t _cur_fsize;          // 记录当前文件已经写入的大小
        // cpu太快，可能一秒就能产生很多文件，这些文件会覆盖，又因为每次都是追加操作
        // 所以现象是滚动文件看似滚动，实则还是写到了一个文件
        size_t _count;              // 解决办法：来一个计数器将这些文件分开，避免一秒内的产生多个文件被写到一起
    };

    class SinkFactory
    {
    public:
        template<typename SinkType, typename ...Args>
        static LogSink::ptr create(Args && ...agrs)
        {
            return std::make_shared<SinkType>(std::forward<Args>(agrs)...);
        }
    };
}