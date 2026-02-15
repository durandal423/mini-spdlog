#ifndef ASYNC_SINK_H
#define ASYNC_SINK_H

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "mini_spdlog/sink/sink.h"

namespace mini_spdlog {

class async_sink : public sink {
public:
    explicit async_sink(std::shared_ptr<sink> target_sink)
        : target_sink_(std::move(target_sink))
        , should_exit_(false) {
        // 启动后台线程
        worker_thread_ = std::jthread([this] { this->worker_loop(); });
    }

    ~async_sink() override {
        {
            std::lock_guard lock(queue_mutex_);
            should_exit_ = true;
        }
        cv_.notify_all();
        // jthread 会自动 join
    }

    void log(const std::string& formatted_msg) override {
        {
            std::lock_guard lock(queue_mutex_);
            // 这里可以添加队列大小限制策略（比如超过 10000 条就丢弃或阻塞）
            msg_queue_.push(formatted_msg);
        }
        cv_.notify_one();
    }

private:
    void worker_loop() {
        while (true) {
            std::vector<std::string> local_msgs;
            {
                std::unique_lock lock(queue_mutex_);
                // 等待有消息或者退出信号
                cv_.wait(lock, [this] {
                    return !msg_queue_.empty() || should_exit_;
                });

                if (should_exit_ && msg_queue_.empty()) {
                    break;
                }

                // 核心优化：批量取出消息，减少锁竞争
                while (!msg_queue_.empty()) {
                    local_msgs.push_back(std::move(msg_queue_.front()));
                    msg_queue_.pop();
                }
            }

            // 在锁之外执行耗时的 I/O 操作
            for (const auto& msg : local_msgs) {
                target_sink_->log(msg);
            }
        }
    }

    std::shared_ptr<sink> target_sink_;
    std::queue<std::string> msg_queue_;
    std::mutex queue_mutex_;
    std::condition_variable cv_;
    std::atomic<bool> should_exit_;
    std::jthread worker_thread_;
};

}  // namespace mini_spdlog

#endif  // ASYNC_SINK_H