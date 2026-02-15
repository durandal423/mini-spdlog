#ifndef STDOUT_SINK_H
#define STDOUT_SINK_H

#include <iostream>
#include <mutex>

#include "mini_spdlog/sink/sink.h"

namespace mini_spdlog {

class stdout_sink : public sink {
public:
    void log(const std::string& formatted_msg) override;
    
private:
    static std::mutex mutex_;
};

}  // namespace mini_spdlog


#endif  // STDOUT_SINK_H