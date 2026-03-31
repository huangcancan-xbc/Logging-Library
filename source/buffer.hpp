// 异步日志缓冲区
// 业务线程不直接落地，而是写入缓冲区，由专门的异步线程统一消费落地

// 单缓冲区/队列：用队列缓存日志消息，逐条处理涉及空间频繁申请释放，互斥严重，效率低
// 环形缓冲区：提前将空间申请好，对空间循环利用，减少IO次数，但锁冲突仍严重
// 双缓冲区：当一个缓冲区任务处理完就交换2个缓冲区。读写分离，省去了频繁申请释放内存的开销，而且
// 一个处理一个处理锁冲突大，这里用一次性处理一批任务，大大减少了冲突，效率会更高



#include "util.hpp"
#include <vector>

namespace mylog
{
    class Buffer
    {
    public:
        Buffer();

        void push(const char *data, size_t len);    // 向缓冲区写入数据
        const char* begin();                        // 返回可读数据的起始地址
        size_t readAbleSize();                      // 返回可读数据的长度
        void reset();                               // 重置读写位置，初始化缓冲区
        void swap(const Buffer &buffer);            // 对缓冲区实现交换
        bool empty();                               // 判断缓冲区是否为空
    
    private:
        void moveReader(size_t len);

    private:
        // 设计：直接存放格式化后的字符串，避免LogMsg对象频繁构造
        // 1.管理一个存放字符串数据的缓冲区（用vector进行空间管理）
        // 2.当前写入数据位置的指针（指向可写区域的起始位置，避免写入覆盖）
        // 3.当前读取数据位置的指针（指向可读数据区域的起始位置，当读取指针与写入指针指向相同位置表示数据取完了）

        std::vector<char> _buffer;
        size_t _reader_idx;         // 读数据指针（本质下标）
        size_t _writer_idx;         // 写数据指针
    };
}