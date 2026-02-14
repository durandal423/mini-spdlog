#include "mini_spdlog/mini_spdlog.h"

int main() {
    using namespace mini_spdlog;

    // ===== 1️⃣ 创建 logger =====
    logger log;

    // ===== 2️⃣ 添加两个 sink =====
    log.add_sink<stdout_sink>();

    auto file = std::make_shared<file_sink>("../../log.txt");
    // log.add_sink<file_sink>("../../log.txt");
    log.add_sink(file);

    // ===== 3️⃣ 基础输出测试 =====
    log.info("Logger initialized.");
    log.warn("Warning number: {}", 1);
    log.error("Error message: {}", "file not found");

    // ===== 4️⃣ Level 过滤测试 =====
    log.set_level(level::warn);

    log.info("This should NOT appear.");
    log.warn("This should appear.");
    log.error("This should also appear.");

    log.set_level(level::info);

    // ===== 5️⃣ Pattern formatter 测试 =====
    log.set_pattern("[%H:%M:%S] %l %v");

    log.info("Pattern formatting test {}", 123);
    log.error("Another error occurred: {}", 3.14);

    // ===== 6️⃣ Reset formatter 测试 =====
    log.reset_formatter();

    log.info("Back to simple formatter.");

    // ===== 7️⃣ 测试共享 sink =====
    logger log2;
    log2.add_sink(file);  // 共享同一个 file_sink

    log2.set_pattern("[SECOND LOGGER] %l %v");
    log2.warn("This should go to same file.");

    // ===== 8️⃣ 多参数格式测试 =====
    log2.error("Multiple params: {} {} {}", 1, 2.5, "test");

    return 0;
}
