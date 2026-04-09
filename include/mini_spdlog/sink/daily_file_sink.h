#ifndef DAILY_FILE_SINK_H
#define DAILY_FILE_SINK_H

#include <chrono>
#include <fstream>
#include <mutex>
#include <string>

#include "mini_spdlog/sink/sink.h"

namespace mini_spdlog {

class daily_file_sink : public sink {
public:
    daily_file_sink(std::string filename, int rotation_hour = 0, int rotation_minute = 0);

    void log(const std::string& formatted_msg) override;
    void flush() override;

private:
    std::string filename_;
    int rotation_hour_;
    int rotation_minute_;
    std::ofstream file_;
    std::chrono::system_clock::time_point next_rotation_;
    std::mutex mutex_;

    std::chrono::system_clock::time_point calc_next_rotation(std::chrono::system_clock::time_point now) const;
    std::string daily_filename(std::chrono::system_clock::time_point now) const;
    void open_for_time(std::chrono::system_clock::time_point now);
};

}  // namespace mini_spdlog

#endif  // DAILY_FILE_SINK_H
