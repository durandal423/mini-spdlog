#include "mini_spdlog/sink/daily_file_sink.h"

#include <ctime>
#include <stdexcept>

#include <fmt/chrono.h>
#include <fmt/format.h>

namespace mini_spdlog {

daily_file_sink::daily_file_sink(std::string filename, int rotation_hour, int rotation_minute)
    : filename_(std::move(filename))
    , rotation_hour_(rotation_hour)
    , rotation_minute_(rotation_minute)
    , next_rotation_(calc_next_rotation(std::chrono::system_clock::now())) {
    if (rotation_hour_ < 0 || rotation_hour_ > 23 || rotation_minute_ < 0 || rotation_minute_ > 59) {
        throw std::invalid_argument("daily_file_sink rotation time is invalid");
    }

    open_for_time(std::chrono::system_clock::now());
}

void daily_file_sink::log(const std::string& formatted_msg) {
    std::lock_guard lock(mutex_);

    const auto now = std::chrono::system_clock::now();
    if (now >= next_rotation_) {
        open_for_time(now);
        next_rotation_ = calc_next_rotation(now);
    }

    file_ << formatted_msg << '\n';
}

void daily_file_sink::flush() {
    std::lock_guard lock(mutex_);
    file_.flush();
}

std::chrono::system_clock::time_point daily_file_sink::calc_next_rotation(std::chrono::system_clock::time_point now) const {
    const auto now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&now_time_t);
    tm.tm_hour = rotation_hour_;
    tm.tm_min = rotation_minute_;
    tm.tm_sec = 0;

    auto rotation_tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    if (rotation_tp <= now) {
        rotation_tp += std::chrono::hours(24);
    }

    return rotation_tp;
}

std::string daily_file_sink::daily_filename(std::chrono::system_clock::time_point now) const {
    return fmt::format("{}_{}.log", filename_, fmt::format("{:%Y-%m-%d}", now));
}

void daily_file_sink::open_for_time(std::chrono::system_clock::time_point now) {
    const auto path = daily_filename(now);
    file_.close();
    file_.open(path, std::ios::out | std::ios::app);

    if (!file_.is_open()) {
        throw std::runtime_error("failed to open daily log file: " + path);
    }
}

}  // namespace mini_spdlog
