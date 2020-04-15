cmake_minimum_required(VERSION 3.12...3.15)

project(Cathedral_Assets_Optimizer VERSION 5.0.10 LANGUAGES CXX)

#Setting version for C++ files
configure_file (
    "${PROJECT_SOURCE_DIR}/Version.h.in"
    "${PROJECT_SOURCE_DIR}/Version.h"
    )

#Include Cotire
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")
include(cotire)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#Qt
set(Qt5_DIR "C:/IT/Qt/5.12.4/msvc2017/lib/cmake/Qt5")
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
#Qt Linguist
set(QtBin "C:/IT/Qt/5.12.4/msvc2017/lib/cmake/Qt5LinguistTools")
find_package(Qt5LinguistTools)


#Setting GUI
#Comment out the next lines to create a CLI version instead (untested)
add_compile_definitions(GUI)
set(GUI true)

#SOURCES

set(SOURCES
    AnimationsOptimizer.cpp
    AnimationsOptimizer.h
    BSA.cpp
    BSA.h
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
set(ICON_RC_PATH "${PROJECT_SOURCE_DIR}/Cathedral_Assets_Optimizer.rc")
set(QRC_PATH "${PROJECT_SOURCE_DIR}/styles/qdarkstyle/style.qrc")

#Generate translations
set(TS_FILES
    translations/AssetsOpt_de.ts
    translations/AssetsOpt_en.ts
    translations/AssetsOpt_fr.ts
    translations/AssetsOpt_ja.ts
    )

message("Updating translations files")
qt5_create_translation(QM_FILES ${TS_FILES} ${SOURCES})

#plog
message("Adding Plog library")
add_subdirectory(libs/plog)
add_compile_definitions(PLOG_OMIT_PLOG_DEFINES)
#LibBsarch
message("Adding Libbsarch library")
add_subdirectory(libs/libbsarch)
#hkxcmd
message("Adding hkxcmd library")
add_subdirectory(libs/hkxcmd)
#Nif Library
message("Adding nif library")
add_subdirectory(libs/NIF)
#DirectXTex library
message("Adding DirectXTex library")
add_subdirectory("libs/DirectXTex")


# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
#Ressources
set(CMAKE_AUTORCC ON)


#Build
message("Adding CAO.exe")
add_executable(Cathedral_Assets_Optimizer WIN32 ${SOURCES} ${ICON_RC_PATH} ${QRC_PATH} ${QM_FILES})
#Windows 7 compatibility
target_link_libraries(Cathedral_Assets_Optimizer PRIVATE
    Qt5::Core Qt5::Widgets
    hkxcmd plog nif directxtex qlibbsarch)

#Cotire (PCH)
set_target_properties(Cathedral_Assets_Optimizer PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "pch.h")
cotire(Cathedral_Assets_Optimizer)

#C++17
target_compile_features(Cathedral_Assets_Optimizer PUBLIC cxx_std_17)

