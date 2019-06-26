cmake_minimum_required(VERSION 3.12)

project(Cathedral_Assets_Optimizer)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
#Ressources
set(CMAKE_AUTORCC ON)

#NEEDS TO BE CHANGED
set(Qt5_DIR "E:/Qt/5.12.3/msvc2017/lib/cmake/Qt5")
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

#parent dir
get_filename_component(PARENT_DIR ${PROJECT_SOURCE_DIR} DIRECTORY)
include_directories(${PARENT_DIR}/CAO_shared)

#SOURCES

set(SRCS
    main.cpp
    Custom.cpp
    #${PARENT_DIR}/CAO_shared/Ini.cpp
    )


#HEADERS

set(HEADERS
    pch.h
    Custom.h
    #${PARENT_DIR}/CAO_shared/Ini.h
    )

#Ui files

set(UI
    Custom.ui
    )

#Translations

SET(QON_TS_FILES
    translations/Cathedral_Assets_Optimizer_en.ts
    translations/Cathedral_Assets_Optimizer_de.ts
    translations/Cathedral_Assets_Optimizer_fr.ts
    translations/Cathedral_Assets_Optimizer_pl.ts
    translations/Cathedral_Assets_Optimizer_zh.ts
    translations/Cathedral_Assets_Optimizer_ru.ts
    translations/Cathedral_Assets_Optimizer_pt.ts
    translations/Cathedral_Assets_Optimizer_es.ts
    translations/Cathedral_Assets_Optimizer_nl.ts
    translations/Cathedral_Assets_Optimizer_hu.ts
    translations/Cathedral_Assets_Optimizer_ja.ts
    translations/Cathedral_Assets_Optimizer_it.ts
    translations/Cathedral_Assets_Optimizer_ar.ts
    )

##qt5_add_translation(QON_QM_FILES ${QON_TS_FILES})

#Icon and qrc

set(ICON_RC_PATH "${PROJECT_SOURCE_DIR}/Cathedral_Assets_Optimizer.rc")
set(QRC_PATH "${PROJECT_SOURCE_DIR}/styles/qdarkstyle/style.qrc")

add_executable(Cathedral_Assets_Optimizer WIN32 ${SRCS} ${HEADERS} ${UI} ${ICON_RC_PATH} ${QRC_PATH})
target_link_libraries(Cathedral_Assets_Optimizer Qt5::Core)
target_link_libraries(Cathedral_Assets_Optimizer Qt5::Widgets)

