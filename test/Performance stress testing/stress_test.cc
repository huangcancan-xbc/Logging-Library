// 压力性能测试


#include "../../source/mylog.h"
#include <chrono>
#include <algorithm>


// 参数: 日志器名, 线程数, 日志总条数, 单条日志字节数
void stress_test(const std::string &logger_name, size_t thread_count, size_t total_msg_count, size_t msg_bytes)
{
    // 1.获取日志器名
    mylog::Logger::ptr logger = mylog::getLogger(logger_name);
    if (!logger)
    {
        std::cout << "Logger: " << logger_name << " not found!\n";
        return;
    }

    // 2.构造数据 (预留1字节给\0，防截断)
    std::string msg(msg_bytes - 1, 'A');

    // 3. 准备多线程环境
    std::vector<std::thread> threads;
    threads.reserve(thread_count);  // 预分配，避免扩容开销
    
    // 平均每个线程写多少条 + 余数处理，防止漏写
    size_t base_count = total_msg_count / thread_count; // 每个线程要处理的日志数量 = 日志总条数 / 线程数
    size_t remainder = total_msg_count % thread_count;  // 余数
    
    // 统计每个线程耗时 (纳秒)
    std::vector<long long> thread_costs(thread_count, 0);

    // 写入速度
    double total_mb = static_cast<double>(msg_bytes) * total_msg_count / 1024.0 / 1024.0;

    // 测试信息/配置
    std::cout << "\n压测配置：\n";
    std::cout << "\t日志器: " << logger_name << '\n';
    std::cout << "\t并发线程: " << thread_count << '\n';
    std::cout << "\t单条大小: " << msg_bytes << " B\n";
    std::cout << "\t总条数: " << total_msg_count << '\n';
    std::cout << "\t总数据量: " << std::fixed << std::setprecision(2) << total_mb << " MB\n";

    // 全局计时开始
    auto global_start = std::chrono::high_resolution_clock::now();

    // 起线程开写
    for (size_t i = 0; i < thread_count; ++i)
    {
        // 最后一个线程吃掉余数，保证总条数准确
        size_t count = (i == thread_count - 1) ? (base_count + remainder) : base_count;
        
        threads.emplace_back([&, i, count]() {
            auto t_start = std::chrono::high_resolution_clock::now();
            
            for (size_t j = 0; j < count; j++)
            {
                logger->fatal("%s", msg.c_str());
            }
            
            auto t_end = std::chrono::high_resolution_clock::now();
            thread_costs[i] = std::chrono::duration_cast<std::chrono::nanoseconds>(t_end - t_start).count();
            
            // 单线程结果（debug）
            std::cout << "[线程" << i << "] 完成 " << count << " 条 | 耗时 " << thread_costs[i] / 1e6 << " ms（" << thread_costs[i] / 1e9 <<" s）\n";
        });
    }

    // 等所有线程跑完
    for (auto &t : threads)
    {
        t.join();
    }

    // 全局计时结束
    auto global_end = std::chrono::high_resolution_clock::now();
    double global_sec = std::chrono::duration<double>(global_end - global_start).count();

    // 找最慢的线程，看看负载均不均匀
    long long max_cost_ns = *std::max_element(thread_costs.begin(), thread_costs.end());

    // 写入速度：万条/秒
    double throughput_wan = total_msg_count / global_sec / 10000.0;
    // 写入速度：MB/秒
    double throughput_mb = total_mb / global_sec;
    // 平均每条耗时：微秒级
    double avg_latency_us = global_sec * 1e6 / total_msg_count;

    std::cout << "\n================== 压测结果 ======================\n";
    std::cout << "\t总耗时: " << std::fixed << std::setprecision(3) << global_sec << " s\n";
    std::cout << "\t最慢线程: " << max_cost_ns / 1e6 << " ms（" << max_cost_ns / 1e9 << " s）\n";
    std::cout << "\t写入速度: " << std::setprecision(2) << throughput_wan << " 万条/秒\n";
    std::cout << "\t写入速度: " << throughput_mb << " MB/秒\n";
    std::cout << "\t平均延迟: " << avg_latency_us << " μs/条\n";
}


void sync_test()
{
    std::unique_ptr<mylog::LoggerBuilder> builder(new mylog::GlobalLoggerBuilder());
    builder->buildLoggerName("sync_logger");
    builder->buildFormatter("%m%n");
    builder->buildLoggerType(mylog::LoggerType::LOGGER_SYNC);
    builder->buildSink<mylog::FileSink>("./logs/sync.log");
    builder->build();

    stress_test("sync_logger", 1, 10000000, 100);
}


void async_test()
{
    std::unique_ptr<mylog::LoggerBuilder> builder(new mylog::GlobalLoggerBuilder());
    builder->buildLoggerName("async_logger");
    builder->buildFormatter("%m%n");
    builder->buildLoggerType(mylog::LoggerType::LOGGER_ASYNC);
    builder->buildEnableUnSafeAsync();
    builder->buildSink<mylog::FileSink>("./logs/async.log");
    builder->build();

    stress_test("async_logger", 1, 10000000, 100);
}


int main()
{
    sync_test();
    // async_test();

    return 0;
}