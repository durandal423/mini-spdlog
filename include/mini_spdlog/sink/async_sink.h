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
    explicit async_sink(std::shared_ptr<sink> target_sink);

    ~async_sink() override;

    void log(const std::string& formatted_msg) override;

private:
    void worker_loop();

    std::shared_ptr<sink> target_sink_;
    std::queue<std::string> msg_queue_;
    std::mutex queue_mutex_;
    std::condition_variable cv_;
    std::atomic<bool> should_exit_;
    std::jthread worker_thread_;
};

}  // namespace mini_spdlog

#endif  // ASYNC_SINK_H