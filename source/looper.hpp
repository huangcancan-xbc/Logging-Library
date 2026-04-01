// 异步工作器：业务线程写缓冲，后台线程统一消费落地

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
        AsyncLooper();              // 启动后台线程

        void stop();                // 停止并等待线程退出
        void push(const char *data, size_t len);  // 业务线程写入数据

    private:
        void threadEntry();         // 后台线程入口：消费数据并调用回调

    private:
        bool _stop;                 // 停止标志
        Buffer _pro_buf;            // 生产者缓冲（业务线程写）
        Buffer _con_buf;            // 消费者缓冲（后台线程读）
        std::mutex _mutex;          // 保护双缓冲交换
        std::condition_variable _cond_pro;  // 通知生产者：缓冲已空可写
        std::condition_variable _cond_con;  // 通知消费者：缓冲有数据可读
        std::thread _thread;        // 后台工作线程

        Function _callBack;         // 数据处理回调
    };
}