// 扩展：基于时间的滚动文件实现（每隔指定时间段自动切换文件）
// 通过系统时间对时间段大小取模，确定当前属于哪个时间片段
// 在写入日志前，用当前时间取模判断是否超出当前时间段，如果超出就切换到新文件

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

        _cur_gap = _gap_size == 1 ? mylog::util::Date::now() : mylog::util::Date::now() / _gap_size; // 获取初始时间段的标识值，用于后续比较判断

        std::string filename = createNewFile();                                     // 创建日志文件并设置初始文件路径
        mylog::util::File::createDirectory(mylog::util::File::getPath(filename));   // 确保日志文件所在目录存在
        _ofs.open(filename, std::ios::binary | std::ios::app);                      // 以追加模式打开文件
        assert(_ofs.is_open());
    }

    // 检查当前时间是否仍在当前时间段内，若不在则切换到新文件
    void log(const char *data, size_t len)
    {
        time_t cur = mylog::util::Date::now();  // 获取当前时间戳
        
        if((cur / _gap_size) != _cur_gap)       // 判断当前时间是否超出当前时间段
        {
            _cur_gap = cur / _gap_size;          // 更新当前时间段标识
            _ofs.close();                        // 关闭当前文件，准备切换
            std::string pathname = createNewFile();
            _ofs.open(pathname, std::ios::binary | std::ios::app);
            assert(_ofs.is_open());
        }

        _ofs.write(data, len);
        assert(_ofs.good());                    // 检查文件写入是否成功
    }

private:
    std::string createNewFile()                 // 根据当前系统时间生成新的日志文件名
    {
        time_t t = mylog::util::Date::now();    // 获取当前时间戳

        // 将时间戳分解为年、月、日、时、分、秒等时间信息（线程安全）
        struct tm lt;
        localtime_r(&t, &lt);

        // 构建带时间戳的日志文件名
        std::stringstream filename;
        filename << _basename << "_";
        filename << lt.tm_year + 1900 << "-";       // 年：tm_year 从 1900 开始计数
        filename << std::setfill('0') << std::setw(2) << (lt.tm_mon + 1) << "-";    // 月：tm_mon 从 0 开始，0 表示 1 月
        filename << lt.tm_mday << " ";
        filename << lt.tm_hour << ":";
        filename << lt.tm_min << ":";
        filename << lt.tm_sec;
        filename << ".log";

        return filename.str();
    }

private:
    std::string _basename;      // 日志文件的基础名称
    std::ofstream _ofs;         // 输出文件流
    size_t _cur_gap;            // 当前处于的连续时间段计数
    size_t _gap_size;           // 设置的时间滚动间隔时长（单位：秒）
};