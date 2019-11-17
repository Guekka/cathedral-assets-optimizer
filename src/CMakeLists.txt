cmake_minimum_required(VERSION 3.12...3.15)
include_guard(GLOBAL)

project(Cathedral_Assets_Optimizer VERSION 6.0.0 LANGUAGES CXX)

# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
#Ressources
set(CMAKE_AUTORCC ON)

#Setting version for C++ files
configure_file (
    "${PROJECT_SOURCE_DIR}/Version.hpp.in"
    "${PROJECT_SOURCE_DIR}/Version.hpp"
    )

#Include Cotire
set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
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
    AnimationsOptimizer.hpp

    BSA/BSAFile.cpp
    BSA/BSAFile.hpp
    BSA/BSAFolder.cpp
    BSA/BSAFolder.hpp

    BSA/Utils/BSA.cpp
    BSA/Utils/BSA.hpp
    BSA/Utils/BSACallback.hpp
    BSA/Utils/BSASplit.cpp
    BSA/Utils/BSASplit.hpp

    BSA/Commands/BSACreate.cpp
    BSA/Commands/BSACreate.hpp
    BSA/Commands/BSAExtract.cpp
    BSA/Commands/BSAExtract.hpp

    FilesystemOperations.cpp
    FilesystemOperations.hpp

    Commands/Command.cpp
    Commands/Command.hpp
    Commands/CommandBook.cpp
    Commands/CommandBook.hpp
    Commands/CommandResult.cpp
    Commands/CommandResult.hpp

    File/Resources.hpp
    File/File.cpp
    File/File.hpp

    Logger.hpp
    MainOptimizer.cpp
    MainOptimizer.hpp
    Manager.cpp
    Manager.hpp

    Meshes/Mesh.hpp

    Meshes/MeshFile.cpp
    Meshes/MeshFile.hpp
    Meshes/Commands/MeshConvert.cpp
    Meshes/Commands/MeshConvert.hpp
    Meshes/Commands/MeshRenameReferencedTextures.cpp
    Meshes/Commands/MeshRenameReferencedTextures.hpp

    OptionsCAO.cpp
    OptionsCAO.hpp

    Plugins/PluginsOperations.cpp
    Plugins/PluginsOperations.hpp

    Profiles.cpp
    Profiles.hpp

    Textures/Texture.hpp

    Textures/TextureCompressionDevice.cpp
    Textures/TextureCompressionDevice.hpp
    Textures/TextureFile.cpp
    Textures/TextureFile.hpp
    Textures/TextureFormats.hpp

    Textures/Commands/TextureConvert.cpp
    Textures/Commands/TextureConvert.hpp
    Textures/Commands/TextureDecompress.cpp
    Textures/Commands/TextureDecompress.hpp
    Textures/Commands/TextureGenerateMipmaps.cpp
    Textures/Commands/TextureGenerateMipmaps.hpp
    Textures/Commands/TextureResize.cpp
    Textures/Commands/TextureResize.hpp

    pch.hpp
    )

#Ui
if(GUI)
    list (APPEND SOURCES
        GUI/MainWindow.cpp
        GUI/MainWindow.hpp
        GUI/MainWindow.ui
        GUI/TexturesFormatSelectDialog.cpp
        GUI/TexturesFormatSelectDialog.hpp
        GUI/TexturesFormatSelectDialog.ui)
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
#Json library
message("Adding json library")
add_subdirectory("libs/json")

#Build
message("Adding CAO.exe")
add_library(Cathedral_Assets_Optimizer_lib ${SOURCES})
target_link_libraries(Cathedral_Assets_Optimizer_lib PUBLIC
    Qt5::Core Qt5::Widgets
    hkxcmd plog nif directxtex Qlibbsarch nlohmann_json::nlohmann_json)

add_executable(Cathedral_Assets_Optimizer WIN32 main.cpp
    ${ICON_RC_PATH} ${QRC_PATH} ${QM_FILES})

target_link_libraries(Cathedral_Assets_Optimizer PRIVATE Cathedral_Assets_Optimizer_lib)
target_include_directories(Cathedral_Assets_Optimizer PRIVATE "${CMAKE_BINARY_DIR}/Cathedral_Assets_Optimizer_lib_autogen/include")

add_executable(Cathedral_Assets_Optimizer_tests
    tests/tests.cpp
    tests/doctest.h
    )
target_include_directories(Cathedral_Assets_Optimizer_tests PRIVATE "${CMAKE_BINARY_DIR}/Cathedral_Assets_Optimizer_lib_autogen/include")
target_link_libraries(Cathedral_Assets_Optimizer_tests PRIVATE Cathedral_Assets_Optimizer_lib)
#Cotire (PCH)
set_target_properties(Cathedral_Assets_Optimizer PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "pch.hpp")
cotire(Cathedral_Assets_Optimizer)

#C++17
target_compile_features(Cathedral_Assets_Optimizer PUBLIC cxx_std_17)
