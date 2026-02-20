#ifndef LOGGER_H
#define LOGGER_H

#include <format>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <mutex>
#include <atomic>

#include "mini_spdlog/formatter/formatter.h"
#include "mini_spdlog/formatter/simple_formatter.h"
#include "mini_spdlog/formatter/pattern_formatter.h"
#include "mini_spdlog/level.h"
#include "mini_spdlog/sink/sink.h"

namespace mini_spdlog {

class logger {
public:
    logger(std::string name, level min_level = level::info);

    const std::string& get_name() const;

    template <typename Sink, typename... Args>
    void add_sink(Args&&... args) {
        static_assert(std::is_base_of_v<sink, Sink>);
        auto old_sinks = sinks_.load();
        auto new_sinks = std::make_shared<std::vector<std::shared_ptr<sink>>>(*old_sinks);
        new_sinks->push_back(std::make_shared<Sink>(std::forward<Args>(args)...));
        std::atomic_store(&sinks_, new_sinks);
    }

    void add_sink(std::shared_ptr<sink>);

    void clear_sinks();

    void log(level lvl, const std::string& msg);

    void set_level(level lvl);
    void set_pattern(std::string pattern);
    void set_formatter(std::unique_ptr<formatter> fmt);
    void reset_formatter();

    void trace(const std::string& msg) { log(level::trace, msg); }
    void debug(const std::string& msg) { log(level::debug, msg); }
    void info(const std::string& msg) { log(level::info, msg); }
    void warn(const std::string& msg) { log(level::warn, msg); }
    void error(const std::string& msg) { log(level::error, msg); }
    void critical(const std::string& msg) { log(level::critical, msg); }
    void off(const std::string& msg) { log(level::off, msg); }

    template <typename... Args>
    void trace(std::format_string<Args...> fmt, Args&&... args) {
        log(level::trace, std::format(fmt, std::forward<Args>(args)...));
    }

    template <typename... Args>
    void debug(std::format_string<Args...> fmt, Args&&... args) {
        log(level::debug, std::format(fmt, std::forward<Args>(args)...));
    }

    template <typename... Args>
    void info(std::format_string<Args...> fmt, Args&&... args) {
        log(level::info, std::format(fmt, std::forward<Args>(args)...));
    }

    template <typename... Args>
    void warn(std::format_string<Args...> fmt, Args&&... args) {
        log(level::warn, std::format(fmt, std::forward<Args>(args)...));
    }

    template <typename... Args>
    void error(std::format_string<Args...> fmt, Args&&... args) {
        log(level::error, std::format(fmt, std::forward<Args>(args)...));
    }

    template <typename... Args>
    void critical(std::format_string<Args...> fmt, Args&&... args) {
        log(level::critical, std::format(fmt, std::forward<Args>(args)...));
    }

    template <typename... Args>
    void off(std::format_string<Args...> fmt, Args&&... args) {
        log(level::off, std::format(fmt, std::forward<Args>(args)...));
    }

private:
    std::string name_;
    std::atomic<level> min_level_;
    std::atomic<std::shared_ptr<const formatter>> formatter_;
    std::atomic<std::shared_ptr<std::vector<std::shared_ptr<sink>>>> sinks_;
};

}  // namespace mini_spdlog


#endif  // LOGGER_H