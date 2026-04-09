// 异步日志缓冲区
// 业务线程不直接落地，而是写入缓冲区，由专门的异步线程统一消费落地

// 单缓冲区/队列：用队列缓存日志消息，逐条处理涉及空间频繁申请释放，互斥严重，效率低
// 环形缓冲区：提前将空间申请好，对空间循环利用，减少IO次数，但锁冲突仍严重
// 双缓冲区：当一个缓冲区任务处理完就交换2个缓冲区。读写分离，省去了频繁申请释放内存的开销，而且
// 一个处理一个处理锁冲突大，这里用一次性处理一批任务，大大减少了冲突，效率会更高

#pragma once

#include "util.hpp"
#include <vector>
#include <cassert>



namespace mylog
{
    // 默认缓冲区大小：100MB
    #define DEFAULT_BUFFER_SIZE (100 * 1024 * 1024)

    #define THRESHOLD_BUFFER_SIZE (80 * 1024 * 1024)    // 缓冲区阈值大小
    #define LINEAR_BUFFER_SIZE (10 * 1024 * 1024)       // 当超过缓冲区阈值就线性增长空间

    class Buffer
    {
    public:
        Buffer()
            : _buffer(DEFAULT_BUFFER_SIZE),
            _reader_idx(0), _writer_idx(0)
        {
            
        }


        void push(const char *data, size_t len)     // 向缓冲区写入数据
        {
            // 缓冲区剩余空间不足的处理（2种思路）
            // a.固定大小，直接返回
            // if(len > writeAbleSize())
            // {
            //     return;
            // }
            // b.直接扩容
            SufficientSpaceSize(len);

            // 1.把数据拷贝到缓冲区：std::copy(首, 尾, 目标)
            std::copy(data, data + len, &_buffer[_writer_idx]);

            // 2.写指针后移
            moveWriter(len);
        }

        const char* begin()                         // 返回可读数据的起始地址
        {
            return &_buffer[_reader_idx];
        }

        size_t readAbleSize()                       // 返回可读数据的长度
        {
            return (_writer_idx - _reader_idx);
        }

        size_t writeAbleSize()                      // 返回可写数据长度
        {
            return (_buffer.size() - _writer_idx);
        }

        void reset()                                // 重置读写位置，初始化缓冲区
        {
            _reader_idx = 0;
            _writer_idx = 0;
        }

        void swap(Buffer &buffer)                   // 对缓冲区实现交换
        {
            _buffer.swap(buffer._buffer);
            std::swap(_reader_idx, buffer._reader_idx);
            std::swap(_writer_idx, buffer._writer_idx);
        }

        bool empty()                                // 判断缓冲区是否为空
        {
            return (_reader_idx == _writer_idx);
        }
        
        void moveReader(size_t len)                 // 读指针后移
        {
            assert(len <= readAbleSize());
            _reader_idx += len;
        }

    private:

        void moveWriter(size_t len)                 // 写指针后移
        {
            assert(len <= writeAbleSize());
            // assert(len + _writer_idx <= _buffer.size());
            _writer_idx += len;
        }

        void SufficientSpaceSize(size_t len)        // 扩容足够空间大小
        {
            if(len <= writeAbleSize())
            {
                return;     // 空间足够不扩容
            }

            size_t new_size = 0;

            // 加上len主要是防止日志信息过于庞大，导致扩容了还不够的情况
            if (_buffer.size() < THRESHOLD_BUFFER_SIZE)
            {
                new_size = _buffer.size() * 2 + len;      // 当缓冲区空间小于阈值时2倍扩容
            }
            else
            {
                new_size = _buffer.size() + LINEAR_BUFFER_SIZE + len;     // 当缓冲区空间超过阈值线性扩容
            }

            _buffer.resize(new_size);
        }

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