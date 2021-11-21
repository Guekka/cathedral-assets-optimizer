cmake_minimum_required(VERSION 3.12...3.20)

project(Cathedral_Assets_Optimizer VERSION 5.3.6 LANGUAGES CXX)

#Setting version for C++ files
configure_file (
    "${PROJECT_SOURCE_DIR}/Version.h.in"
    "${PROJECT_SOURCE_DIR}/Version.h"
    )

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#Qt
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
#Qt Linguist
find_package(Qt5LinguistTools REQUIRED)

#Setting GUI
#Comment out the next lines to create a CLI version instead (untested)
add_compile_definitions(GUI)
set(GUI true)

#SOURCES

set(SOURCES
    AnimationsOptimizer.cpp
    AnimationsOptimizer.h
    BSAOptimizer.cpp
    BSAOptimizer.h
    FilesystemOperations.cpp
    FilesystemOperations.h
    Logger.h
    MainOptimizer.cpp
    MainOptimizer.h
    Manager.cpp
    Manager.h
    MeshesOptimizer.cpp
    MeshesOptimizer.h
    OptionsCAO.cpp
    OptionsCAO.h
    PluginsOperations.cpp
    PluginsOperations.h
    Profiles.cpp
    Profiles.h
    TexturesFormats.h
    TexturesOptimizer.cpp
    TexturesOptimizer.h
    main.cpp
    pch.h
    )

#Ui
if(GUI)
    list (APPEND SOURCES
        MainWindow.cpp
        MainWindow.h
        MainWindow.ui
        TexturesFormatSelectDialog.cpp
        TexturesFormatSelectDialog.h
        TexturesFormatSelectDialog.ui)
endif()

#Icon and qrc
set(RESOURCES
    "${PROJECT_SOURCE_DIR}/Cathedral_Assets_Optimizer.rc"
    "${PROJECT_SOURCE_DIR}/Cathedral_Assets_Optimizer.manifest"
    "${PROJECT_SOURCE_DIR}/styles/qdarkstyle/style.qrc"
    )

#Generate translations
set(TS_FILES
    translations/AssetsOpt_de.ts
    translations/AssetsOpt_en.ts
    translations/AssetsOpt_es.ts
    translations/AssetsOpt_fr_FR.ts
    translations/AssetsOpt_ja.ts
    translations/AssetsOpt_ko_KR.ts
    translations/AssetsOpt_nl.ts
    translations/AssetsOpt_tr.ts
    translations/AssetsOpt_zh_CN.ts
    )

message("Updating translations files")
qt5_create_translation(QM_FILES ${TS_FILES} ${SOURCES})

#plog
message("Adding Plog library")
find_path(PLOG_INCLUDE_DIRS "plog/Appenders/AndroidAppender.h")
add_compile_definitions(PLOG_OMIT_PLOG_DEFINES)
#LibBsarch
find_package(bethutil-bsa CONFIG REQUIRED)
#Nif Library
find_package(nifly CONFIG REQUIRED)
# DirectXTex
find_package(directxtex CONFIG REQUIRED)
find_package(directx-headers CONFIG REQUIRED)
find_package(directxmath CONFIG REQUIRED)

# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
#Ressources
set(CMAKE_AUTORCC ON)


#Build
message("Adding CAO.exe")
add_executable(Cathedral_Assets_Optimizer WIN32 ${SOURCES} ${RESOURCES} ${QM_FILES})
#Windows 7 compatibility
target_link_libraries(Cathedral_Assets_Optimizer PRIVATE
    Qt5::Core Qt5::Widgets
    nifly Microsoft::DirectXTex bethutil-bsa::bethutil-bsa)
target_include_directories(Cathedral_Assets_Optimizer PRIVATE ${PLOG_INCLUDE_DIRS})

# PCH
target_precompile_headers(Cathedral_Assets_Optimizer PRIVATE "pch.h")

#C++17
target_compile_features(Cathedral_Assets_Optimizer PUBLIC cxx_std_20)

