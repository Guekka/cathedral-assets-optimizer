cmake_minimum_required(VERSION 3.12...3.15)
include_guard(GLOBAL)

project(Cathedral_Assets_Optimizer VERSION 6.0.0 LANGUAGES CXX)

#Set to false to create a CLI version instead (untested)
option(GUI "Create GUI version of CAO" true)

# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
#Ressources
set(CMAKE_AUTORCC ON)

#Setting version for C++ files
configure_file (
    source/Version.hpp.in
    source/Version.hpp
    )

#Include Cotire
set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
include(cotire)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Core Widgets LinguistTools REQUIRED)

#Include external libraries

#plog
message("Adding Plog library")
add_subdirectory(include/libs/plog)
add_compile_definitions(PLOG_OMIT_PLOG_DEFINES)
#LibBsarch
message("Adding Libbsarch library")
add_subdirectory(include/libs/libbsarch)
#hkxcmd
message("Adding hkxcmd library")
add_subdirectory(include/libs/hkxcmd)
#Nif Library
message("Adding nif library")
add_subdirectory(include/libs/NIF)
#DirectXTex library
message("Adding DirectXTex library")
add_subdirectory("include/libs/DirectXTex")
#Json library
message("Adding json library")
add_subdirectory("include/libs/json")

set(SOURCES
    source/FilesystemOperations.cpp
    source/FilesystemOperations.hpp
    source/Logger.hpp
    source/main.cpp
    source/MainOptimizer.cpp
    source/MainOptimizer.hpp
    source/Manager.cpp
    source/Manager.hpp
    source/pch.hpp
    source/Version.hpp
    source/Version.hpp.in
    source/Commands/Command.cpp
    source/Commands/Command.hpp
    source/Commands/CommandBook.cpp
    source/Commands/CommandBook.hpp
    source/Commands/CommandResult.cpp
    source/Commands/CommandResult.hpp
    source/Commands/Animations/AnimationsConvert.cpp
    source/Commands/Animations/AnimationsConvert.hpp
    source/Commands/BSA/BSACreate.cpp
    source/Commands/BSA/BSACreate.hpp
    source/Commands/BSA/BSAExtract.cpp
    source/Commands/BSA/BSAExtract.hpp
    source/Commands/BSA/Utils/BSA.cpp
    source/Commands/BSA/Utils/BSA.hpp
    source/Commands/BSA/Utils/BSACallback.hpp
    source/Commands/BSA/Utils/BSASplit.cpp
    source/Commands/BSA/Utils/BSASplit.hpp
    source/Commands/Meshes/Mesh.hpp
    source/Commands/Meshes/MeshConvert.cpp
    source/Commands/Meshes/MeshConvert.hpp
    source/Commands/Meshes/MeshRenameReferencedTextures.cpp
    source/Commands/Meshes/MeshRenameReferencedTextures.hpp
    source/Commands/Plugins/PluginsOperations.cpp
    source/Commands/Plugins/PluginsOperations.hpp
    source/Commands/Textures/Texture.hpp
    source/Commands/Textures/TextureCompressionDevice.cpp
    source/Commands/Textures/TextureCompressionDevice.hpp
    source/Commands/Textures/TextureConvert.cpp
    source/Commands/Textures/TextureConvert.hpp
    source/Commands/Textures/TextureDecompress.cpp
    source/Commands/Textures/TextureDecompress.hpp
    source/Commands/Textures/TextureFormats.hpp
    source/Commands/Textures/TextureGenerateMipmaps.cpp
    source/Commands/Textures/TextureGenerateMipmaps.hpp
    source/Commands/Textures/TextureResize.cpp
    source/Commands/Textures/TextureResize.hpp
    source/File/File.cpp
    source/File/File.hpp
    source/File/Resources.hpp
    source/File/Animations/AnimationFile.cpp
    source/File/Animations/AnimationFile.hpp
    source/File/BSA/BSAFile.cpp
    source/File/BSA/BSAFile.hpp
    source/File/BSA/BSAFolder.cpp
    source/File/BSA/BSAFolder.hpp
    source/File/Meshes/MeshFile.cpp
    source/File/Meshes/MeshFile.hpp
    source/File/Textures/TextureFile.cpp
    source/File/Textures/TextureFile.hpp
    source/Settings/BaseTypes.hpp
    source/Settings/JSON.cpp
    source/Settings/JSON.hpp
    source/Settings/Settings.cpp
    source/Settings/Settings.hpp
    source/Settings/Profiles.cpp
    source/Settings/Profiles.hpp
    source/Settings/SettingsList.hpp
    )

if(GUI)
    list(APPEND SOURCES
        source/GUI/BSAFilesToPack.cpp
        source/GUI/BSAFilesToPack.hpp
        source/GUI/BSAFilesToPack.ui
        source/GUI/ListDialog.cpp
        source/GUI/ListDialog.hpp
        source/GUI/ListDialog.ui
        source/GUI/MainWindow.cpp
        source/GUI/MainWindow.hpp
        source/GUI/MainWindow.ui)
endif()


#Macro used for standard cao settings
macro(cao_include_and_link file)
    target_link_libraries(${file}
        Qt5::Core Qt5::Widgets
        hkxcmd plog nif directxtex Qlibbsarch nlohmann_json::nlohmann_json)

    #Include source, C++17
    target_include_directories(${file} PUBLIC "include")
    target_include_directories(${file} PUBLIC "source" )
    target_include_directories(${file} PUBLIC "${CMAKE_BINARY_DIR}/CAO_LIB_autogen/include")
    target_compile_features(${file} PUBLIC cxx_std_17)

    #Set GUI compile definition
    if(GUI)
        target_compile_definitions(${file} PUBLIC GUI)
    endif()


    #Output directory
    set_target_properties(${file}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        )
    set_target_properties(${file} PROPERTIES AUTOUIC_SEARCH_PATHS "${CMAKE_SOURCE_DIR}/source/GUI")
endmacro()

#MAIN LIBRARY
add_library(CAO_LIB STATIC ${SOURCES})
cao_include_and_link(CAO_LIB)

#Main exe
add_executable(Cathedral_Assets_Optimizer WIN32 source/main.cpp styles/Cathedral_Assets_Optimizer.rc styles/qdarkstyle/style.qrc)
target_link_libraries(Cathedral_Assets_Optimizer CAO_LIB)
cao_include_and_link(Cathedral_Assets_Optimizer)

#Tests
add_library(doctest tests/doctest.h)
add_executable(CAO_test tests/tests.cpp)
target_link_libraries(CAO_test CAO_LIB)
cao_include_and_link(CAO_test)

#Generate translations
set(TS_FILES
    source/translations/AssetsOpt_de.ts
    source/translations/AssetsOpt_en.ts
    source/translations/AssetsOpt_fr.ts
    source/translations/AssetsOpt_ja.ts
    )

message("Updating translations files")
qt5_create_translation(QM_FILES ${TS_FILES} ${SOURCES})

#Cotire (PCH)
set_target_properties(Cathedral_Assets_Optimizer PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "source/pch.hpp")
cotire(Cathedral_Assets_Optimizer)
