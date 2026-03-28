#ifndef __M_FMT_H__
#define __M_FMT_H__

#include "level.hpp"
#include "message.hpp"
#include <memory>
#include <ctime>

namespace mylog
{
    // 抽象格式化子项父类
    class FormatItem
    {
    public:
        using ptr = std::shared_ptr<FormatItem>;
        virtual void format(std::ostream &out, LogMsg &msg) = 0;
    };



    // 格式化子项的子类：

    // 消息主体
    class MsgFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, LogMsg &msg) override
        {
            out << msg._payload;
        }
    };

    // 日志等级
    class LevelFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, LogMsg &msg) override
        {
            out << LogLevel::toString(msg._level);
        }
    };

    // 日志输出时间
    class TimeFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, LogMsg &msg) override
        {
            // localtime_r能将时间戳分解成年、月、日、时、分、秒等（线程安全）
            // 参数：指向time_t时间戳的指针、struct tm 结构体，成功返回struct tm 结构体指针，失败为nullptr
            struct tm t;
            localtime_r(&msg._ctime, &t);

            char tmp[32] = {0};
            // strftime指定格式把时间转成字符串。成功返回写入字节数，失败为0
            // 参数：输出缓冲区指针、缓冲区大小、格式化方式（比如：%Y-%m-%d %H:%M:%S）、struct tm 时间结构体指针
            strftime(tmp, 31, _time_fmt.c_str(), &t);

            out << tmp;
        }

        TimeFormatItem(const std::string &fmt = "%H:%M:%S")
            : _time_fmt(fmt)
        {
            
        }

    private:
        std::string _time_fmt;  // %H:%M:%S
    };

    // 日志相关文件
    class FileFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, LogMsg &msg) override
        {
            out << msg._file;
        }
    };

    // 日志相关文件里具体某一行
    class LineFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, LogMsg &msg) override
        {
            out << msg._line;
        }
    };

    // 线程ID
    class ThreadFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, LogMsg &msg) override
        {
            out << msg._tid;
        }
    };

    // 日志器名称
    class LoggerFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, LogMsg &msg) override
        {
            out << msg._logger;
        }
    };

    // 制表符
    class TabFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, LogMsg &msg) override
        {
            out << "\t";
        }
    };

    // 换行符
    class NewLineFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, LogMsg &msg) override
        {
            out << "\n";
        }
    };

    // 其他内容
    class OtherFormatItem : public FormatItem
    {
    public:
        OtherFormatItem(const std::string& str)
            : _str(str)
        {

        }

        void format(std::ostream &out, LogMsg &msg) override
        {
            out << _str;
        }
    private:
        std::string _str;
    };
}

#endif