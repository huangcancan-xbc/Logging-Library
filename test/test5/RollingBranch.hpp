// 扩展：用时间来实现滚动文件（每隔xx的时间就切换文件）
// 以当前系统时间，取模时间段大小，可以得到当前时间段是第几个时间段
// 每次以当前系统时间取模，判断与当前文件的时间段是否一致，不一致代表不是同一个时间段

#pragma once
#include "../../source/sink.hpp"

enum class TimeGap
{
    GAP_SECOUND,
    GAP_MINUTE,
    GAP_HOUR,
    GAP_DAY,
};


class RollByTimeSink : public mylog::LogSink
{
public:
    RollByTimeSink(const std::string &basename, TimeGap gap_type)
        : _basename(basename)
    {
        switch(gap_type)
        {
            case TimeGap::GAP_SECOUND: _gap_size = 1; break;
            case TimeGap::GAP_MINUTE: _gap_size = 60; break;
            case TimeGap::GAP_HOUR:_gap_size = 3600; break;
            case TimeGap::GAP_DAY:_gap_size = 3600 * 24; break;
        }

        _cur_gap = _gap_size == 1 ? mylog::util::Date::now() : mylog::util::Date::now() % _gap_size;

        std::string filename = createNewFile();
        mylog::util::File::createDirectory(mylog::util::File::getPath(filename));
        _ofs.open(filename, std::ios::binary | std::ios::app);
        assert(_ofs.is_open());
    }

    // 判断当前时间是否是当前文件的时间段，不是就切换文件
    void log(const char *data, size_t len)
    {
        time_t cur = mylog::util::Date::now();
        if((cur % _gap_size) != _cur_gap)
        {
            _ofs.close();               // 先关闭当前文件！
            std::string pathname = createNewFile();
            _ofs.open(pathname, std::ios::binary | std::ios::app);
            assert(_ofs.is_open());
        }

        _ofs.write(data, len);
        assert(_ofs.good());            // 检查文件操作有没有出错
    }

private:
    std::string createNewFile()         // 进行大小判断，超过最大大小则创建新文件
    {
        time_t t = mylog::util::Date::now();   // 获取系统时间

        // localtime_r能将时间戳分解成年、月、日、时、分、秒等（线程安全）
        // 参数：指向time_t时间戳的指针、struct tm 结构体，成功返回struct tm 结构体指针，失败为nullptr
        struct tm lt;
        localtime_r(&t, &lt);

        std::stringstream filename;     // 创建一个流用于存放以时间格式的日志文件名
        filename << _basename << "_";
        filename << lt.tm_year + 1900 << "-";       // 年：tm_year 从 1900 开始计数
        filename << std::setfill('0') << std::setw(2) << (lt.tm_mon + 1) << "-";           // 月：tm_mon 从 0 开始，0 表示 1 月
        filename << lt.tm_mday << " ";
        filename << lt.tm_hour << ":";
        filename << lt.tm_min << ":";
        filename << lt.tm_sec << "-";
        filename << ".log";

        return filename.str();
    }

private:
    std::string _basename;      // 输出当前文件名，格式：基础文件名+扩展文件名（用时间生成）的组合
    std::ofstream _ofs;         // 输出文件流
    size_t _cur_gap;            // 当前是第几个时间段
    size_t _gap_size;           // 时间段的大小
};