#include "mini_spdlog/logger.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace mini_spdlog {

static bool is_level_enabled(level lvl, level min_lvl) {
    return static_cast<int>(lvl) >= static_cast<int>(min_lvl);
}

void logger::add_sink(std::shared_ptr<sink> sink) {
    sinks_.push_back(sink);
}

void logger::clear_sinks() {
    sinks_.clear();
}

void logger::log(level lvl, const std::string& msg) {

    if (!is_level_enabled(lvl, min_level_)) {
        return;
    }

    log_msg log{
        lvl,
        msg,
        std::chrono::system_clock::now()
    };

    std::string formatted = formatter_->format(log);

    for (auto& s : sinks_) {
        s->log(formatted);
    }
}

void logger::set_level(level lvl) { min_level_ = lvl; }

void logger::set_pattern(std::string pattern) {
    formatter_ = std::make_unique<pattern_formatter>(std::move(pattern));
}

void logger::set_formatter(std::unique_ptr<formatter> fmt) {
    formatter_ = std::move(fmt);
}

void logger::set_formatter(const formatter& fmt) {
    formatter_ = fmt.clone();
}

void logger::reset_formatter() {
    formatter_ = std::make_unique<simple_formatter>();
}

}  // namespace mini_spdlog
