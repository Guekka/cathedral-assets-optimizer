cmake_minimum_required(VERSION 3.16)
include_guard(GLOBAL)

project(Cathedral_Assets_Optimizer VERSION 6.0.0 LANGUAGES CXX)

#Set to false to create a CLI version instead (untested)
option(GUI "Create GUI version of CAO" ON)

# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
#Ressources
set(CMAKE_AUTORCC ON)

#Setting version for C++ files
configure_file (src/Version.hpp.in src/Version.hpp)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Core Widgets LinguistTools REQUIRED)

#Include external libraries

#plog
message("Adding Plog library")
add_subdirectory(external/plog)
add_compile_definitions(PLOG_OMIT_PLOG_DEFINES)
#LibBsarch
message("Adding Libbsarch library")
add_subdirectory(external/libbsarch)
#hkxcmd
message("Adding hkxcmd library")
add_subdirectory(external/hkxcmd)
#Nif Library
message("Adding nif library")
add_subdirectory(external/NIF)
#DirectXTex library
message("Adding DirectXTex library")
add_subdirectory("external/DirectXTex")
#Json library
message("Adding json library")
add_subdirectory("external/json")
#Pipes library
message("Adding Pipes library")
add_subdirectory("external/pipes")
#Doctest
add_library(doctest "external/doctest.h")

set(SOURCES
    src/Logger.hpp
    src/main.cpp
    src/MainOptimizer.cpp
    src/MainOptimizer.hpp
    src/Manager.cpp
    src/Manager.hpp
    src/pch.hpp
    src/Version.hpp
    src/Version.hpp.in

    src/Utils/Filesystem.cpp
    src/Utils/Filesystem.hpp
    src/Utils/TemplateMetaProgramming.hpp
    src/Utils/TypeConvert.hpp

    src/Commands/Command.cpp
    src/Commands/Command.hpp
    src/Commands/CommandBook.cpp
    src/Commands/CommandBook.hpp
    src/Commands/CommandResult.cpp
    src/Commands/CommandResult.hpp

    src/Commands/Animations/AnimationsConvert.cpp
    src/Commands/Animations/AnimationsConvert.hpp

    src/Commands/BSA/BSACreate.cpp
    src/Commands/BSA/BSACreate.hpp
    src/Commands/BSA/BSAExtract.cpp
    src/Commands/BSA/BSAExtract.hpp
    src/Commands/BSA/Utils/BSA.cpp
    src/Commands/BSA/Utils/BSA.hpp
    src/Commands/BSA/Utils/BSACallback.hpp
    src/Commands/BSA/Utils/BSASplit.cpp
    src/Commands/BSA/Utils/BSASplit.hpp

    src/Commands/Meshes/Mesh.hpp
    src/Commands/Meshes/MeshConvert.cpp
    src/Commands/Meshes/MeshConvert.hpp
    src/Commands/Meshes/MeshRenameReferencedTextures.cpp
    src/Commands/Meshes/MeshRenameReferencedTextures.hpp

    src/Commands/Plugins/PluginsOperations.cpp
    src/Commands/Plugins/PluginsOperations.hpp

    src/Commands/Textures/Texture.hpp
    src/Commands/Textures/TextureCompressionDevice.cpp
    src/Commands/Textures/TextureCompressionDevice.hpp
    src/Commands/Textures/TextureConvert.cpp
    src/Commands/Textures/TextureConvert.hpp
    src/Commands/Textures/TextureDecompress.cpp
    src/Commands/Textures/TextureDecompress.hpp
    src/Commands/Textures/TextureFormats.hpp
    src/Commands/Textures/TextureGenerateMipmaps.cpp
    src/Commands/Textures/TextureGenerateMipmaps.hpp
    src/Commands/Textures/TextureResize.cpp
    src/Commands/Textures/TextureResize.hpp

    src/File/File.cpp
    src/File/File.hpp
    src/File/Resources.hpp
    src/File/Animations/AnimationFile.cpp
    src/File/Animations/AnimationFile.hpp
    src/File/BSA/BSAFile.cpp
    src/File/BSA/BSAFile.hpp
    src/File/BSA/BSAFolder.cpp
    src/File/BSA/BSAFolder.hpp
    src/File/Meshes/MeshFile.cpp
    src/File/Meshes/MeshFile.hpp
    src/File/Textures/TextureFile.cpp
    src/File/Textures/TextureFile.hpp

    src/Settings/BaseTypes.hpp
    src/Settings/JSON.cpp
    src/Settings/JSON.hpp
    src/Settings/MigrateProfiles.hpp
    src/Settings/MigrateProfiles.cpp
    src/Settings/Settings.cpp
    src/Settings/Settings.hpp
    src/Settings/PatternMap.cpp
    src/Settings/PatternMap.hpp
    src/Settings/Profiles.cpp
    src/Settings/Profiles.hpp
    src/Settings/UISync.cpp
    src/Settings/UISync.hpp
    src/Settings/Utils/QValueWrapper.hpp
    )

if(GUI)
    list(APPEND SOURCES
        src/GUI/BSAFilesToPack.cpp
        src/GUI/BSAFilesToPack.hpp
        src/GUI/BSAFilesToPack.ui

        src/GUI/ListDialog.cpp
        src/GUI/ListDialog.hpp
        src/GUI/ListDialog.ui

        src/GUI/MainWindow.cpp
        src/GUI/MainWindow.hpp
        src/GUI/MainWindow.ui

        src/GUI/AdvancedTexturesWindow.cpp
        src/GUI/AdvancedTexturesWindow.hpp
        src/GUI/AdvancedTexturesWindow.ui

        src/GUI/IWindowModule.hpp
        )
endif()


#MAIN LIBRARY
add_library(CAO_LIB STATIC ${SOURCES})

target_link_libraries(CAO_LIB PUBLIC
    Qt5::Core Qt5::Widgets
    hkxcmd plog nif directxtex libbsarch_OOP nlohmann_json::nlohmann_json pipes)

#Include source, C++17
target_include_directories(CAO_LIB PUBLIC "external")
target_include_directories(CAO_LIB PUBLIC "src" )
target_compile_features(CAO_LIB PUBLIC cxx_std_17)

#Set GUI compile definition
if(GUI)
    target_compile_definitions(CAO_LIB PUBLIC GUI)
endif()

#Strictly follow the standard
target_compile_options(CAO_LIB PRIVATE "/permissive-")

#AUTOUIC
set_target_properties(CAO_LIB PROPERTIES AUTOUIC_SEARCH_PATHS "${CMAKE_SOURCE_DIR}/src/GUI")
target_include_directories(CAO_LIB PUBLIC "${CMAKE_BINARY_DIR}/CAO_LIB_autogen/include")

#Main exe
add_executable(Cathedral_Assets_Optimizer WIN32
    src/main.cpp
    data/styles/Cathedral_Assets_Optimizer.rc
    data/styles/qdarkstyle/style.qrc)
target_link_libraries(Cathedral_Assets_Optimizer CAO_LIB)

#Tests
add_executable(CAO_test tests/tests.cpp)
target_link_libraries(CAO_test CAO_LIB)

#Generate translations
set(TS_FILES
    src/translations/AssetsOpt_de.ts
    src/translations/AssetsOpt_en.ts
    src/translations/AssetsOpt_fr.ts
    src/translations/AssetsOpt_ja.ts
    )

message("Updating translations files")
qt5_create_translation(QM_FILES ${TS_FILES} ${SOURCES})

#PCH
target_precompile_headers(Cathedral_Assets_Optimizer PUBLIC "src/pch.hpp")
