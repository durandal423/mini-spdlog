#include "mini_spdlog/mini_spdlog.h"

int main() {
    mini_spdlog::info("Hello, {}", 42);
    mini_spdlog::info("Hello, {}", 42);
    mini_spdlog::info("Hello, {}", 42);

    return 0;
}
