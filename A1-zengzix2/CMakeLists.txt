cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

project(
    csc369h1_processes
    VERSION 20241
    LANGUAGES C
)

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(Unicorn REQUIRED)

set(CSC369H1_PROCESSES_SANDBOX_DIR ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)
