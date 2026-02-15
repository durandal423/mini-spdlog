#ifndef MINI_SPDLOG_H
#define MINI_SPDLOG_H

#include "mini_spdlog/logger.h"
#include "mini_spdlog/registry.h"
#include "mini_spdlog/sink/file_sink.h"
#include "mini_spdlog/sink/sink.h"
#include "mini_spdlog/sink/stdout_sink.h"

namespace mini_spdlog {

inline std::shared_ptr<logger> get(const std::string& name) {
    return registry::instance().get(name);
}

inline std::shared_ptr<logger> default_logger() {
    return registry::instance().get_default_logger();
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
inline void trace(std::format_string<Args...> fmt, Args&&... args) {
    default_logger()->trace(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void debug(std::format_string<Args...> fmt, Args&&... args) {
    default_logger()->debug(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void info(std::format_string<Args...> fmt, Args&&... args) {
    default_logger()->info(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void warn(std::format_string<Args...> fmt, Args&&... args) {
    default_logger()->warn(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void error(std::format_string<Args...> fmt, Args&&... args) {
    default_logger()->error(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void critical(std::format_string<Args...> fmt, Args&&... args) {
    default_logger()->critical(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void off(std::format_string<Args...> fmt, Args&&... args) {
    default_logger()->off(fmt, std::forward<Args>(args)...);
}

}  // namespace mini_spdlog

#endif  // MINI_SPDLOG_H