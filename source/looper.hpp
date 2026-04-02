// 异步工作器：业务线程写缓冲，后台线程统一消费落地
// 业务线程：负责"生产"日志，往缓冲区扔，扔完就走
// 后台线程：负责"消费"日志，批量取走，统一写文件
// 两个线程通过两个缓冲区+条件变量协作，互不阻塞


#include "buffer.hpp"
#include <mutex>
#include <condition_variable>
#include <thread>
#include <functional>
#include <memory>



namespace mylog
{
    // 回调：接收一个 Buffer 自行处理
    using Function = std::function<void(Buffer &)>;

    enum class AsyncType
    {
        ASYNC_SAFE,   // 安全状态，缓冲区满了就阻塞，能够避免资源耗尽
        ASYNC_UNSAFE, // 不安全状态，缓冲区空间不够就扩容，用于压力测试（生产者永不阻塞！）
    };


    // 双缓冲生产者-消费者模型，读写分离减少锁竞争
    class AsyncLooper
    {
    public:
        using ptr = std::shared_ptr<AsyncLooper>;

        AsyncLooper(const Function &cb, AsyncType looper_type = AsyncType::ASYNC_SAFE)   // 启动后台线程
            : _looper_type(looper_type),
            _callBack(cb),
            _stop(false),
            _thread(std::thread(&AsyncLooper::threadEntry, this))
        {

        }

        ~AsyncLooper()
        {
            stop();
        }

        void stop()                     // 停止并等待线程退出
        {
            if (_thread.joinable())     // 看看现在能不能安全地调 join，不管线程到底跑没跑完
            {
                _stop = true;           // 告诉线程该停了
                _cond_con.notify_all(); // 唤醒工作线程
                _thread.join();
            }
        }

        void push(const char* data, size_t len)                 // 业务线程写入数据
        {
            // a.固定大小：生产缓冲区中数据满了就阻塞等待消费者取走
            // b.无限扩容：空间不够就让buffer自动扩容，生产者不阻塞
            std::unique_lock<std::mutex> lock(_mutex);          // 1.加锁

            // 安全：空间不够->睡觉等->要唤醒
            // 不安全：空间不够->buffer自动扩容->不睡觉->不需要通知生产者
            if (_looper_type == AsyncType::ASYNC_SAFE)
            {
                // wait：让线程睡觉，等条件成立再继续（解锁 + 睡觉 + 被唤醒 + 重新加锁）
                // 参数：锁（必须unique_lock！）、一个返回bool的东西（即什么时候可以继续往下走？）
                _cond_pro.wait(lock, [&]()
                               { return len <= _pro_buf.writeAbleSize(); }); // 2.等可写空间
            }

            _pro_buf.push(data, len);                           // 3.写数据
            _cond_con.notify_one();                             // 4.通知/唤醒消费者对缓冲区数据进行处理
        }

    private:
        void threadEntry()          // 后台线程入口：消费数据并调用回调
        {
            while(true)
            {
                // 不对数据处理加锁保护，只对缓冲区交换加锁
                {
                    std::unique_lock<std::mutex> lock(_mutex);  // 1.加锁

                    // 退出的标志被设置并且缓冲区中没有数据了，这个时候再退出，否则缓冲区可能残留数据没处理就退出了
                    if(_stop && _pro_buf.empty())
                    {
                        break;
                    }

                    _cond_con.wait(lock, [&]()
                                   { return (_stop || !_pro_buf.empty()); });    // 2.等有数据
                    _con_buf.swap(_pro_buf);                    // 3.交换缓冲区

                    if (_looper_type == AsyncType::ASYNC_SAFE)
                    {
                        _cond_pro.notify_all();                     // 4.通知生产者
                    }
                }

                _callBack(_con_buf);                            // 5.处理数据
                _con_buf.reset();                               // 6.清空消费缓冲区
            }
        }

    private:
        AsyncType _looper_type;     // 缓冲区策略：阻塞 or 扩容
        bool _stop;                 // 停止标志：true表示退出，false表示正常运行
        Buffer _pro_buf;            // 生产者缓冲（业务线程写）
        Buffer _con_buf;            // 消费者缓冲（后台线程读）
        std::mutex _mutex;          // 保护双缓冲交换
        std::condition_variable _cond_pro;  // 通知生产者：缓冲已空可写
        std::condition_variable _cond_con;  // 通知消费者：缓冲有数据可读
        std::thread _thread;        // 后台工作线程

        Function _callBack;         // 数据处理回调
    };
}