cmake_minimum_required(VERSION 3.5..3.15)

option(copy_dll_to_binary_dir "Will copy libbsarch DLL to the binary directory. May not work if libbsarch is used as a subproject" ON)

# Project
project(libbsarch CXX)

# Add library to build.
add_library(libbsarch SHARED
    src/DDS.h
    src/libbsarch.h
    src/libbsarch.cpp
    src/libbsarch.def
    )

target_compile_features(libbsarch PRIVATE cxx_std_11)

# Preprocessor definitions
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(MSVC)
        target_compile_options(libbsarch PRIVATE /W3 /MDd /Od /EHsc)
    endif()
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        target_compile_options(libbsarch PRIVATE /W3 /Zi /EHsc)
    endif()
endif()

target_compile_definitions(libbsarch PRIVATE BSARCH_DLL_EXPORT)

############################################################
# libbsarch_OOP                                            #
############################################################

find_package(Qt5 OPTIONAL_COMPONENTS Core)

add_library(libbsarch_OOP STATIC
    src/libbsarch.h
    src/dds.h

    src/libbsarch.hpp

    src/base_types.cpp
    src/base_types.hpp

    src/bsa.cpp
    src/bsa.hpp

    src/bsa_saver.cpp
    src/bsa_saver.hpp

    src/bsa_entry_list.cpp
    src/bsa_entry_list.hpp

    src/transform_archive.cpp
    src/transform_archive.hpp

    src/utils/string_convert.cpp
    src/utils/string_convert.hpp
    )


## Copying the corresponding DLL ##

#Finding 32 or 64bits
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    # 64 bits
    set(lib_delphi "${CMAKE_CURRENT_SOURCE_DIR}/delphi/lib64")
    message("libbsarch: using 64bits DLL")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    # 32 bits
    set(lib_delphi "${CMAKE_CURRENT_SOURCE_DIR}/delphi/lib")
    message("libbsarch: using 32bits DLL")
endif()

if(copy_dll_to_binary_dir)
    #Finding debug or release
    if(CMAKE_BUILD_TYPE MATCHES DEBUG)
        set(libbsarch_dll_path "${lib_delphi}/libbsarchd.dll")
    else()
        set(libbsarch_dll_path "${lib_delphi}/libbsarch.dll")
    endif()

    #Actually copying the DLL
    add_custom_command(TARGET libbsarch POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${libbsarch_dll_path}
        ${CMAKE_BINARY_DIR}/libbsarch.dll)
    message("libbsarch: Copying the DLL to ${CMAKE_BINARY_DIR}/libbsarch.dll")
endif()

target_link_libraries(libbsarch_OOP PUBLIC libbsarch)
target_compile_features(libbsarch_OOP PRIVATE cxx_std_17)
target_compile_definitions(libbsarch_OOP PRIVATE _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)

# Tests
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_executable(libbsarch_tests
        tests/main.test.cpp

        tests/transform_archive.test.cpp
        tests/save_and_load.test.cpp

        tests/utils.hpp
        tests/utils.cpp)

    target_link_libraries(libbsarch_tests PUBLIC libbsarch_OOP)
    target_compile_features(libbsarch_tests PRIVATE cxx_std_17)
endif()
