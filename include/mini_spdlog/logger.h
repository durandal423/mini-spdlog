#ifndef LOGGER_H
#define LOGGER_H

#include <format>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "mini_spdlog/formatter/formatter.h"
#include "mini_spdlog/formatter/simple_formatter.h"
#include "mini_spdlog/formatter/pattern_formatter.h"
#include "mini_spdlog/level.h"
#include "mini_spdlog/sink/sink.h"

namespace mini_spdlog {

class logger {
public:
    logger(level min_level = level::info)
        : min_level_(min_level)
        , formatter_(std::make_unique<simple_formatter>()) {}

    template <typename Sink, typename... Args>
    void add_sink(Args&&... args) {
        static_assert(std::is_base_of_v<sink, Sink>);
        sinks_.push_back(
            std::make_shared<Sink>(std::forward<Args>(args)...)
        );
    }

    void add_sink(std::shared_ptr<sink>);

    void clear_sinks();

    void log(level lvl, const std::string& msg);

    void set_level(level lvl);
    void set_pattern(std::string pattern);
    void set_formatter(std::unique_ptr<formatter> fmt);
    void set_formatter(const formatter& fmt);
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
    level min_level_;
    std::vector<std::shared_ptr<sink>> sinks_;
    std::unique_ptr<formatter> formatter_;
};

}  // namespace mini_spdlog


#endif  // LOGGER_H