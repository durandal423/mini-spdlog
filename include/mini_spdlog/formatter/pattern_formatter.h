#ifndef PATTERN_FORMATTER_H
#define PATTERN_FORMATTER_H

#include <fmt/format.h>
#include <fmt/chrono.h>
#include <memory>
#include <vector>

#include "mini_spdlog/log_msg.h"
#include "mini_spdlog/formatter/formatter.h"

namespace mini_spdlog {

class format_item {
public:
    virtual ~format_item() = default;

    virtual void append(const log_msg& msg, std::string& out) const = 0;
};

class literal_item : public format_item {
public:
    explicit literal_item(std::string text)
        : text_(std::move(text)) {}

    void append(const log_msg&, std::string& out) const override {
        out += text_;
    }
private:
    std::string text_;
};

class level_item : public format_item {
public:
    void append(const log_msg& msg, std::string& out) const override {
        out += level_to_string(msg.lvl);
    }
};

class message_item : public format_item {
public:
    void append(const log_msg& msg, std::string& out) const override {
        out += msg.payload;
    }
};

class time_item : public format_item {
public:
    explicit time_item(std::string fmt)
        : fmt_(std::move(fmt)) {}

    void append(const log_msg& msg, std::string& out) const override {
        out += fmt::vformat(fmt_, fmt::make_format_args(msg.time));
    }
private:
    std::string fmt_;
};


class pattern_formatter : public formatter {
public:
    explicit pattern_formatter(std::string pattern) {
        compile_pattern(std::move(pattern));
    }

    std::string format(const log_msg& msg) const override;
private:
    std::vector<std::unique_ptr<format_item>> items_;

    void compile_pattern(std::string pattern);
};

}  // namespace mini_spdlog


#endif  // PATTERN_FORMATTER_H