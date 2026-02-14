#include "mini_spdlog/level.h"

namespace mini_spdlog {

std::string level_to_string(level lvl) {
    switch (lvl) {
        case level::trace:    return "[TRACE]";
        case level::debug:    return "[DEBUG]";
        case level::info:     return "[INFO]";
        case level::warn:     return "[WARN]";
        case level::error:    return "[ERROR]";
        case level::critical: return "[CRITICAL]";
        case level::off:      return "[OFF]";
        default: return "[UNKNOWN]";
    }
}

}  // namespace mini_spdlog
