#ifndef __M_FMT_H__
#define __M_FMT_H__

#include "level.hpp"
#include "message.hpp"
#include <memory>
#include <ctime>
#include <vector>
#include <cassert>
#include <sstream>



// 示例：[14:30:25][1234][myapp][main.cpp:100][INFO]  这是日志消息

namespace mylog
{
    // 抽象格式化子项父类
    class FormatItem
    {
    public:
        using ptr = std::shared_ptr<FormatItem>;
        virtual void format(std::ostream &out, const LogMsg &msg) = 0;
    };



    // 格式化子项的子类：

    // 消息主体
    class MsgFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMsg &msg) override
        {
            out << msg._payload;
        }
    };

    // 日志等级
    class LevelFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMsg &msg) override
        {
            out << LogLevel::toString(msg._level);
        }
    };

    // 日志输出时间
    class TimeFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMsg &msg) override
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
        void format(std::ostream &out, const LogMsg &msg) override
        {
            out << msg._file;
        }
    };

    // 日志相关文件里具体某一行
    class LineFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMsg &msg) override
        {
            out << msg._line;
        }
    };

    // 线程ID
    class ThreadFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMsg &msg) override
        {
            out << msg._tid;
        }
    };

    // 日志器名称
    class LoggerFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMsg &msg) override
        {
            out << msg._logger;
        }
    };

    // 制表符
    class TabFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMsg &msg) override
        {
            out << "\t";
        }
    };

    // 换行符
    class NewLineFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMsg &msg) override
        {
            out << "\n";
        }
    };

    // 其他内容：保存普通文本（比如括号、冒号、空格等），不是特殊占位符的字符都需要它
    class OtherFormatItem : public FormatItem
    {
    public:
        OtherFormatItem(const std::string& str)
            : _str(str)
        {

        }

        void format(std::ostream &out, const LogMsg &msg) override
        {
            out << _str;
        }
    private:
        std::string _str;
    };



    // %d：日期格式
    // %t: 线程ID
    // %c: 日志器名称
    // %f: 文件名
    // %l: 文件行号
    // %p: 日志级别
    // %T: 制表符缩进
    // %m: 主题消息
    // %n: 换行符
    class Formatter
    {
    public:
        using ptr = std::shared_ptr<Formatter>;
        
        // 组装说明书
        Formatter(const std::string&pattern = "[%d{%H:%M:%S}][%t][%c][%f:%l][%p]%T%m%n")
            : _pattern(pattern)
        {
            assert(parsePattern());
        }

        // 对msg进行格式化/组装输出
        void format(std::ostream &out, const LogMsg &msg)
        {
            for(auto& item : _items)
            {
                item->format(out, msg);
            }
        }
        std::string format(const LogMsg &msg)
        {
            std::stringstream ss;
            format(ss, msg);
            return ss.str();
        }

    private:
        // 解析格式模板字符串，把每个"零部件"拆出来
        bool parsePattern()
        {
            // 存放解析结果：key=占位符(如d/t/p), val=普通文本或子参数(如{H:M:S})
            std::vector<std::pair<std::string, std::string>> fmt_order;
            size_t pos = 0;
            std::string key, val;  // 临时存放当前正在处理的字符

            while (pos < _pattern.size())
            {
                char ch = _pattern[pos];

                // 情况1：普通字符（不是%）
                if (ch != '%')
                {
                    val.push_back(ch);
                    pos++;
                    continue;
                }

                // 情况2：%% 转义，代表输出一个真实的 %
                if (pos + 1 < _pattern.size() && _pattern[pos + 1] == '%')
                {
                    val.push_back('%');
                    pos += 2;
                    continue;
                }

                // 情况3：%后面是格式化占位符（如%d, %p, %m）
                // 先把之前积累的普通字符保存（如果有的话）
                if (!val.empty())
                {
                    fmt_order.push_back({"", val});  // key为空表示普通文本
                    val.clear();
                }

                pos++;  // 跳过 %，现在指向格式化字符

                // % 后面没有字符了，报错
                if (pos >= _pattern.size())
                {
                    std::cout << "% 后面没有格式化字符!\n";
                    return false;
                }

                // 读取格式化字符（如 d, t, p）
                key = _pattern[pos];
                pos++;

                // 读取可选的子参数，如 %d{H:M:S} 中的 H:M:S
                if (pos < _pattern.size() && _pattern[pos] == '{')
                {
                    pos++;  // 跳过 {
                    while (pos < _pattern.size() && _pattern[pos] != '}')
                    {
                        val.push_back(_pattern[pos++]);
                    }
                    if (pos >= _pattern.size())
                    {
                        std::cout << "子参数 {} 没有闭合!\n";
                        return false;
                    }
                    pos++;  // 跳过 }
                }

                // 保存这个占位符及其参数
                fmt_order.push_back({key, val});
                key.clear();
                val.clear();
            }

            // 循环结束后，可能还有剩余的普通字符（如 "ab%cde" 最后剩余的 "de"）
            if (!val.empty())
            {
                fmt_order.push_back({"", val});
            }

            // 根据解析结果，创建对应的 FormatItem 对象
            for (auto &it : fmt_order)
            {
                _items.push_back(createItem(it.first, it.second));
            }

            return true;
        }
        
        // 根据不同的格式化字符创建不同的格式化子项对象（根据代号创建对应零部件工厂）
        FormatItem::ptr createItem(const std::string &key, const std::string &val)
        {
            if(key == "d")
                return std::make_shared<TimeFormatItem>(val);
            if(key == "t")
                return std::make_shared<ThreadFormatItem>();
            if(key == "c")
                return std::make_shared<LoggerFormatItem>();
            if(key == "f")
                return std::make_shared<FileFormatItem>();
            if(key == "l")
                return std::make_shared<LineFormatItem>();
            if(key == "p")
                return std::make_shared<LevelFormatItem>();
            if(key == "T")
                return std::make_shared<TabFormatItem>();
            if(key == "m")
                return std::make_shared<MsgFormatItem>();
            if(key == "n")
                return std::make_shared<NewLineFormatItem>();

            return std::make_shared<OtherFormatItem>(val);
        }

    private:
        std::string _pattern;       // 格式化规则字符串
        std::vector<FormatItem::ptr> _items;
    };
}

#endif