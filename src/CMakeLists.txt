set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/Logger.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Logger.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/main_process.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/main_process.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/manager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/manager.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Version.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Utils/CallOnce.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Utils/CallOnce.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Utils/Filesystem.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Utils/ScopeGuard.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Utils/Filesystem.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Utils/TypeConvert.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/settings/base_types.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/settings/json.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/settings/per_file_settings.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/settings/settings.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/settings/settings.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/settings/profile.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/ListDialog.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/ListDialog.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/ListDialog.ui
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/LevelSelector.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/LevelSelector.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/LevelSelector.ui
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/ProgressWindow.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/ProgressWindow.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/ProgressWindow.ui
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/MainWindow.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/MainWindow.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/MainWindow.ui
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/IntermediateModeModule.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/IntermediateModeModule.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/IntermediateModeModule.ui
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/AdvancedAnimationsModule.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/AdvancedAnimationsModule.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/AdvancedAnimationsModule.ui
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/AdvancedMeshesModule.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/AdvancedMeshesModule.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/AdvancedMeshesModule.ui
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/AdvancedBSAModule.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/AdvancedBSAModule.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/AdvancedBSAModule.ui
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/AdvancedTexturesModule.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/AdvancedTexturesModule.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/AdvancedTexturesModule.ui
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/ProfilesManagerWindow.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/ProfilesManagerWindow.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/ProfilesManagerWindow.ui
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/PatternsManagerWindow.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/PatternsManagerWindow.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/PatternsManagerWindow.ui
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/SelectGPUWindow.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/SelectGPUWindow.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/SelectGPUWindow.ui
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/IWindowModule.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/IWindowModule.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/Utils.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/Utils/SetTheme.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/Utils/SetTheme.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI/Utils/ConnectionWrapper.hpp)

# Resources
getresources(resources)

# Main library
add_library(CAO_LIB STATIC ${SOURCES} ${resources})

# Include source
target_include_directories(CAO_LIB PUBLIC ${CMAKE_SOURCE_DIR}/external
        ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_features(CAO_LIB PUBLIC cxx_std_20)

# AUTOUIC I don't know why, but ui files are not found if I do not manually
# include them
target_include_directories(
        CAO_LIB PUBLIC "${CMAKE_BINARY_DIR}/src/CAO_LIB_autogen/include")

# Link with external libraries
target_link_libraries(CAO_LIB PUBLIC Qt)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(CAO_LIB PUBLIC nlohmann_json::nlohmann_json)

find_package(bethutil CONFIG REQUIRED)
target_link_libraries(CAO_LIB PUBLIC btu::bethutil)

find_package(platform_folders CONFIG REQUIRED)
target_link_libraries(CAO_LIB PRIVATE sago::platform_folders)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(CAO_LIB PRIVATE fmt::fmt)


# Icon
geticon(icon)

# Translations
generatetranslations("${SOURCES}" qm_files)

# Main exe
add_executable(Cathedral_Assets_Optimizer WIN32
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${icon} ${qm_files})
target_link_libraries(Cathedral_Assets_Optimizer PRIVATE CAO_LIB)
