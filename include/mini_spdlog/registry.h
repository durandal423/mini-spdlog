#ifndef REGISTRY_H
#define REGISTRY_H

#include <memory>
#include <mutex>
#include <unordered_map>

#include "mini_spdlog/logger.h"
#include "mini_spdlog/sink/stdout_sink.h"

namespace mini_spdlog {

class registry {
public:
    registry();
    static registry& instance();

    void register_logger(std::shared_ptr<logger> logger);
    std::shared_ptr<logger> get(const std::string& name);
    std::shared_ptr<logger> get_default_logger();
    void set_default_logger(std::shared_ptr<logger> logger);
    void drop(const std::string& name);
    void drop_all();

private:
    std::mutex mutex_;
    std::unordered_map<std::string, std::shared_ptr<logger>> loggers_;
    std::shared_ptr<logger> default_logger_;
};

}  // namespace mini_spdlog


#endif  // REGISTRY_H