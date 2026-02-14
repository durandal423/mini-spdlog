#ifndef PATTERN_FORMATTER_H
#define PATTERN_FORMATTER_H

#include <format>
#include <memory>
#include <vector>

#include "mini_spdlog/log_msg.h"
#include "mini_spdlog/formatter/formatter.h"

namespace mini_spdlog {

class format_item {
public:
    virtual ~format_item() = default;

    virtual void append(const log_msg& msg, std::string& out) const = 0;

    virtual std::unique_ptr<format_item> clone() const = 0;
};

class literal_item : public format_item {
public:
    explicit literal_item(std::string text)
        : text_(std::move(text)) {}

    void append(const log_msg&, std::string& out) const override {
        out += text_;
    }

    std::unique_ptr<format_item> clone() const {
        return std::make_unique<literal_item>(*this);
    }

private:
    std::string text_;
};

class level_item : public format_item {
public:
    void append(const log_msg& msg, std::string& out) const override {
        out += level_to_string(msg.lvl);
    }

    std::unique_ptr<format_item> clone() const {
        return std::make_unique<level_item>(*this);
    }
};

class message_item : public format_item {
public:
    void append(const log_msg& msg, std::string& out) const override {
        out += msg.payload;
    }

    std::unique_ptr<format_item> clone() const {
        return std::make_unique<message_item>(*this);
    }
};

class time_item : public format_item {
public:
    explicit time_item(std::string fmt)
        : fmt_(std::move(fmt)) {}

    void append(const log_msg& msg, std::string& out) const override {
        out += std::vformat(fmt_, std::make_format_args(msg.time));
    }

    std::unique_ptr<format_item> clone() const {
        return std::make_unique<time_item>(*this);
    }

private:
    std::string fmt_;
};


class pattern_formatter : public formatter {
public:
    explicit pattern_formatter(std::string pattern) {
        compile_pattern(std::move(pattern));
    }

    pattern_formatter(const pattern_formatter& other) {
        for (const auto& item : other.items_) {
            items_.push_back(item->clone());
        }
    }

    std::string format(const log_msg& msg) override;

    std::unique_ptr<formatter> clone() const;

private:
    std::vector<std::unique_ptr<format_item>> items_;

    void compile_pattern(std::string pattern);
};

}  // namespace mini_spdlog


#endif  // PATTERN_FORMATTER_H