#include "mini_spdlog/logger.h"

#include <atomic>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace mini_spdlog {

static bool is_level_enabled(level lvl, level min_lvl) {
    return static_cast<int>(lvl) >= static_cast<int>(min_lvl);
}

logger::logger(std::string name, level min_level)
    : name_(std::move(name))
    , min_level_(min_level)
    , formatter_(std::make_shared<simple_formatter>())
    , sinks_(std::make_shared<std::vector<std::shared_ptr<sink>>>()) {}

const std::string& logger::get_name() const {
    return name_;
}

void logger::add_sink(std::shared_ptr<sink> new_sink) {
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
    if (!is_level_enabled(lvl, min_level_)) {
        return;
    }

    log_msg log {
        lvl,
        msg,
        std::chrono::system_clock::now()};

    auto fmt   = formatter_.load(std::memory_order_acquire);
    auto sinks = sinks_.load(std::memory_order_acquire);

    std::string formatted = fmt->format(log);

    for (auto& s : *sinks) {
        s->log(formatted);
    }
}

void logger::set_level(level lvl) {
    min_level_.store(lvl, std::memory_order_relaxed);
}

void logger::set_pattern(std::string pattern) {
    auto fmt = std::make_shared<pattern_formatter>(std::move(pattern));
    std::atomic_store(&formatter_, fmt);
}

void logger::set_formatter(std::unique_ptr<formatter> fmt) {
    std::atomic_store(&formatter_, std::move(fmt));
}

void logger::reset_formatter() {
    std::atomic_store(&formatter_, std::make_shared<simple_formatter>());
}

}  // namespace mini_spdlog
