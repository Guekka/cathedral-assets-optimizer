# DirectX Texture Library
#
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.
#
# http://go.microsoft.com/fwlink/?LinkId=248926

cmake_minimum_required (VERSION 3.11)
project (DirectXTex LANGUAGES CXX)

add_library (directxtex STATIC
    BC.h
    BCDirectCompute.h
    DDS.h
    DirectXTex.h
    DirectXTexP.h
    filters.h
    scoped.h
    BC.cpp
    BC4BC5.cpp
    BC6HBC7.cpp
    BCDirectCompute.cpp
    DirectXTexCompress.cpp
    DirectXTexCompressGPU.cpp
    DirectXTexConvert.cpp
    DirectXTexD3D11.cpp
    DirectXTexDDS.cpp
    DirectXTexFlipRotate.cpp
    DirectXTexHDR.cpp
    DirectXTexImage.cpp
    DirectXTexMipmaps.cpp
    DirectXTexMisc.cpp
    DirectXTexNormalMaps.cpp
    DirectXTexPMAlpha.cpp
    DirectXTexResize.cpp
    DirectXTexTGA.cpp
    DirectXTexUtil.cpp
    DirectXTexWIC.cpp
    )

target_compile_features(directxtex PRIVATE cxx_std_14)

if(MSVC)
    target_compile_options(directxtex PRIVATE /permissive- /JMC- /Zc:__cplusplus /fp:fast)

    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.26)
        target_compile_options(directxtex PRIVATE /Zc:preprocessor /wd5105)
    endif()


    if (${CMAKE_SIZEOF_VOID_P} EQUAL "4")
        target_compile_options(directxtex PRIVATE /arch:SSE2)
    endif()
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options( directxtex PRIVATE "-Wpedantic" "-Wextra" "-ffast-math")
    if (${CMAKE_SIZEOF_VOID_P} EQUAL "4")
        target_compile_options( directxtex PRIVATE -arch:SSE2)
    endif()
endif()

target_compile_definitions(directxtex PRIVATE _UNICODE UNICODE _WIN32_WINNT=_WIN32_WINNT_WIN7)
