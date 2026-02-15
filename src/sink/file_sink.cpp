#include "mini_spdlog/sink/file_sink.h"

namespace mini_spdlog {
    
file_sink::file_sink(const std::string& filename)
    : file_(filename, std::ios::app) {}

void mini_spdlog::file_sink::log(const std::string& formatted_msg) {
    std::lock_guard lock(mutex_);
    file_ << formatted_msg << std::endl;
}

}  // namespace mini_spdlog
