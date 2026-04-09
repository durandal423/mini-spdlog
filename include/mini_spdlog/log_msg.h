#ifndef LOG_MSG_H
#define LOG_MSG_H

#include <chrono>
#include <string>
#include <thread>

#include "mini_spdlog/level.h"
#include "mini_spdlog/source_loc.h"

namespace mini_spdlog {

struct log_msg {
    level lvl;
    std::string payload;
    std::chrono::system_clock::time_point time;
    std::string logger_name;
    std::thread::id thread_id;
    source_loc loc;
};

}  // namespace mini_spdlog


#endif  // LOG_MSG_H
