#include "mini_spdlog/sink/file_sink.h"

namespace mini_spdlog {

void mini_spdlog::file_sink::log(const std::string& formatted_msg) {
    file_ << formatted_msg << std::endl;
}

}  // namespace mini_spdlog
