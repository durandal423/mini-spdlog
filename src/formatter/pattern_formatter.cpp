#include "mini_spdlog/formatter/pattern_formatter.h"

namespace mini_spdlog {

std::string pattern_formatter::format(const log_msg& msg) const  {
    std::string result;
    result.reserve(128);

    for (const auto& item : items_) {
        item->append(msg, result);
    }

    return result;
}

void mini_spdlog::pattern_formatter::compile_pattern(std::string pattern) {
    std::string literal;

    for (size_t i = 0; i < pattern.size(); ++i) {
        if (pattern[i] == '%' && i + 1 < pattern.size()) {
            if (!literal.empty()) {
                items_.push_back(std::make_unique<literal_item>(literal));
                literal.clear();
            }

            char flag = pattern[++i];

            switch (flag) {
                case 'v':
                    items_.push_back(std::make_unique<message_item>());
                    break;

                case 'l':
                    items_.push_back(std::make_unique<level_item>());
                    break;

                case 'Y':
                case 'm':
                case 'd':
                case 'H':
                case 'M':
                case 'S': {
                    std::string fmt = fmt::format("{{:%{}}}", flag);
                    items_.push_back(std::make_unique<time_item>(fmt));
                    break;
                }

                default:
                    literal += '%';
                    literal += flag;
                    break;
            }
        } else {
            literal += pattern[i];
        }
    }

    if (!literal.empty()) {
        items_.push_back(std::make_unique<literal_item>(literal));
    }
}

}  // namespace mini_spdlog
