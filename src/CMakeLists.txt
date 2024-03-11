set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(SOURCE_DIR ${ROOT_DIR}/src)
set(DATA_DIR ${ROOT_DIR}/data)

# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
# Resources
set(CMAKE_AUTORCC ON)


set(SOURCES
        ${SOURCE_DIR}/logger.cpp
        ${SOURCE_DIR}/logger.hpp
        ${SOURCE_DIR}/main_process.cpp
        ${SOURCE_DIR}/main_process.hpp
        ${SOURCE_DIR}/manager.cpp
        ${SOURCE_DIR}/manager.hpp
        ${SOURCE_DIR}/settings/base_types.hpp
        ${SOURCE_DIR}/settings/json.hpp
        ${SOURCE_DIR}/settings/per_file_settings.hpp
        ${SOURCE_DIR}/settings/settings.cpp
        ${SOURCE_DIR}/settings/settings.hpp
        ${SOURCE_DIR}/settings/profile.hpp
        ${SOURCE_DIR}/gui/ListDialog.cpp
        ${SOURCE_DIR}/gui/ListDialog.hpp
        ${SOURCE_DIR}/gui/ListDialog.ui
        ${SOURCE_DIR}/gui/LevelSelector.cpp
        ${SOURCE_DIR}/gui/LevelSelector.hpp
        ${SOURCE_DIR}/gui/LevelSelector.ui
        ${SOURCE_DIR}/gui/ProgressWindow.cpp
        ${SOURCE_DIR}/gui/ProgressWindow.hpp
        ${SOURCE_DIR}/gui/ProgressWindow.ui
        ${SOURCE_DIR}/gui/MainWindow.cpp
        ${SOURCE_DIR}/gui/MainWindow.hpp
        ${SOURCE_DIR}/gui/MainWindow.ui

        ${SOURCE_DIR}/gui/ModuleDisplay.cpp
        ${SOURCE_DIR}/gui/ModuleDisplay.hpp

        ${SOURCE_DIR}/gui/AdvancedAnimationsModule.cpp
        ${SOURCE_DIR}/gui/AdvancedAnimationsModule.hpp
        ${SOURCE_DIR}/gui/AdvancedAnimationsModule.ui
        ${SOURCE_DIR}/gui/AdvancedMeshesModule.cpp
        ${SOURCE_DIR}/gui/AdvancedMeshesModule.hpp
        ${SOURCE_DIR}/gui/AdvancedMeshesModule.ui
        ${SOURCE_DIR}/gui/AdvancedBSAModule.cpp
        ${SOURCE_DIR}/gui/AdvancedBSAModule.hpp
        ${SOURCE_DIR}/gui/AdvancedBSAModule.ui
        ${SOURCE_DIR}/gui/AdvancedTexturesModule.cpp
        ${SOURCE_DIR}/gui/AdvancedTexturesModule.hpp
        ${SOURCE_DIR}/gui/AdvancedTexturesModule.ui
        ${SOURCE_DIR}/gui/ProfilesManagerWindow.cpp
        ${SOURCE_DIR}/gui/ProfilesManagerWindow.hpp
        ${SOURCE_DIR}/gui/ProfilesManagerWindow.ui
        ${SOURCE_DIR}/gui/PatternsManagerWindow.cpp
        ${SOURCE_DIR}/gui/PatternsManagerWindow.hpp
        ${SOURCE_DIR}/gui/PatternsManagerWindow.ui
        ${SOURCE_DIR}/gui/SelectGPUWindow.cpp
        ${SOURCE_DIR}/gui/SelectGPUWindow.hpp
        ${SOURCE_DIR}/gui/SelectGPUWindow.ui
        ${SOURCE_DIR}/gui/IWindowModule.hpp
        ${SOURCE_DIR}/gui/IWindowModule.cpp
        ${SOURCE_DIR}/gui/utils/utils.hpp)

set(TS_FILES
        ${DATA_DIR}/translations/AssetsOpt_de.ts
        ${DATA_DIR}/translations/AssetsOpt_en.ts
        ${DATA_DIR}/translations/AssetsOpt_es.ts
        ${DATA_DIR}/translations/AssetsOpt_fr_FR.ts
        ${DATA_DIR}/translations/AssetsOpt_ja.ts
        ${DATA_DIR}/translations/AssetsOpt_ko_KR.ts
        ${DATA_DIR}/translations/AssetsOpt_nl.ts
        ${DATA_DIR}/translations/AssetsOpt_ru.ts
        ${DATA_DIR}/translations/AssetsOpt_tr.ts
        ${DATA_DIR}/translations/AssetsOpt_zh_CN.ts)

# Main library
add_library(CAO_LIB INTERFACE)
target_sources(CAO_LIB INTERFACE ${SOURCES} ${TS_FILES})

# Include source
target_include_directories(CAO_LIB INTERFACE ${SOURCE_DIR})

# Set version file
configure_file(${SOURCE_DIR}/version.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/version/version.hpp @ONLY)
target_include_directories(CAO_LIB INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/version)

# Set C++ 20
target_compile_features(CAO_LIB INTERFACE cxx_std_20)

# Link with external libraries
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(CAO_LIB INTERFACE nlohmann_json::nlohmann_json)

find_package(bethutil CONFIG REQUIRED)
target_link_libraries(CAO_LIB INTERFACE btu::bethutil)

find_package(platform_folders CONFIG REQUIRED)
target_link_libraries(CAO_LIB INTERFACE sago::platform_folders)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(CAO_LIB INTERFACE fmt::fmt)

find_package(Qt6 COMPONENTS Core Widgets Gui LinguistTools REQUIRED)
target_link_libraries(CAO_LIB INTERFACE Qt6::Core Qt6::Widgets Qt6::Gui)

# Main exe
add_executable(Cathedral_Assets_Optimizer WIN32
        ${SOURCE_DIR}/main.cpp
        ${DATA_DIR}/styles/Cathedral_Assets_Optimizer.rc
        ${DATA_DIR}/styles/qdarkstyle/style.qrc
)

target_link_libraries(Cathedral_Assets_Optimizer PUBLIC CAO_LIB)
