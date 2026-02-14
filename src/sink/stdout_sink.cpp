#include "mini_spdlog/sink/stdout_sink.h"

namespace mini_spdlog {
    
void mini_spdlog::stdout_sink::log(const std::string& formatted_msg) {
    std::cout << formatted_msg << std::endl;
}
    
} // namespace mini_spdlog


