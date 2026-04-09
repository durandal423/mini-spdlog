#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <regex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "mini_spdlog/mini_spdlog.h"

namespace {

class memory_sink final : public mini_spdlog::sink {
public:
    void log(const std::string& formatted_msg) override {
        std::lock_guard lock(mutex_);
        messages_.push_back(formatted_msg);
    }

    void flush() override {}

    std::vector<std::string> snapshot() const {
        std::lock_guard lock(mutex_);
        return messages_;
    }

private:
    mutable std::mutex mutex_;
    std::vector<std::string> messages_;
};

class counting_sink final : public mini_spdlog::sink {
public:
    explicit counting_sink(std::chrono::milliseconds delay = std::chrono::milliseconds {0})
        : delay_(delay)
        , count_(0) {}

    void log(const std::string&) override {
        if (delay_.count() > 0) {
            std::this_thread::sleep_for(delay_);
        }
        count_.fetch_add(1, std::memory_order_relaxed);
    }

    int count() const {
        return count_.load(std::memory_order_relaxed);
    }

private:
    std::chrono::milliseconds delay_;
    std::atomic<int> count_;
};

class prefix_formatter final : public mini_spdlog::formatter {
public:
    std::string format(const mini_spdlog::log_msg& msg) const override {
        return std::string("CUSTOM:") + msg.payload;
    }
};

bool check(bool condition, const std::string& msg, int& failures) {
    if (!condition) {
        std::cerr << "[FAIL] " << msg << "\n";
        ++failures;
        return false;
    }

    std::cout << "[PASS] " << msg << "\n";
    return true;
}

std::string today_str() {
    const auto now = std::chrono::system_clock::now();
    const auto tt = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&tt);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d");
    return oss.str();
}

bool test_pattern_and_source_loc(int& failures) {
    auto sink = std::make_shared<memory_sink>();
    mini_spdlog::logger log("blackbox-pattern");
    log.add_sink(sink);
    log.set_pattern("[%n][%l][tid:%t][%s:%# %!] %v.%e.%f");

    mini_spdlog::source_loc loc {"blackbox_stress_test.cpp", 321, "test_pattern_and_source_loc"};
    log.info(loc, "hello {}", 7);

    const auto msgs = sink->snapshot();
    if (!check(msgs.size() == 1, "pattern/source_loc produces one message", failures)) {
        return false;
    }

    const auto& line = msgs.front();
    check(line.find("[blackbox-pattern]") != std::string::npos, "contains %n logger name", failures);
    check(line.find("[INFO]") != std::string::npos, "contains %l level", failures);
    check(line.find("blackbox_stress_test.cpp:321 test_pattern_and_source_loc") != std::string::npos, "contains %s/%#/%! source location", failures);
    check(line.find("hello 7") != std::string::npos, "contains formatted payload", failures);

    const std::regex fractional_re(".*\\.[0-9]{3}\\.[0-9]{6}$");
    check(std::regex_match(line, fractional_re), "contains %e/%f fractional timestamp", failures);

    return true;
}

bool test_backtrace_dump(int& failures) {
    auto sink = std::make_shared<memory_sink>();
    mini_spdlog::logger log("blackbox-backtrace");
    log.add_sink(sink);
    log.set_pattern("%v");
    log.enable_backtrace(3);

    log.info("A");
    log.info("B");
    log.info("C");
    log.info("D");
    log.dump_backtrace(mini_spdlog::level::info);

    const auto msgs = sink->snapshot();
    bool has_start = false;
    bool has_end = false;
    bool has_b = false;
    bool has_c = false;
    bool has_d = false;

    for (const auto& m : msgs) {
        has_start = has_start || m.find("Backtrace Start") != std::string::npos;
        has_end = has_end || m.find("Backtrace End") != std::string::npos;
        has_b = has_b || m == "B";
        has_c = has_c || m == "C";
        has_d = has_d || m == "D";
    }

    check(has_start, "backtrace dump has start marker", failures);
    check(has_end, "backtrace dump has end marker", failures);
    check(has_b && has_c && has_d, "backtrace dump includes recent messages", failures);
    return true;
}

bool test_level_filter_and_backtrace_gate(int& failures) {
    auto sink = std::make_shared<memory_sink>();
    mini_spdlog::logger log("blackbox-level-gate");
    log.add_sink(sink);
    log.set_pattern("%v");
    log.set_level(mini_spdlog::level::warn);
    log.enable_backtrace(2);

    log.info("filtered-info");
    log.warn("visible-warn");
    log.error("visible-error");

    log.dump_backtrace(mini_spdlog::level::info);
    log.dump_backtrace(mini_spdlog::level::error);

    const auto msgs = sink->snapshot();
    bool has_filtered_info = false;
    bool has_visible_warn = false;
    bool has_visible_error = false;
    int start_marker_count = 0;

    for (const auto& m : msgs) {
        has_filtered_info = has_filtered_info || m == "filtered-info";
        has_visible_warn = has_visible_warn || m == "visible-warn";
        has_visible_error = has_visible_error || m == "visible-error";
        if (m.find("Backtrace Start") != std::string::npos) {
            ++start_marker_count;
        }
    }

    check(!has_filtered_info, "set_level filters lower-level messages", failures);
    check(has_visible_warn && has_visible_error, "set_level keeps warn/error messages", failures);
    check(start_marker_count == 1, "dump_backtrace obeys dump level gate", failures);
    return true;
}

bool test_null_inputs_and_formatter_reset(int& failures) {
    auto sink = std::make_shared<memory_sink>();
    mini_spdlog::logger log("blackbox-null-inputs");
    log.add_sink(nullptr);
    log.add_sink(sink);
    log.set_pattern("%v");

    log.set_formatter(nullptr);
    log.info("default-formatter-still-works");

    log.set_formatter(std::make_unique<prefix_formatter>());
    log.info("custom-format");

    log.clear_sinks();
    log.info("dropped-by-clear-sinks");

    const auto msgs = sink->snapshot();
    bool has_default_msg = false;
    bool has_custom_msg = false;
    bool has_after_clear = false;

    for (const auto& m : msgs) {
        has_default_msg = has_default_msg || m == "default-formatter-still-works";
        has_custom_msg = has_custom_msg || m == "CUSTOM:custom-format";
        has_after_clear = has_after_clear || m == "dropped-by-clear-sinks";
    }

    check(has_default_msg, "set_formatter(nullptr) keeps previous formatter", failures);
    check(has_custom_msg, "set_formatter(unique_ptr) swaps formatter", failures);
    check(!has_after_clear, "clear_sinks stops further delivery", failures);
    return true;
}

bool test_rotating_sink(const std::filesystem::path& dir, int& failures) {
    std::filesystem::create_directories(dir);
    const auto base = (dir / "rotating.log").string();
    std::error_code ec;
    std::filesystem::remove(base, ec);
    std::filesystem::remove(base + ".1", ec);
    std::filesystem::remove(base + ".2", ec);

    mini_spdlog::logger log("blackbox-rotating");
    auto rotating = std::make_shared<mini_spdlog::rotating_file_sink>(base, 128, 2);
    log.add_sink(rotating);
    log.set_pattern("%v");

    for (int i = 0; i < 50; ++i) {
        log.info("rotate-line-{:03d}-xxxxxxxxxx", i);
    }
    log.flush();

    check(std::filesystem::exists(base), "rotating sink keeps active file", failures);
    check(std::filesystem::exists(base + ".1"), "rotating sink creates rolled file", failures);
    check(std::filesystem::exists(base + ".2"), "rotating sink respects max backup depth", failures);
    return true;
}

bool test_daily_sink(const std::filesystem::path& dir, int& failures) {
    std::filesystem::create_directories(dir);
    const auto base = (dir / "daily_test").string();

    mini_spdlog::logger log("blackbox-daily");
    auto daily = std::make_shared<mini_spdlog::daily_file_sink>(base, 0, 0);
    log.add_sink(daily);
    log.set_pattern("%v");
    log.info("daily-message");
    log.flush();

    const auto expected = base + "_" + today_str() + ".log";
    check(std::filesystem::exists(expected), "daily sink creates today's file", failures);

    std::ifstream in(expected);
    std::string content;
    std::getline(in, content);
    check(content.find("daily-message") != std::string::npos, "daily sink writes message", failures);

    bool threw = false;
    try {
        auto invalid_sink = std::make_shared<mini_spdlog::daily_file_sink>(base + "_invalid", 24, 0);
        (void)invalid_sink;
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    check(threw, "daily sink rejects invalid rotation hour", failures);
    return true;
}

bool test_async_pool_policies(int& failures) {
    {
        auto sink = std::make_shared<counting_sink>();
        auto pool = std::make_shared<mini_spdlog::async_thread_pool>(64, 2);
        auto async = std::make_shared<mini_spdlog::async_sink>(sink, pool, mini_spdlog::async_overflow_policy::block);

        mini_spdlog::logger log("blackbox-async-block");
        log.add_sink(async);

        for (int i = 0; i < 300; ++i) {
            log.info("msg-{}", i);
        }
        log.flush();

        check(sink->count() == 300, "async block policy keeps all messages", failures);
    }

    {
        auto sink = std::make_shared<counting_sink>(std::chrono::milliseconds {2});
        auto pool = std::make_shared<mini_spdlog::async_thread_pool>(16, 1);
        auto async = std::make_shared<mini_spdlog::async_sink>(sink, pool, mini_spdlog::async_overflow_policy::overrun_oldest);

        mini_spdlog::logger log("blackbox-async-overrun");
        log.add_sink(async);

        for (int i = 0; i < 500; ++i) {
            log.info("msg-{}", i);
        }
        log.flush();

        const auto cnt = sink->count();
        check(cnt > 0, "async overrun policy still processes some messages", failures);
        check(cnt < 500, "async overrun policy drops old messages under pressure", failures);
    }

    return true;
}

bool test_async_shutdown_drain(int& failures) {
    auto sink = std::make_shared<counting_sink>();

    {
        auto pool = std::make_shared<mini_spdlog::async_thread_pool>(256, 2);
        auto async = std::make_shared<mini_spdlog::async_sink>(sink, pool, mini_spdlog::async_overflow_policy::block);

        mini_spdlog::logger log("blackbox-async-shutdown");
        log.add_sink(async);

        for (int i = 0; i < 120; ++i) {
            log.info("shutdown-msg-{}", i);
        }
        // 不手动 flush，验证线程池析构时会处理完队列
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    check(sink->count() == 120, "async thread pool drains queued logs on shutdown", failures);
    return true;
}

}  // namespace

int main() {
    int failures = 0;
    const std::filesystem::path out_dir = std::filesystem::temp_directory_path() / "mini_spdlog_blackbox";

    test_pattern_and_source_loc(failures);
    test_backtrace_dump(failures);
    test_level_filter_and_backtrace_gate(failures);
    test_null_inputs_and_formatter_reset(failures);
    test_rotating_sink(out_dir, failures);
    test_daily_sink(out_dir, failures);
    test_async_pool_policies(failures);
    test_async_shutdown_drain(failures);

    if (failures > 0) {
        std::cerr << "\nBlackbox stress test finished with " << failures << " failure(s).\n";
        return 1;
    }

    std::cout << "\nBlackbox stress test finished successfully.\n";
    return 0;
}
