// 实用工具类的实现

// 获取系统时间
// 判断文件是否存在
// 获取文件所在路径
// 创建目录


#include <iostream>
#include <ctime>

namespace mylog
{
    namespace util
    {
        class Date
        {
        public:
            static size_t getTime()
            {
                return (size_t)time(nullptr);
            }
        };

        class File
        {
        public:
            static bool isExist(const std::string &pathname);
            static std::string path(const std::string &pathname);
            void createDirectory(const std::string &pathname);
        };
    }
}