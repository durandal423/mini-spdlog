#include "mini_spdlog/mini_spdlog.h"

int main() {
    mini_spdlog::logger log;

    log.add_sink<mini_spdlog::stdout_sink>();

    auto s = std::make_shared<mini_spdlog::file_sink>("../../log.txt");
    log.add_sink(s);

    log.info("Hello sink world!");
    log.error("Something went wrong.");

    log.set_pattern("[%H:%M:%S] [%l] %v");

    log.warn("Hello {}!", 42);

    return 0;
}
