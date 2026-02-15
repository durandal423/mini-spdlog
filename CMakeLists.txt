cmake_minimum_required(VERSION 3.10)
project(mini_spdlog)

set(CMAKE_CXX_STANDARD 23)

add_library(mini_spdlog
    src/level.cpp
    src/logger.cpp
    src/registry.cpp
    src/sink/file_sink.cpp
    src/sink/stdout_sink.cpp
    src/sink/async_sink.cpp
    src/formatter/pattern_formatter.cpp
    src/formatter/simple_formatter.cpp
)

target_include_directories(mini_spdlog PUBLIC include)

add_executable(example src/main.cpp)
target_link_libraries(example PRIVATE mini_spdlog)