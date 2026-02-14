#ifndef SINK_H
#define SINK_H

#include <string>

namespace mini_spdlog {

class sink {
public:
    virtual ~sink() = default;

    virtual void log(const std::string& formatted_msg) = 0;
};

}  // namespace mini_spdlog


#endif  // SINK_H