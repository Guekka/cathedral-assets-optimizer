set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/logger.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/logger.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/main_process.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/main_process.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/manager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/manager.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/version.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/settings/base_types.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/settings/json.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/settings/per_file_settings.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/settings/settings.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/settings/settings.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/settings/profile.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/ListDialog.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/ListDialog.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/ListDialog.ui
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/LevelSelector.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/LevelSelector.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/LevelSelector.ui
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/ProgressWindow.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/ProgressWindow.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/ProgressWindow.ui
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/MainWindow.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/MainWindow.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/MainWindow.ui
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/IntermediateModeModule.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/IntermediateModeModule.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/IntermediateModeModule.ui
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/AdvancedAnimationsModule.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/AdvancedAnimationsModule.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/AdvancedAnimationsModule.ui
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/AdvancedMeshesModule.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/AdvancedMeshesModule.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/AdvancedMeshesModule.ui
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/AdvancedBSAModule.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/AdvancedBSAModule.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/AdvancedBSAModule.ui
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/AdvancedTexturesModule.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/AdvancedTexturesModule.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/AdvancedTexturesModule.ui
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/ProfilesManagerWindow.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/ProfilesManagerWindow.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/ProfilesManagerWindow.ui
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/PatternsManagerWindow.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/PatternsManagerWindow.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/PatternsManagerWindow.ui
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/SelectGPUWindow.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/SelectGPUWindow.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/SelectGPUWindow.ui
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/IWindowModule.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/IWindowModule.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/utils/utils.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/utils/set_theme.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/utils/set_theme.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gui/utils/ConnectionWrapper.hpp)

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
