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

    // 双缓冲生产者-消费者模型，读写分离减少锁竞争
    class AsyncLooper
    {
    public:
        using ptr = std::shared_ptr<AsyncLooper>;

        AsyncLooper(Function &cb)   // 启动后台线程
            : _stop(false),
            _thread(std::thread(&AsyncLooper::threadEntry, this)),
            _callBack(cb)
        {

        }

        void stop()                 // 停止并等待线程退出
        {
            _stop = true;           // 告诉线程该停了
            _cond_con.notify_all(); // 唤醒工作线程
            _thread.join();
        }

        void push(const char* data, size_t len)                 // 业务线程写入数据
        {
            // a.固定大小：生产缓冲区中数据满了就阻塞等待消费者取走
            // b.无限扩容：空间不够就让buffer自动扩容，生产者不阻塞
            std::unique_lock<std::mutex> lock(_mutex);          // 1.加锁

            // wait：让线程睡觉，等条件成立再继续（解锁 + 睡觉 + 被唤醒 + 重新加锁）
            // 参数：锁（必须unique_lock！）、一个返回bool的东西（即什么时候可以继续往下走？）
            _cond_pro.wait(lock, [&]()
                { return len <= _pro_buf.writeAbleSize(); });   // 2.等可写空间

            _pro_buf.push(data, len);                           // 3.写数据
            _cond_con.notify_one();                             // 4.通知/唤醒消费者对缓冲区数据进行处理
        }

    private:
        void threadEntry()          // 后台线程入口：消费数据并调用回调
        {
            while(!_stop)
            {
                // 不对数据处理加锁保护，只对缓冲区交换加锁
                {
                    std::unique_lock<std::mutex> lock(_mutex);  // 1.加锁
                    _cond_con.wait(lock, [&]()
                                   { return (_stop || !_pro_buf.empty()); });    // 2.等有数据
                    _con_buf.swap(_pro_buf);                    // 3.交换缓冲区
                    _cond_pro.notify_all();                     // 4.通知生产者
                }

                _callBack(_con_buf);                            // 5.处理数据
                _con_buf.reset();                               // 6.清空消费缓冲区
            }
        }

    private:
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