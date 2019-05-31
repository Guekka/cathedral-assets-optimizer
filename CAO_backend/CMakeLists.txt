cmake_minimum_required(VERSION 3.12)

project(Cathedral_Assets_Optimizer_back)

#Include Cotire

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")
include(cotire)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

#C++17

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#Qt
set(Qt5_DIR "E:/Qt/5.12.3/msvc2017/lib/cmake/Qt5")
find_package(Qt5 COMPONENTS Core REQUIRED)

#SOURCES

set(SOURCES
    main.cpp
    Manager.cpp
    TexturesOptimizer.cpp
    MeshesOptimizer.cpp
    BsaOptimizer.cpp
    FilesystemOperations.cpp
    PluginsOperations.cpp
    AnimationsOptimizer.cpp
    MainOptimizer.cpp
    IntegrationTests.cpp
    )


#HEADERS

set(HEADERS
    Manager.h
    TexturesOptimizer.h
    MeshesOptimizer.h
    BsaOptimizer.h
    FilesystemOperations.h
    PluginsOperations.h
    AnimationsOptimizer.h
    MainOptimizer.h
    IntegrationTests.h
    pch.h
    )

#QLogger

include_directories(${PROJECT_SOURCE_DIR}/libs/QLogger)
file(GLOB_RECURSE SOURCES_QLOGGER ${PROJECT_SOURCE_DIR}/libs/QLogger/*.cpp)
file(GLOB_RECURSE HEADERS_QLOGGER ${PROJECT_SOURCE_DIR}/libs/QLogger/*.h)

#LibBsarch

include_directories(${PROJECT_SOURCE_DIR}/libs/libbsarch)
file(GLOB_RECURSE SOURCES_LIBBSARCH ${PROJECT_SOURCE_DIR}/libs/libbsarch/*.cpp)
file(GLOB_RECURSE HEADERS_LIBBSARCH ${PROJECT_SOURCE_DIR}/libs/libbsarch/*.h)

#Havok

include_directories("${PROJECT_SOURCE_DIR}/libs/havok_2010_2_0/Source")

set ( HAVOK_LIBS_DIR_RELEASE
    ${PROJECT_SOURCE_DIR}/libs/havok_2010_2_0/Lib/win32_net_9-0/release_multithreaded)

set ( HAVOK_LIBS_DIR_DEBUG
    ${PROJECT_SOURCE_DIR}/libs/havok_2010_2_0/Lib/win32_net_9-0/debug_multithreaded)

set ( HAVOK_LIBS
    hkaAnimation
    hkaInternal
    hkaRagdoll
    hkBase
    hkCompat
    hkgBridge
    hkgCommon
    hkgDx10
    hkgDx9
    hkgDx9s
    hkGeometryUtilities
    hkgOgl
    hkgOglES
    hkgOglES2
    hkgOgls
    hkInternal
    hkpCollide
    hkpConstraintSolver
    hkpDynamics
    hkpInternal
    hkpUtilities
    hkpVehicle
    hkSceneData
    hksCommon
    hkSerialize
    hksXAudio2
    hkVisualize
    )

foreach(havok_lib IN LISTS HAVOK_LIBS)
    message( "[MAIN]: adding ${havok_lib}" )

    add_library(
        ${havok_lib}
        STATIC
        IMPORTED GLOBAL
        )

    set_target_properties(
        ${havok_lib}
        PROPERTIES IMPORTED_LOCATION
        "${HAVOK_LIBS_DIR_RELEASE}/${havok_lib}.lib"
        )

    set_target_properties(
        ${havok_lib}
        PROPERTIES IMPORTED_LOCATION_DEBUG
        "${HAVOK_LIBS_DIR_DEBUG}/${havok_lib}.lib"
        )

endforeach(havok_lib)

#hkxcmd

include_directories(${PROJECT_SOURCE_DIR}/libs/hkxcmd)
file(GLOB_RECURSE SOURCES_CK-CMD ${PROJECT_SOURCE_DIR}/libs/hkxcmd/*.cpp)
file(GLOB_RECURSE HEADERS_CK-CMD ${PROJECT_SOURCE_DIR}/libs/hkxcmd/*.h)

#2010 Behavior compat

set(HAVOK_COMPAT_INCLUDES_PATH
    "${PROJECT_SOURCE_DIR}/libs/havok_2010_2_0/compat"
    )

include_directories(${HAVOK_COMPAT_INCLUDES_PATH})

file(GLOB_RECURSE HAVOK_COMPAT_HEADERS
    "${HAVOK_COMPAT_INCLUDES_PATH}/*.h"
    "${HAVOK_COMPAT_INCLUDES_PATH}/*.inl"
    )

file(GLOB_RECURSE HAVOK_COMPAT_SRC
    "${HAVOK_COMPAT_INCLUDES_PATH}/*.cpp"
    )

#Nif Library

set(NIFLIB_INCLUDES_PATH
    "${PROJECT_SOURCE_DIR}/libs/NIF"
    )

include_directories(${NIFLIB_INCLUDES_PATH})

file(GLOB_RECURSE NIFLIB_HEADERS
    "${NIFLIB_INCLUDES_PATH}/*.h"
    )

file(GLOB_RECURSE NIFLIB_SRC
    "${NIFLIB_INCLUDES_PATH}/*.cpp"
    )

#Translations
#Broken
#execute_process(COMMAND cmd /C translations.bat )

#Merge all

list (APPEND SOURCES ${SOURCES_QLOGGER})
list (APPEND SOURCES ${SOURCES_LIBBSARCH})
list (APPEND SOURCES ${SOURCES_HAVOK})
list (APPEND SOURCES ${SOURCES_CK-CMD})
list (APPEND SOURCES ${HAVOK_COMPAT_SRC})
list (APPEND SOURCES ${NIFLIB_SRC})

list (APPEND HEADERS ${HEADERS_QLOGGER})
list (APPEND HEADERS ${HEADERS_LIBBSARCH})
list (APPEND HEADERS ${HEADERS_HAVOK})
list (APPEND HEADERS ${HEADERS_CK-CMD})
list (APPEND HEADERS ${HAVOK_COMPAT_HEADERS})
list (APPEND HEADERS ${NIFLIB_HEADERS})

list (APPEND LIBS Qt5::Core)
list (APPEND LIBS ${PROJECT_SOURCE_DIR}/libs/libbsarch/libbsarch.lib)
list (APPEND LIBS ${HAVOK_LIBS})
list (APPEND LIBS Shlwapi.lib legacy_stdio_definitions.lib)

source_group("Havok Compat\\Headers" FILES ${HAVOK_COMPAT_HEADERS})
source_group("Havok Compat\\Sources" FILES ${HAVOK_COMPAT_SRC})
source_group("Header Files" FILES ${HEADERS})

#MSVC

IF (MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj /D_SCL_SECURE_NO_WARNINGS=1 /D_CRT_NONSTDC_NO_DEPRECATE=1")
add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
# Update compiler flags.
IF (PROJECT_STATIC_RUNTIME)
FOREACH(flag
    CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_RELWITHDEBINFO
    CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_DEBUG_INIT
    CMAKE_CXX_FLAGS_RELEASE  CMAKE_CXX_FLAGS_RELWITHDEBINFO
    CMAKE_CXX_FLAGS_DEBUG  CMAKE_CXX_FLAGS_DEBUG_INIT)
STRING(REPLACE "/MD"  "/MT" "${flag}" "${${flag}}")
SET("${flag}" "${${flag}} /EHsc ")
ENDFOREACH()
ENDIF ()

ENDIF ()

#Build

add_executable(Cathedral_Assets_Optimizer_back ${SOURCES} ${HEADERS} ${QON_QM_FILES})
target_link_libraries(Cathedral_Assets_Optimizer_back ${LIBS})

#Cotire (PCH)
set_target_properties(Cathedral_Assets_Optimizer_back PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "pch.h")
cotire(Cathedral_Assets_Optimizer_back)
