// 测试日志器模块 —— 异步缓冲区模块


#include <iostream>
#include "../../source/logger.hpp"
#include "../../source/buffer.hpp"
#include <unistd.h>



void test1()
{
    std::ifstream ifs("./a.log", std::ios::binary);
    if(ifs.is_open() == false)
    {
        std::cout << "open faild!\n";
    }

    ifs.seekg(0, std::ios::end);        // 把光标移到文件末尾
    size_t fsize = ifs.tellg();         // 拿到已有内容的大小/长度
    ifs.seekg(0, std::ios::beg);        // 再把光标移到开头

    std::string body;
    body.resize(fsize);
    ifs.read(&body[0], fsize);          // 从body中拿fsize的数据
    if(ifs.good() == false)
    {
        std::cout << "read error\n";
    }

    ifs.close();

    // 创建缓冲区，每次写入一个字节数据
    mylog::Buffer buffer;
    for (int i = 0; i < body.size(); i++)
    {
        buffer.push(&body[i], 1);
    }

    // 每次往b.log中写入一个字节数据
    std::ofstream ofs("./b.log", std::ios::binary);
    // ofs.write(buffer.begin(), buffer.readAbleSize());    // 一次性将缓冲区全部数据拿出
    while (buffer.readAbleSize() > 0)
    {
        ofs.write(buffer.begin(), 1);
        buffer.moveReader(1);
    }

    ofs.close();
}

int main()
{
    test1();

    return 0;
}
