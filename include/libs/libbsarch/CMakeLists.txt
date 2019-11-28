cmake_minimum_required(VERSION 3.5)

project(Qlibbsarch LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5 COMPONENTS Core REQUIRED)

add_library(Qlibbsarch STATIC
    BSArchive.cpp
    BSArchive.h
    BSArchiveAuto.cpp
    BSArchiveAuto.h
    BSArchiveEntries.cpp
    BSArchiveEntries.h
    dds.h
    libbsarch.h
    Qlibbsarch.hpp
)

add_library(
    libbsarch
    STATIC
    IMPORTED GLOBAL
    )

set_target_properties(
    libbsarch
    PROPERTIES IMPORTED_LOCATION
    ${CMAKE_CURRENT_SOURCE_DIR}/libbsarch.lib
    )

set_target_properties(
    libbsarch
    PROPERTIES IMPORTED_LOCATION_DEBUG
    ${CMAKE_CURRENT_SOURCE_DIR}/libbsarchd.lib
    )

target_link_libraries(Qlibbsarch PUBLIC Qt5::Core libbsarch)

target_compile_features(Qlibbsarch PUBLIC cxx_std_17)
