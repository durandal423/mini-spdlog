#include "mini_spdlog/formatter/simple_formatter.h"

namespace mini_spdlog {
    
std::unique_ptr<formatter> simple_formatter::clone() const {
    return std::make_unique<simple_formatter>(*this);
}
    
} // namespace mini_spdlog
