cmake_minimum_required(VERSION 3.12)

project(Cathedral_Assets_Optimizer_front)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

#SOURCES

set(SRCS
    main.cpp
    SkyrimSE.cpp
    )


#HEADERS

set(HEADERS
    SkyrimSE.h
    pch.h
    )

#Translations

SET(QON_TS_FILES
    translations/Cathedral_Assets_Optimizer_front_en.ts
    translations/Cathedral_Assets_Optimizer_front_de.ts
    translations/Cathedral_Assets_Optimizer_front_fr.ts
    translations/Cathedral_Assets_Optimizer_front_pl.ts
    translations/Cathedral_Assets_Optimizer_front_zh.ts
    translations/Cathedral_Assets_Optimizer_front_ru.ts
    translations/Cathedral_Assets_Optimizer_front_pt.ts
    translations/Cathedral_Assets_Optimizer_front_es.ts
    translations/Cathedral_Assets_Optimizer_front_nl.ts
    translations/Cathedral_Assets_Optimizer_front_hu.ts
    translations/Cathedral_Assets_Optimizer_front_ja.ts
    translations/Cathedral_Assets_Optimizer_front_it.ts
    translations/Cathedral_Assets_Optimizer_front_ar.ts
    )

##qt5_add_translation(QON_QM_FILES ${QON_TS_FILES})

add_executable(Cathedral_Assets_Optimizer_front ${SRCS} ${HEADERS})
target_link_libraries(Cathedral_Assets_Optimizer_front Qt5::Core)
target_link_libraries(Cathedral_Assets_Optimizer_front Qt5::Widgets)

