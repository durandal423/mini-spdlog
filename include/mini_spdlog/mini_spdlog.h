#ifndef MINI_SPDLOG_H
#define MINI_SPDLOG_H

#include <cstddef>

#include "mini_spdlog/logger.h"
#include "mini_spdlog/registry.h"
#include "mini_spdlog/sink/file_sink.h"
#include "mini_spdlog/sink/sink.h"
#include "mini_spdlog/sink/stdout_sink.h"
#include "mini_spdlog/sink/async_sink.h"

namespace mini_spdlog {

inline std::shared_ptr<logger> get(const std::string& name) {
    return registry::instance().get(name);
}

inline std::shared_ptr<logger> default_logger() {
    return registry::instance().get_default_logger();
}

inline void register_logger(std::shared_ptr<logger> log) {
    registry::instance().register_logger(std::move(log));
}

inline void set_pattern(std::string pattern) {
    default_logger()->set_pattern(std::move(pattern));
}

inline void flush() {
    default_logger()->flush();
}

inline void enable_backtrace(std::size_t n_messages) {
    default_logger()->enable_backtrace(n_messages);
}

inline void disable_backtrace() {
    default_logger()->disable_backtrace();
}

inline void dump_backtrace(level lvl = level::info) {
    default_logger()->dump_backtrace(lvl);
}

inline void trace(const std::string& msg) {
    default_logger()->trace(msg);
}

inline void debug(const std::string& msg) {
    default_logger()->debug(msg);
}

inline void info(const std::string& msg) {
    default_logger()->info(msg);
}

inline void warn(const std::string& msg) {
    default_logger()->warn(msg);
}

inline void error(const std::string& msg) {
    default_logger()->error(msg);
}

inline void critical(const std::string& msg) {
    default_logger()->critical(msg);
}

inline void off(const std::string& msg) {
    default_logger()->off(msg);
}

template<typename... Args>
inline void trace(fmt::format_string<Args...> fmt, Args&&... args) {
    default_logger()->trace(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void debug(fmt::format_string<Args...> fmt, Args&&... args) {
    default_logger()->debug(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void info(fmt::format_string<Args...> fmt, Args&&... args) {
    default_logger()->info(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void warn(fmt::format_string<Args...> fmt, Args&&... args) {
    default_logger()->warn(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void error(fmt::format_string<Args...> fmt, Args&&... args) {
    default_logger()->error(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void critical(fmt::format_string<Args...> fmt, Args&&... args) {
    default_logger()->critical(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void off(fmt::format_string<Args...> fmt, Args&&... args) {
    default_logger()->off(fmt, std::forward<Args>(args)...);
}

}  // namespace mini_spdlog

#endif  // MINI_SPDLOG_H
