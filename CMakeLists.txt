cmake_minimum_required(VERSION 3.18)
include_guard(GLOBAL)

project(
        Cathedral_Assets_Optimizer
        VERSION 6.0.0
        LANGUAGES CXX)

# Setting version for C++ files
configure_file(src/version.hpp.in src/version.hpp)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/Qt.cmake)
include(cmake/BuildType.cmake)

add_subdirectory(data)
add_subdirectory(src)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(tests)
endif ()
