#include "mini_spdlog/sink/async_sink.h"

#include <stdexcept>

namespace mini_spdlog {

async_sink::async_sink(
    std::shared_ptr<sink> target_sink,
    std::shared_ptr<async_thread_pool> thread_pool,
    async_overflow_policy overflow_policy)
    : target_sink_(std::move(target_sink))
    , thread_pool_(std::move(thread_pool))
    , overflow_policy_(overflow_policy) {
    if (!target_sink_) {
        throw std::invalid_argument("async_sink requires a non-null target sink");
    }

    if (!thread_pool_) {
        throw std::invalid_argument("async_sink requires a non-null thread pool");
    }
}

void async_sink::log(const std::string& formatted_msg) {
    thread_pool_->post(target_sink_, formatted_msg, overflow_policy_);
}

void async_sink::flush() {
    thread_pool_->flush();
    target_sink_->flush();
}

}  // namespace mini_spdlog
