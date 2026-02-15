#include "mini_spdlog/sink/stdout_sink.h"

namespace mini_spdlog {
    
std::mutex stdout_sink::mutex_;

void mini_spdlog::stdout_sink::log(const std::string& formatted_msg) {
    std::lock_guard lock(mutex_);
    std::cout << formatted_msg << std::endl;
}
    
} // namespace mini_spdlog


