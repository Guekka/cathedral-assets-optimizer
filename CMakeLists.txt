cmake_minimum_required(VERSION 3.18)
include_guard(GLOBAL)

project(
  Cathedral_Assets_Optimizer
  VERSION 7.0.0
  LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/BuildType.cmake)

add_subdirectory(src)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
  add_subdirectory(tests)
endif()
