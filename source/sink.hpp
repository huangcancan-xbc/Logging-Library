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
#include <cstring>
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
            openFile(createNewFile());
        }


        // 下面有2个方案实现，根据自己所需使用，使用其中一个，请将另一个注释掉！！！

        // // 方案1：不允许日志大小超过指定大小用这个
        // void log(const char *data, size_t len)
        // {
        //     // 按行(以\n为分隔)遍历整个缓冲区,确保单条日志完整写入同一文件
        //     // 避免一条日志被截断分散到多个文件，方便了后续日志分析和问题排查
        //     const char *start = data;              // 当前处理位置
        //     const char *end = data + len;          // 缓冲区结束位置
            
        //     while (start < end)
        //     {
        //         // memchr: 在内存区域查找指定字符首次出现的位置
        //         // 参数：待搜索内存起始地址、要查找的字符是换行符、搜索字节数即从start开始向后搜索多少字节
        //         // 返回值: 找到返回指向该位置的指针,未找到返回nullptr
        //         const char *newline = static_cast<const char*>(memchr(start, '\n', end - start));
                
        //         if (newline == nullptr)
        //         {
        //             // 剩余数据如果不包含换行符，且会导致超限（前提是当前文件非空），则滚动
        //             if (_cur_fsize > 0 && _cur_fsize + (end - start) > _max_fsize)
        //             {
        //                 _ofs.close();
        //                 openFile(createNewFile());
        //             }

        //             _ofs.write(start, end - start);
        //             _cur_fsize += (end - start);
        //             break;
        //         }
                
        //         size_t line_len = newline - start + 1;  // 找到换行符，计算当前行长度(包含换行符)
                
        //         // 检查当前行写入后是否会超限
        //         if (_cur_fsize > 0 && _cur_fsize + line_len > _max_fsize)
        //         {
        //             _ofs.close();
        //             openFile(createNewFile());
        //         }
                
        //         _ofs.write(start, line_len);
        //         _cur_fsize += line_len;
                
        //         start = newline + 1;
        //     }

        //     assert(_ofs.good());
        // }


        // 方案2：允许日志大小超过指定大小的一点点
        void log(const char *data, size_t len)
        {
            const char *start = data;
            const char *end = data + len;
            
            while (start < end)
            {
                const char *newline = static_cast<const char*>(memchr(start, '\n', end - start));
                if (newline == nullptr)
                {
                    // 将剩余数据全部写入
                    size_t remaining_len = end - start;
                    _ofs.write(start, remaining_len);
                    _cur_fsize += remaining_len;
                    
                    // 即使是最后一段，写入后也需要判断是否超限并滚动
                    if (_cur_fsize >= _max_fsize)
                    {
                        _ofs.close();
                        openFile(createNewFile());
                    }
                    break;
                }
                
                size_t line_len = newline - start + 1;
                
                // 先写入再判断，允许单条日志超限，但避免在文件边界截断
                _ofs.write(start, line_len);
                _cur_fsize += line_len;
                
                // 写入后如果超限则滚动到新文件
                if (_cur_fsize >= _max_fsize)
                {
                    _ofs.close();
                    openFile(createNewFile());
                }
                
                start = newline + 1;
            }
            assert(_ofs.good());
        }

    private:
        // 打开一个日志文件，并把 RollBySizeSink 内部记录的当前文件大小 _cur_fsize 同步成这个文件的真实大小
        void openFile(const std::string &filename)
        {
            // 统一负责打开日志文件，并在打开后同步当前文件实际大小
            // 这样即使文件是以追加模式打开，_cur_fsize 也不会错误地从 0 开始计算
            util::File::createDirectory(util::File::getPath(filename));
            _ofs.open(filename, std::ios::binary | std::ios::app);
            assert(_ofs.is_open());

            // app 模式下写指针位于文件末尾，tellp() 可以拿到当前文件已有大小
            std::streampos pos = _ofs.tellp();
            _cur_fsize = (pos < 0) ? 0 : static_cast<size_t>(pos);
        }

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