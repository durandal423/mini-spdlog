#ifndef ASYNC_SINK_H
#define ASYNC_SINK_H

#include <memory>

#include "mini_spdlog/sink/async_thread_pool.h"
#include "mini_spdlog/sink/sink.h"

namespace mini_spdlog {

class async_sink : public sink {
public:
    explicit async_sink(
        std::shared_ptr<sink> target_sink,
        std::shared_ptr<async_thread_pool> thread_pool = async_thread_pool::default_instance(),
        async_overflow_policy overflow_policy = async_overflow_policy::block);

    ~async_sink() override = default;

    void log(const std::string& formatted_msg) override;
    void flush() override;

private:
    std::shared_ptr<sink> target_sink_;
    std::shared_ptr<async_thread_pool> thread_pool_;
    async_overflow_policy overflow_policy_;
};

}  // namespace mini_spdlog

#endif  // ASYNC_SINK_H
