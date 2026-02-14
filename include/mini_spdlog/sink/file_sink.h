#ifndef FILE_SINK_H
#define FILE_SINK_H

#include <fstream>

#include "mini_spdlog/sink/sink.h"

namespace mini_spdlog {

class file_sink : public sink {
public:
    explicit file_sink(const std::string& filename)
        : file_(filename, std::ios::app) {}

    void log(const std::string& formatted_msg) override;

private:
    std::ofstream file_;
};

}  // namespace mini_spdlog


#endif  // FILE_SINK_H