#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "mini_spdlog/mini_spdlog.h"

using namespace mini_spdlog;

int main() {
    try {
        // 1. 初始化控制台和文件 Sink
        auto console = std::make_shared<stdout_sink>();
        auto file    = std::make_shared<file_sink>("../../logs/stress_test.txt");

        // 2. 注册一个新的 Logger
        auto multi_logger = std::make_shared<logger>("multi_worker");
        multi_logger->add_sink(console);
        multi_logger->add_sink(file);
        mini_spdlog::register_logger(multi_logger);

        // 3. 准备并发测试
        const int thread_count    = 10;
        const int logs_per_thread = 100;
        std::vector<std::thread> workers;

        std::cout << "Starting stress test with " << thread_count << " threads...\n";

        for (int i = 0; i < thread_count; ++i) {
            workers.emplace_back([i, multi_logger]() {
                for (int j = 0; j < logs_per_thread; ++j) {
                    // 测试不同级别和格式化内容
                    multi_logger->info("Thread {} is logging message #{}", i, j);

                    if (j == 50) {
                        // 模拟在运行期间设置格式（测试 RCU 线程安全性）
                        multi_logger->set_pattern("[%H:%M:%S] [Thread-Specific] %v");
                    }

                    std::this_thread::sleep_for(std::chrono::microseconds(10));
                }
            });
        }

        // 4. 同时在主线程测试 Registry 的全局访问
        std::thread config_thread([]() {
            for (int i = 0; i < 5; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
                // 测试动态增加 Sink 是否会导致正在 log 的线程崩溃
                auto extra = std::make_shared<file_sink>("../../logs/log.txt");
                mini_spdlog::get("multi_worker")->add_sink(extra);
                mini_spdlog::warn("Dynamic config: Added a new stdout_sink");
            }
        });

        for (auto& t : workers) t.join();
        config_thread.join();

        // 5. 测试默认 Logger
        mini_spdlog::set_pattern("%Y-%m-%d %H:%M:%S | %l | %v");
        mini_spdlog::critical("Final system check: All threads finished. Result code: {}", 0);

        // 6. 测试异步 创建真实的落地 Sink
        auto real_file_sink = std::make_shared<file_sink>("../../logs/async_log.txt");

        // 用异步外壳包裹它
        auto async_wrapper = std::make_shared<async_sink>(real_file_sink);

        // 将异步外壳交给 Logger
        auto logger = std::make_shared<mini_spdlog::logger>("async_logger");
        logger->add_sink(async_wrapper);

        // 现在调用 info，主线程会瞬间返回
        for (int i = 0; i < 1000; ++i) {
            logger->info("This is an async message #{}", i);
        }

        std::cout << "Test completed successfully. Check stress_test.txt for logs.\n";

    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
    }

    return 0;
}