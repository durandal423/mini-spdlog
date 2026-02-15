#ifndef SIMPLE_FORMATTER_H
#define SIMPLE_FORMATTER_H

#include <iomanip>
#include <sstream>

#include "mini_spdlog/formatter/formatter.h"

namespace mini_spdlog {

class simple_formatter : public formatter {
public:
    std::string format(const log_msg& msg) const override;
};

}  // namespace mini_spdlog


#endif  // SIMPLE_FORMATTER_H