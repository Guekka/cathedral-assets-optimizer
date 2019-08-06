cmake_minimum_required(VERSION 3.12)

project(Cathedral_Assets_Optimizer)

#Include Cotire

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")
include(cotire)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
#Ressources
set(CMAKE_AUTORCC ON)

#C++17

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#Qt
set(Qt5_DIR "C:/IT/Qt/5.12.4/msvc2017/lib/cmake/Qt5")
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

#Setting GUI
#Comment out the next line to create a CLI version instead (untested)
add_compile_definitions(GUI)

#SOURCES

set(SOURCES
    main.cpp
    Manager.cpp
    TexturesOptimizer.cpp
    MeshesOptimizer.cpp
    BsaOptimizer.cpp
    FilesystemOperations.cpp
    PluginsOperations.cpp
    AnimationsOptimizer.cpp
    MainOptimizer.cpp
    IntegrationTests.cpp
    Profiles.cpp
    OptionsCAO.cpp

    #Ui
    MainWindow.cpp
    )


#HEADERS

set(HEADERS
    Manager.h
    TexturesOptimizer.h
    MeshesOptimizer.h
    BsaOptimizer.h
    FilesystemOperations.h
    PluginsOperations.h
    AnimationsOptimizer.h
    MainOptimizer.h
    IntegrationTests.h
    pch.h
    Profiles.h
    Logger.h
    OptionsCAO.h
    TexturesFormats.h
    #Ui
    MainWindow.h
    )

#Ui
set(UI
    MainWindow.ui
    )

#Icon and qrc
set(ICON_RC_PATH "${PROJECT_SOURCE_DIR}/Cathedral_Assets_Optimizer.rc")
set(QRC_PATH "${PROJECT_SOURCE_DIR}/styles/qdarkstyle/style.qrc")

#Groups
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

source_group(
    TREE ${PROJECT_SOURCE_DIR}
    PREFIX "Headers"
    FILES ${HEADERS})
source_group(
    TREE ${PROJECT_SOURCE_DIR}
    PREFIX "Sources"
    FILES ${SOURCES})

#plog

include_directories(${PROJECT_SOURCE_DIR}/libs/plog/include)
file(GLOB_RECURSE HEADERS_PLOG ${PROJECT_SOURCE_DIR}/libs/plog/include/*.h)

#LibBsarch

include_directories(${PROJECT_SOURCE_DIR}/libs/libbsarch)
file(GLOB_RECURSE SOURCES_LIBBSARCH ${PROJECT_SOURCE_DIR}/libs/libbsarch/*.cpp)
file(GLOB_RECURSE HEADERS_LIBBSARCH ${PROJECT_SOURCE_DIR}/libs/libbsarch/*.h)

#Havok

include_directories("${PROJECT_SOURCE_DIR}/libs/havok_2010_2_0/Source")

set ( HAVOK_LIBS_DIR_RELEASE
    ${PROJECT_SOURCE_DIR}/libs/havok_2010_2_0/Lib/win32_net_9-0/release_multithreaded)

set ( HAVOK_LIBS_DIR_DEBUG
    ${PROJECT_SOURCE_DIR}/libs/havok_2010_2_0/Lib/win32_net_9-0/debug_multithreaded)

set ( HAVOK_LIBS
    hkaAnimation
    hkaInternal
    hkaRagdoll
    hkBase
    hkCompat
    hkgBridge
    hkgCommon
    hkgDx10
    hkgDx9
    hkgDx9s
    hkGeometryUtilities
    hkgOgl
    hkgOglES
    hkgOglES2
    hkgOgls
    hkInternal
    hkpCollide
    hkpConstraintSolver
    hkpDynamics
    hkpInternal
    hkpUtilities
    hkpVehicle
    hkSceneData
    hksCommon
    hkSerialize
    hksXAudio2
    hkVisualize
    )

foreach(havok_lib IN LISTS HAVOK_LIBS)
    message( "[MAIN]: adding ${havok_lib}" )

    add_library(
        ${havok_lib}
        STATIC
        IMPORTED GLOBAL
        )

    set_target_properties(
        ${havok_lib}
        PROPERTIES IMPORTED_LOCATION
        "${HAVOK_LIBS_DIR_RELEASE}/${havok_lib}.lib"
        )

    set_target_properties(
        ${havok_lib}
        PROPERTIES IMPORTED_LOCATION_DEBUG
        "${HAVOK_LIBS_DIR_DEBUG}/${havok_lib}.lib"
        )

endforeach(havok_lib)

#hkxcmd

include_directories(${PROJECT_SOURCE_DIR}/libs/hkxcmd)
file(GLOB_RECURSE SOURCES_CK-CMD ${PROJECT_SOURCE_DIR}/libs/hkxcmd/*.cpp)
file(GLOB_RECURSE HEADERS_CK-CMD ${PROJECT_SOURCE_DIR}/libs/hkxcmd/*.h)

#2010 Behavior compat

set(HAVOK_COMPAT_INCLUDES_PATH
    "${PROJECT_SOURCE_DIR}/libs/havok_2010_2_0/compat"
    )

include_directories(${HAVOK_COMPAT_INCLUDES_PATH})

file(GLOB_RECURSE HAVOK_COMPAT_HEADERS
    "${HAVOK_COMPAT_INCLUDES_PATH}/*.h"
    "${HAVOK_COMPAT_INCLUDES_PATH}/*.inl"
    )

file(GLOB_RECURSE HAVOK_COMPAT_SRC
    "${HAVOK_COMPAT_INCLUDES_PATH}/*.cpp"
    )

#Nif Library

set(NIFLIB_INCLUDES_PATH
    "${PROJECT_SOURCE_DIR}/libs/NIF"
    )

include_directories(${NIFLIB_INCLUDES_PATH})

file(GLOB_RECURSE NIFLIB_HEADERS
    "${NIFLIB_INCLUDES_PATH}/*.h"
    )

file(GLOB_RECURSE NIFLIB_SRC
    "${NIFLIB_INCLUDES_PATH}/*.cpp"
    )

#DirectXTex library


set(DIRECTX_INCLUDES_PATH
    "${PROJECT_SOURCE_DIR}/libs/DirectXTex"
    )

include_directories(${DIRECTX_INCLUDES_PATH})

file(GLOB_RECURSE DIRECTX_HEADERS
    "${DIRECTX_INCLUDES_PATH}/*.h"
    )

file(GLOB_RECURSE DIRECTX_SRC
    "${DIRECTX_INCLUDES_PATH}/*.cpp"
    )


#Merge all

list (APPEND SOURCES ${SOURCES_LIBBSARCH})
list (APPEND SOURCES ${SOURCES_HAVOK})
list (APPEND SOURCES ${SOURCES_CK-CMD})
list (APPEND SOURCES ${HAVOK_COMPAT_SRC})
list (APPEND SOURCES ${NIFLIB_SRC})
list (APPEND SOURCES ${DIRECTX_SRC})

list (APPEND HEADERS ${HEADERS_PLOG})
list (APPEND HEADERS ${HEADERS_LIBBSARCH})
list (APPEND HEADERS ${HEADERS_HAVOK})
list (APPEND HEADERS ${HEADERS_CK-CMD})
list (APPEND HEADERS ${HAVOK_COMPAT_HEADERS})
list (APPEND HEADERS ${NIFLIB_HEADERS})
list (APPEND HEADERS ${DIRECTX_HEADERS})

list (APPEND LIBS Qt5::Core Qt5::Widgets)
list (APPEND LIBS ${PROJECT_SOURCE_DIR}/libs/libbsarch/libbsarch.lib)
list (APPEND LIBS ${HAVOK_LIBS})
list (APPEND LIBS Shlwapi.lib legacy_stdio_definitions.lib)

source_group("Havok Compat\\Headers" FILES ${HAVOK_COMPAT_HEADERS})
source_group("Havok Compat\\Sources" FILES ${HAVOK_COMPAT_SRC})
source_group("Header Files" FILES ${HEADERS})

#MSVC

IF (MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj /D_SCL_SECURE_NO_WARNINGS=1 /D_CRT_NONSTDC_NO_DEPRECATE=1 /EHsc")
add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
# Update compiler flags.
IF (PROJECT_STATIC_RUNTIME)
FOREACH(flag
    CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_RELWITHDEBINFO
    CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_DEBUG_INIT
    CMAKE_CXX_FLAGS_RELEASE  CMAKE_CXX_FLAGS_RELWITHDEBINFO
    CMAKE_CXX_FLAGS_DEBUG  CMAKE_CXX_FLAGS_DEBUG_INIT)
STRING(REPLACE "/MD"  "/MT" "${flag}" "${${flag}}")
ENDFOREACH()
ENDIF ()

ENDIF ()

#Build

add_executable(Cathedral_Assets_Optimizer WIN32 ${SOURCES} ${HEADERS} ${QON_QM_FILES} ${UI} ${ICON_RC_PATH} ${QRC_PATH})
target_link_libraries(Cathedral_Assets_Optimizer ${LIBS})

#Cotire (PCH)
set_target_properties(Cathedral_Assets_Optimizer PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "pch.h")
cotire(Cathedral_Assets_Optimizer)
