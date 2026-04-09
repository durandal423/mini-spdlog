#ifndef ROTATING_FILE_SINK_H
#define ROTATING_FILE_SINK_H

#include <cstddef>
#include <fstream>
#include <mutex>
#include <string>

#include "mini_spdlog/sink/sink.h"

namespace mini_spdlog {

class rotating_file_sink : public sink {
public:
    rotating_file_sink(std::string filename, std::size_t max_size, std::size_t max_files);

    void log(const std::string& formatted_msg) override;
    void flush() override;

private:
    std::string filename_;
    std::size_t max_size_;
    std::size_t max_files_;
    std::size_t current_size_;
    std::ofstream file_;
    std::mutex mutex_;

    void open_file(bool truncate = false);
    void rotate();
};

}  // namespace mini_spdlog

#endif  // ROTATING_FILE_SINK_H
