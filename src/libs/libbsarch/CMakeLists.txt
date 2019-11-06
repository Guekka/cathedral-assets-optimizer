Project(qlibbsarch)

add_library(qlibbsarch STATIC
    BSArchive.cpp
    BSArchive.h
    BSArchiveAuto.cpp
    BSArchiveAuto.h
    BSArchiveEntries.cpp
    BSArchiveEntries.h
    libbsarch.h
    QLibbsarch.h)

target_link_directories(qlibbsarch PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(qlibbsarch Qt5::Core libbsarch)
target_compile_features(qlibbsarch PUBLIC cxx_std_17)
