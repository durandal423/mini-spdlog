#ifndef SOURCE_LOC_H
#define SOURCE_LOC_H

namespace mini_spdlog {

struct source_loc {
    const char* filename;
    int line;
    const char* funcname;

    constexpr source_loc(const char* file = "", int line_num = 0, const char* func = "")
        : filename(file)
        , line(line_num)
        , funcname(func) {}

    [[nodiscard]] constexpr bool empty() const {
        return line == 0;
    }
};

}  // namespace mini_spdlog

#endif  // SOURCE_LOC_H
