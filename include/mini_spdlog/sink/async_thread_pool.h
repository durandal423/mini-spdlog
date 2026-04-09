#ifndef ASYNC_THREAD_POOL_H
#define ASYNC_THREAD_POOL_H

#include <condition_variable>
#include <cstddef>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "mini_spdlog/sink/sink.h"

namespace mini_spdlog {

enum class async_overflow_policy {
    block,
    overrun_oldest,
};

class async_thread_pool {
public:
    async_thread_pool(std::size_t queue_size, std::size_t thread_count);
    ~async_thread_pool();

    static std::shared_ptr<async_thread_pool> default_instance();

    void post(std::shared_ptr<sink> sink_target, std::string msg, async_overflow_policy policy);
    void flush();

private:
    struct async_task {
        std::shared_ptr<sink> sink_target;
        std::string msg;
    };

    std::size_t queue_size_;
    std::queue<async_task> queue_;
    std::mutex mutex_;
    std::condition_variable push_cv_;
    std::condition_variable pop_cv_;
    std::condition_variable drained_cv_;
    bool stopping_;
    std::size_t active_workers_;
    std::vector<std::jthread> workers_;

    void worker_loop();
};

}  // namespace mini_spdlog

#endif  // ASYNC_THREAD_POOL_H
