cmake_minimum_required(VERSION 3.12...3.15)

project(Cathedral_Assets_Optimizer VERSION 3.0.7 LANGUAGES CXX)

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
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
#Ressources
set(CMAKE_AUTORCC ON)


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
    main.cpp
    Manager.cpp
    TexturesOptimizer.cpp
    MeshesOptimizer.cpp
    BsaOptimizer.cpp
    FilesystemOperations.cpp
    PluginsOperations.cpp
    AnimationsOptimizer.cpp
    MainOptimizer.cpp
    Profiles.cpp
    OptionsCAO.cpp
    BSA.cpp
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
    pch.h
    Profiles.h
    Logger.h
    OptionsCAO.h
    TexturesFormats.h
    BSA.h
    #Ui
    )

#Ui
if(GUI)
    list (APPEND HEADERS
        MainWindow.h
        TexturesFormatSelectDialog.h)

    list (APPEND SOURCES
        MainWindow.cpp
        TexturesFormatSelectDialog.cpp)

    set(UI
        MainWindow.ui
        TexturesFormatSelectDialog.ui
        )
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
qt5_create_translation(QM_FILES ${TS_FILES} ${SOURCES} ${HEADERS} ${UI})
add_custom_target(Localization ALL DEPENDS ${QM_FILES})


#plog
message("Building Plog library")
add_subdirectory(libs/plog)
add_compile_definitions(PLOG_OMIT_PLOG_DEFINES)
#LibBsarch
message("Building Libbsarch library")
add_subdirectory(libs/libbsarch)
#hkxcmd
message("Building hkxcmd library")
add_subdirectory(libs/hkxcmd)
#Nif Library
message("Building nif library")
add_subdirectory(libs/NIF)
#DirectXTex library
message("Building DirectXTex library")
add_subdirectory("libs/DirectXTex")
#Windows 7 compatibility
add_compile_definitions(_WIN32_WINNT=_WIN32_WINNT_WIN7)

#Build
message("Building CAO.exe")
add_executable(Cathedral_Assets_Optimizer WIN32 ${SOURCES} ${HEADERS} ${QM_FILES} ${QON_QM_FILES} ${UI} ${ICON_RC_PATH} ${QRC_PATH})
target_link_libraries(Cathedral_Assets_Optimizer PRIVATE Qt5::Core Qt5::Widgets)
target_link_libraries(Cathedral_Assets_Optimizer PRIVATE hkxcmd plog nif directxtex qlibbsarch)

#Cotire (PCH)
set_target_properties(Cathedral_Assets_Optimizer PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "pch.h")
cotire(Cathedral_Assets_Optimizer)

#C++17
target_compile_features(Cathedral_Assets_Optimizer PUBLIC cxx_std_17)
