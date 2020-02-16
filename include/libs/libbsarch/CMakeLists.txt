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

#We only want the lib file. The DLL will be provided by Delphi
#Removing the generated DLL
add_custom_command(TARGET libbsarch POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove
    ${CMAKE_CURRENT_BINARY_DIR}/libbsarch.dll)

############################################################
# libbsarch_OOP                                            #
############################################################

find_package(Qt5 OPTIONAL_COMPONENTS Core)

add_library(libbsarch_OOP STATIC
    src/base_types.hpp
    src/bs_archive_auto.cpp
    src/bs_archive_auto.hpp
    src/bs_archive.cpp
    src/bs_archive.h
    src/bs_archive_entries.cpp
    src/bs_archive_entries.h
    src/dds.h

    src/libbsarch.hpp
    src/libbsarch.h
    src/dds.h

    src/utils/convertible_string.cpp
    src/utils/convertible_string.hpp
    src/utils/convertible_ostream.cpp
    src/utils/convertible_ostream.hpp
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

#Adding Qt support
if(Qt5_FOUND)
    message("libbsarch: Qt5 found, enabling Qt support")
    target_link_libraries(libbsarch_OOP PUBLIC Qt5::Core)
    target_compile_definitions(libbsarch PUBLIC "QT")
    target_compile_definitions(libbsarch_OOP PUBLIC "QT")
endif()

target_link_libraries(libbsarch_OOP PUBLIC libbsarch)
target_compile_features(libbsarch_OOP PRIVATE cxx_std_17)
target_compile_definitions(libbsarch_OOP PRIVATE _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
