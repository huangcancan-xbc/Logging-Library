// 实用工具类的实现

// 获取系统时间
// 判断文件是否存在
// 获取文件所在路径
// 创建目录

#ifndef __M_UTIL_H_
#define __M_UTIL_H_

#include <iostream>
#include <ctime>
// #include <unistd.h>     // access
#include <sys/stat.h>       // stat

namespace mylog
{
    namespace util
    {
        class Date
        {
        public:
            // 获取系统时间
            static size_t now()
            {
                return (size_t)time(nullptr);
            }
        };

        class File
        {
        public:
            // 判断文件是否存在
            static bool isExist(const std::string &pathname)
            {
                // // （man）access用于检查当前进程是否具有某种访问权限（读/写/执行），或者文件是否存在
                // // 参数：文件路径、检查名称（F_OK：文件是否存在，R_OK：是否可读，W_OK：写，X_OK：执行）
                // // 成功返回0失败-1
                // return (access(pathname.c_str(), F_OK) == 0);



                // stat：Linux原生 + Windows有对应版本。可以拿到文件信息或判断文件存在（man fstat）
                // 参数：文件路径、struct stat用于接收文件信息
                // 成功返回0表示存在，失败-1不存在
                struct stat st;
                if(stat(pathname.c_str(), &st) < 0)
                {
                    return false;
                }

                return true;
            }

            // 获取文件所在路径
            static std::string getPath(const std::string &pathname)
            {
                // find_last_of：在当前字符串的pos索引位置开始，查找最后一个位于子串s的字符
                // 参数：目标字符串，起始位置（默认从0开始）
                // 找到返回第一次匹配的下标，否则npos（-1）
                size_t pos = pathname.find_last_of("/\\");
                if(pos == std::string::npos)
                {
                    return ".";
                }

                return pathname.substr(0, pos + 1); // 要当前目录的路径
            }

            // 创建目录
            static void createDirectory(const std::string &pathname)
            {
                size_t pos = 0, index = 0;
                while(index < pathname.size())
                {
                    // 从index开始找下一个路径分隔符
                    pos = pathname.find_first_of("/\\", index);

                    // 如果找不到，说明已经到最后一段了
                    if(pos == std::string::npos)
                    {
                        if(!isExist(pathname))
                        {
                            mkdir(pathname.c_str(), 0777);
                        }
                        break;
                    }

                    // 逐步构造父目录：
                    // "./"
                    // "./abc/"
                    // "./abc/bcd/"
                    std::string parent_dir = pathname.substr(0, pos + 1);

                    // 如果这一层已经存在，就继续处理下一层
                    if(isExist(parent_dir))
                    {
                        index = pos + 1;
                        continue;
                    }
                    
                    mkdir(parent_dir.c_str(), 0777);    // 创建当前这一层目录
                    index = pos + 1;                    // 继续处理后面的路径
                }
            }
        };
    }
}

#endif