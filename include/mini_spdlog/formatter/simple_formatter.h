#ifndef SIMPLE_FORMATTER_H
#define SIMPLE_FORMATTER_H

#include <iomanip>
#include <sstream>

#include "mini_spdlog/formatter/formatter.h"

namespace mini_spdlog {

class simple_formatter : public formatter {
public:
    std::string format(const log_msg& msg) override {
        return std::format(
            "[{:%Y-%m-%d %H:%M:%S}] {:<11} {}",
            msg.time,
            level_to_string(msg.lvl),
            msg.payload);
    }
    std::unique_ptr<formatter> clone() const;
};

}  // namespace mini_spdlog


#endif  // SIMPLE_FORMATTER_H