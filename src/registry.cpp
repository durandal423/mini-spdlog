#include "mini_spdlog/registry.h"

namespace mini_spdlog {

registry::registry()  {
    default_logger_ = std::make_shared<logger>("default");
    default_logger_->add_sink(std::make_shared<stdout_sink>());
    loggers_["default"] = default_logger_;
}

registry& registry::instance() {
    static registry inst;
    return inst;
}

std::shared_ptr<logger> registry::get(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = loggers_.find(name);
    return it != loggers_.end() ? it->second : nullptr;
}

std::shared_ptr<logger> registry::get_default_logger() {
    std::lock_guard lock(mutex_);
    return default_logger_;
}

void registry::set_default_logger(std::shared_ptr<logger> logger) {
    std::lock_guard<std::mutex> lock(mutex_);
    default_logger_ = logger;
}

void registry::drop(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    loggers_.erase(name);
}

void registry::drop_all() {
    std::lock_guard<std::mutex> lock(mutex_);
    loggers_.clear();
}

void registry::register_logger(std::shared_ptr<logger> logger) {
    std::lock_guard<std::mutex> lock(mutex_);
    const auto& name = logger->get_name();

    if (auto it = loggers_.find(name); it != loggers_.end()) {
        throw std::runtime_error("logger already exists: " + name);
    }

    loggers_[name] = logger;
}

}  // namespace mini_spdlog
