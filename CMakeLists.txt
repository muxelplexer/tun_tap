cmake_minimum_required(VERSION 3.29)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
project(tun_tap
        VERSION 0.0.1)

add_subdirectory(ext/mhl)
add_subdirectory(ext/dotenv)

add_executable(tun_tap
    src/main.cpp
    src/tap_dev.cpp
    src/ether/frame.cpp
)

target_include_directories(tun_tap PRIVATE
    inc/
)

target_link_libraries(tun_tap PRIVATE
    mhl
    dotenv
)

find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}")
set_target_properties(tun_tap PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")

target_compile_options(tun_tap PRIVATE
    -Wall -Wextra -Wpedantic -Wshadow -Weffc++ -Wconversion -Wsign-conversion -fms-extensions
)


set_property(TARGET tun_tap PROPERTY CXX_STANDARD 23)
set_property(TARGET tun_tap PROPERTY CXX_EXTENSIONS FALSE)
