#ifndef LOG_MSG_H
#define LOG_MSG_H

#include <chrono>
#include <string>

#include "mini_spdlog/level.h"

namespace mini_spdlog {

struct log_msg {
    level lvl;
    std::string payload;
    std::chrono::system_clock::time_point time;
};

}  // namespace mini_spdlog


#endif  // LOG_MSG_H