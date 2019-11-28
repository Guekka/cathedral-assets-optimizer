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

#Setting GUI
#Comment out the next lines to create a CLI version instead (untested)
add_compile_definitions(GUI)

#Macro used for standard cao settings
macro(cao_include_and_link file)
    get_target_property(target_type ${file} TYPE)
    set(lib_type "PUBLIC")
    if (target_type STREQUAL "INTERFACE_LIBRARY")
        set(lib_type "INTERFACE")
    endif()

    target_link_libraries(${file} ${lib_type}
        Qt5::Core Qt5::Widgets
        hkxcmd plog nif directxtex Qlibbsarch nlohmann_json::nlohmann_json)

    #Include source, C++17
    target_include_directories(${file} ${lib_type} "source")
    target_include_directories(${file} ${lib_type} "include")

    if (NOT target_type STREQUAL "INTERFACE_LIBRARY")
        target_compile_features(${file} PUBLIC cxx_std_17)

        #Output directory
        set_target_properties(${file}
            PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
            )
        set_target_properties(${file} PROPERTIES AUTOUIC_SEARCH_PATHS "${CMAKE_SOURCE_DIR}/source/GUI")
    endif()
endmacro()

#Splitting CAO into several libraries in order to reduce compilation time

set(SOURCES
    source/FilesystemOperations
    source/Logger
    source/MainOptimizer
    source/Manager
    source/pch
    source/Commands/Command
    source/Commands/CommandBook
    source/Commands/CommandResult
    source/Commands/Animations/AnimationsConvert
    source/Commands/BSA/BSACreate
    source/Commands/BSA/BSAExtract
    source/Commands/BSA/Utils/BSA
    source/Commands/BSA/Utils/BSACallback
    source/Commands/BSA/Utils/BSASplit
    source/Commands/Meshes/Mesh
    source/Commands/Meshes/MeshConvert
    source/Commands/Meshes/MeshRenameReferencedTextures
    source/Commands/Plugins/PluginsOperations
    source/Commands/Textures/Texture
    source/Commands/Textures/TextureCompressionDevice
    source/Commands/Textures/TextureConvert
    source/Commands/Textures/TextureDecompress
    source/Commands/Textures/TextureFormats
    source/Commands/Textures/TextureGenerateMipmaps
    source/Commands/Textures/TextureResize
    source/File/File
    source/File/Resources
    source/File/Animations/AnimationFile
    source/File/BSA/BSAFile
    source/File/BSA/BSAFolder
    source/File/Meshes/MeshFile
    source/File/Textures/TextureFile
    source/GUI/MainWindow
    source/GUI/TexturesFormatSelectDialog
    source/Settings/AdvancedSettings
    source/Settings/Profiles
    source/Settings/Settings
    source/Settings/StandardSettings
    source/Settings/Utils/InitJSON
    )

set(extensions ".cpp" ".hpp" ".ui")

foreach(unit IN LISTS SOURCES)
    set(SourceForThisUnit)
    foreach(ext IN LISTS extensions)
        if(EXISTS ${CMAKE_SOURCE_DIR}/${unit}${ext})
            list(APPEND SourceForThisUnit ${unit}${ext})
        endif()
    endforeach()

    string(REPLACE "source/" "" unit_tmp ${unit})
    string(REPLACE "/" "_" unit_tmp ${unit_tmp})
    set(unit_tmp "CAO_${unit_tmp}")

    add_library(${unit_tmp} STATIC ${SourceForThisUnit})
    cao_include_and_link(${unit_tmp})
endforeach()

#MAIN LIBRARY
add_library(CAO_LIB INTERFACE)
foreach(unit IN LISTS SOURCES)
    string(REPLACE "source/" "" unit_tmp ${unit})
    string(REPLACE "/" "_" unit_tmp ${unit_tmp})
    set(unit_tmp "CAO_${unit_tmp}")
    target_link_libraries(CAO_LIB INTERFACE ${unit_tmp})
endforeach()
cao_include_and_link(CAO_LIB)

#Main exe
add_executable(Cathedral_Assets_Optimizer WIN32 source/main.cpp styles/Cathedral_Assets_Optimizer.rc styles/qdarkstyle/style.qrc)
target_link_libraries(Cathedral_Assets_Optimizer PRIVATE CAO_LIB )
cao_include_and_link(Cathedral_Assets_Optimizer)

#Tests
add_library(doctest tests/doctest.h)
add_executable(CAO_test tests/tests.cpp)
target_link_libraries(CAO_test PRIVATE CAO_LIB)
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
