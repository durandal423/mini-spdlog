#ifndef LEVEL_H
#define LEVEL_H

#include <string>

namespace mini_spdlog {

enum class level {
    trace,
    debug,
    info,
    warn,
    error,
    critical,
    off
};

std::string level_to_string(level lvl);

}  // namespace mini_spdlog


#endif  // LEVEL_H