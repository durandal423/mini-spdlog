#include "mini_spdlog/sink/async_thread_pool.h"

#include <algorithm>
#include <stdexcept>

namespace mini_spdlog {

async_thread_pool::async_thread_pool(std::size_t queue_size, std::size_t thread_count)
    : queue_size_(queue_size)
    , stopping_(false)
    , active_workers_(0) {
    if (queue_size_ == 0) {
        throw std::invalid_argument("async_thread_pool queue_size must be > 0");
    }

    const auto workers = std::max<std::size_t>(1, thread_count);
    workers_.reserve(workers);
    for (std::size_t i = 0; i < workers; ++i) {
        workers_.emplace_back([this] { worker_loop(); });
    }
}

async_thread_pool::~async_thread_pool() {
    {
        std::lock_guard lock(mutex_);
        stopping_ = true;
    }
    pop_cv_.notify_all();
    push_cv_.notify_all();
}

std::shared_ptr<async_thread_pool> async_thread_pool::default_instance() {
    static auto pool = std::make_shared<async_thread_pool>(8192, 1);
    return pool;
}

void async_thread_pool::post(std::shared_ptr<sink> sink_target, std::string msg, async_overflow_policy policy) {
    if (!sink_target) {
        return;
    }

    std::unique_lock lock(mutex_);

    if (policy == async_overflow_policy::block) {
        push_cv_.wait(lock, [this] {
            return queue_.size() < queue_size_ || stopping_;
        });
    } else if (queue_.size() >= queue_size_) {
        queue_.pop();
    }

    if (stopping_) {
        return;
    }

    queue_.push(async_task {std::move(sink_target), std::move(msg)});
    pop_cv_.notify_one();
}

void async_thread_pool::flush() {
    std::unique_lock lock(mutex_);
    drained_cv_.wait(lock, [this] {
        return queue_.empty() && active_workers_ == 0;
    });
}

void async_thread_pool::worker_loop() {
    while (true) {
        async_task task;
        {
            std::unique_lock lock(mutex_);
            pop_cv_.wait(lock, [this] {
                return stopping_ || !queue_.empty();
            });

            if (stopping_ && queue_.empty()) {
                break;
            }

            task = std::move(queue_.front());
            queue_.pop();
            ++active_workers_;
            push_cv_.notify_one();
        }

        task.sink_target->log(task.msg);

        {
            std::lock_guard lock(mutex_);
            --active_workers_;
            if (queue_.empty() && active_workers_ == 0) {
                drained_cv_.notify_all();
            }
        }
    }

    std::lock_guard lock(mutex_);
    if (queue_.empty() && active_workers_ == 0) {
        drained_cv_.notify_all();
    }
}

}  // namespace mini_spdlog
