#include "mini_spdlog/logger.h"

#include <atomic>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <thread>

namespace mini_spdlog {

static bool is_level_enabled(level lvl, level min_lvl) {
    return static_cast<int>(lvl) >= static_cast<int>(min_lvl);
}

logger::logger(std::string name, level min_level)
    : name_(std::move(name))
    , min_level_(min_level)
    , formatter_(std::make_shared<simple_formatter>())
    , sinks_(std::make_shared<std::vector<std::shared_ptr<sink>>>())
    , backtrace_capacity_(0)
    , backtrace_enabled_(false) {}

const std::string& logger::get_name() const {
    return name_;
}

void logger::add_sink(std::shared_ptr<sink> new_sink) {
    if (!new_sink) {
        return;
    }

    auto old_sinks = sinks_.load();

    if (std::find(old_sinks->begin(), old_sinks->end(), new_sink) != old_sinks->end()) {
        return;
    }

    auto new_sinks = std::make_shared<std::vector<std::shared_ptr<sink>>>(*old_sinks);
    new_sinks->push_back(std::move(new_sink));
    std::atomic_store(&sinks_, new_sinks);
}

void logger::clear_sinks() {
    auto new_sinks = std::make_shared<std::vector<std::shared_ptr<sink>>>();
    std::atomic_store(&sinks_, new_sinks);
}

void logger::log(level lvl, const std::string& msg) {
    log(lvl, msg, {});
}

void logger::log(level lvl, const std::string& msg, source_loc loc) {
    if (!is_level_enabled(lvl, min_level_.load(std::memory_order_relaxed))) {
        return;
    }

    log_msg log {
        lvl,
        msg,
        std::chrono::system_clock::now(),
        name_,
        std::this_thread::get_id(),
        loc};

    auto fmt = formatter_.load(std::memory_order_acquire);
    std::string formatted = fmt->format(log);

    push_backtrace(formatted);
    log_to_sinks(formatted);
}

void logger::set_level(level lvl) {
    min_level_.store(lvl, std::memory_order_relaxed);
}

void logger::set_pattern(std::string pattern) {
    auto fmt = std::make_shared<pattern_formatter>(std::move(pattern));
    std::atomic_store(&formatter_, fmt);
}

void logger::set_formatter(std::unique_ptr<formatter> fmt) {
    if (!fmt) {
        return;
    }

    std::atomic_store(
        &formatter_,
        std::shared_ptr<const formatter>(std::move(fmt)));
}

void logger::reset_formatter() {
    std::atomic_store(&formatter_, std::make_shared<simple_formatter>());
}

void logger::flush() {
    auto sinks = sinks_.load(std::memory_order_acquire);
    for (auto& s : *sinks) {
        s->flush();
    }
}

void logger::enable_backtrace(std::size_t n_messages) {
    std::lock_guard lock(backtrace_mutex_);
    backtrace_enabled_ = n_messages > 0;
    backtrace_capacity_ = n_messages;
    backtrace_buffer_.clear();
}

void logger::disable_backtrace() {
    std::lock_guard lock(backtrace_mutex_);
    backtrace_enabled_ = false;
    backtrace_capacity_ = 0;
    backtrace_buffer_.clear();
}

void logger::dump_backtrace(level lvl) {
    if (!is_level_enabled(lvl, min_level_.load(std::memory_order_relaxed))) {
        return;
    }

    std::vector<std::string> snapshot;

    {
        std::lock_guard lock(backtrace_mutex_);
        snapshot.assign(backtrace_buffer_.begin(), backtrace_buffer_.end());
    }

    if (snapshot.empty()) {
        return;
    }

    log_to_sinks("*************** Backtrace Start ***************");
    for (const auto& line : snapshot) {
        log_to_sinks(line);
    }
    log_to_sinks("**************** Backtrace End ****************");
}

void logger::push_backtrace(std::string formatted_msg) {
    std::lock_guard lock(backtrace_mutex_);
    if (!backtrace_enabled_ || backtrace_capacity_ == 0) {
        return;
    }

    if (backtrace_buffer_.size() >= backtrace_capacity_) {
        backtrace_buffer_.pop_front();
    }

    backtrace_buffer_.push_back(std::move(formatted_msg));
}

void logger::log_to_sinks(const std::string& formatted_msg) {
    auto sinks = sinks_.load(std::memory_order_acquire);
    for (auto& s : *sinks) {
        s->log(formatted_msg);
    }
}

}  // namespace mini_spdlog
