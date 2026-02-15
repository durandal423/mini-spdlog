#ifndef FORMATTER_H
#define FORMATTER_H

#include <string>

#include "mini_spdlog/log_msg.h"

namespace mini_spdlog {

class formatter {
public:
    virtual ~formatter() = default;

    virtual std::string format(const log_msg& msg) const = 0;
};

}  // namespace mini_spdlog


#endif  // FORMATTER_H