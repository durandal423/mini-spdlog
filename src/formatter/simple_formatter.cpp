#include "mini_spdlog/formatter/simple_formatter.h"

namespace mini_spdlog {

std::string mini_spdlog::simple_formatter::format(const log_msg& msg) const {
    return std::format(
        "[{:%Y-%m-%d %H:%M:%S}] {} {}",
        msg.time,
        level_to_string(msg.lvl),
        msg.payload);
}
}  // namespace mini_spdlog
