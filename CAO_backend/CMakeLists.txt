cmake_minimum_required(VERSION 3.12)

project(Cathedral_Assets_Optimizer_back)

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
    libs/QLogger/QLogger.cpp
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
    libs/QLogger/QLogger.h
    )

#QLogger

include_directories(${PROJECT_SOURCE_DIR}/libs/QLogger)

#LibBsarch

include_directories(${PROJECT_SOURCE_DIR}/libs/libbsarch)
file(GLOB SOURCES_LIBBSARCH ${PROJECT_SOURCE_DIR}/libs/libbsarch/*.cpp)
file(GLOB HEADERS_LIBBSARCH ${PROJECT_SOURCE_DIR}/libs/libbsarch/*.h)

#Translations

execute_process(COMMAND cmd /C translations.bat )

#Build

add_executable(Cathedral_Assets_Optimizer_back ${SOURCES} ${SOURCES_LIBBSARCH} ${HEADERS} ${HEADERS_LIBBSARCH} ${QON_QM_FILES})
target_link_libraries(Cathedral_Assets_Optimizer_back Qt5::Core)
target_link_libraries(Cathedral_Assets_Optimizer_back ${PROJECT_SOURCE_DIR}/libs/libbsarch/libbsarch.lib)

