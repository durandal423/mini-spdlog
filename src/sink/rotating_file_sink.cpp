#include "mini_spdlog/sink/rotating_file_sink.h"

#include <filesystem>
#include <stdexcept>

namespace mini_spdlog {

rotating_file_sink::rotating_file_sink(std::string filename, std::size_t max_size, std::size_t max_files)
    : filename_(std::move(filename))
    , max_size_(max_size)
    , max_files_(max_files)
    , current_size_(0) {
    if (max_size_ == 0 || max_files_ == 0) {
        throw std::invalid_argument("rotating_file_sink requires max_size and max_files > 0");
    }

    open_file(false);
    if (std::filesystem::exists(filename_)) {
        current_size_ = std::filesystem::file_size(filename_);
    }
}

void rotating_file_sink::log(const std::string& formatted_msg) {
    std::lock_guard lock(mutex_);
    const auto write_size = formatted_msg.size() + 1;

    if (current_size_ + write_size > max_size_) {
        rotate();
    }

    file_ << formatted_msg << '\n';
    current_size_ += write_size;
}

void rotating_file_sink::flush() {
    std::lock_guard lock(mutex_);
    file_.flush();
}

void rotating_file_sink::open_file(bool truncate) {
    std::ios::openmode mode = std::ios::out;
    mode |= truncate ? std::ios::trunc : std::ios::app;
    file_.open(filename_, mode);

    if (!file_.is_open()) {
        throw std::runtime_error("failed to open log file: " + filename_);
    }
}

void rotating_file_sink::rotate() {
    file_.close();

    for (std::size_t i = max_files_; i > 0; --i) {
        const auto src = i == 1 ? filename_ : filename_ + "." + std::to_string(i - 1);
        const auto dst = filename_ + "." + std::to_string(i);

        if (std::filesystem::exists(src)) {
            std::error_code ec;
            std::filesystem::rename(src, dst, ec);
            if (ec) {
                std::filesystem::remove(dst, ec);
                std::filesystem::rename(src, dst, ec);
            }
        }
    }

    open_file(true);
    current_size_ = 0;
}

}  // namespace mini_spdlog
