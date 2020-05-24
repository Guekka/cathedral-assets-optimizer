set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Logger.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MainOptimizer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MainOptimizer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Manager.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pch.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Version.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Version.hpp.in

    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/Filesystem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/Filesystem.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/TemplateMetaProgramming.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/TypeConvert.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/Command.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/Command.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/CommandBook.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/CommandBook.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/CommandResult.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/CommandResult.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/Animations/AnimationsConvert.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/Animations/AnimationsConvert.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/BSA/BSACreate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/BSA/BSACreate.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/BSA/BSAExtract.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/BSA/BSAExtract.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/BSA/Utils/BSA.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/BSA/Utils/BSA.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/BSA/Utils/BSACallback.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/BSA/Utils/BSASplit.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/BSA/Utils/BSASplit.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/Meshes/Mesh.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/Meshes/MeshConvert.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/Meshes/MeshConvert.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/Meshes/MeshRenameReferencedTextures.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/Meshes/MeshRenameReferencedTextures.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/Plugins/PluginsOperations.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/Plugins/PluginsOperations.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/Textures/Texture.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/Textures/TextureCompressionDevice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/Textures/TextureCompressionDevice.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/Textures/TextureConvert.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/Textures/TextureConvert.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/Textures/TextureDecompress.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/Textures/TextureDecompress.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/Textures/TextureFormats.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/Textures/TextureGenerateMipmaps.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/Textures/TextureGenerateMipmaps.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/Textures/TextureResize.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Commands/Textures/TextureResize.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/File/File.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/File/File.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/File/Resources.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/File/Animations/AnimationFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/File/Animations/AnimationFile.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/File/BSA/BSAFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/File/BSA/BSAFile.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/File/BSA/BSAFolder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/File/BSA/BSAFolder.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/File/Meshes/MeshFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/File/Meshes/MeshFile.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/File/Textures/TextureFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/File/Textures/TextureFile.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/Settings/BaseTypes.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Settings/JSON.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Settings/JSON.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Settings/MigrateProfiles.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Settings/MigrateProfiles.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Settings/Settings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Settings/Settings.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Settings/GeneralSettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Settings/GeneralSettings.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Settings/PatternSettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Settings/PatternSettings.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Settings/FileTypes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Settings/FileTypes.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Settings/PatternMap.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Settings/PatternMap.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Settings/Profiles.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Settings/Profiles.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Settings/UISync.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Settings/UISync.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Settings/Utils/QJSONValueWrapper.hpp)

if(GUI)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/BSAFilesToPack.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/BSAFilesToPack.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/BSAFilesToPack.ui

        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/ListDialog.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/ListDialog.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/ListDialog.ui

        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/MainWindow.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/MainWindow.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/MainWindow.ui

        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/AdvancedTexturesWindow.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/AdvancedTexturesWindow.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/AdvancedTexturesWindow.ui

        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/IWindowModule.hpp)
endif()


#Main library
add_library(CAO_LIB STATIC ${SOURCES})

#Include source
target_include_directories(CAO_LIB PUBLIC
    ${CMAKE_SOURCE_DIR}/external
    ${CMAKE_CURRENT_SOURCE_DIR})

#C++17
target_compile_features(CAO_LIB PUBLIC cxx_std_17)

#Set GUI compile definition
if(GUI)
    target_compile_definitions(CAO_LIB PUBLIC GUI)
endif()

#Strictly follow the standard
target_compile_options(CAO_LIB PRIVATE "/permissive-")

#AUTOUIC
#I don't know why, but ui files are not found if I do not manually include them
target_include_directories(CAO_LIB PUBLIC "${CMAKE_BINARY_DIR}/src/CAO_LIB_autogen/include")

#Link with external libraries
getExternalLibrariesList(libs)
target_link_libraries(CAO_LIB PUBLIC ${libs} Qt)

#Main exe
add_executable(Cathedral_Assets_Optimizer WIN32
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(Cathedral_Assets_Optimizer CAO_LIB)


