#-------------------------------------------------
#
# Project created by QtCreator 2019-01-25T21:26:26
#
#-------------------------------------------------

QT       -= widgets

TARGET = Cathedral_Assets_Optimizer_back
TEMPLATE = app

CONFIG += console

SOURCES += \
    $$files(libbsarch/*.cpp) \
    main.cpp \
    QLogger/QLogger.cpp \
    TexturesOptimizer.cpp \
    MeshesOptimizer.cpp \
    BsaOptimizer.cpp \
    FilesystemOperations.cpp \
    PluginsOperations.cpp \
    AnimationsOptimizer.cpp \
    MainOptimizer.cpp \
    IntegrationTests.cpp \


HEADERS += \
    $$files(libbsarch/*.h) \
    mainwindow.h \
    QLogger/QLogger.h \
    TexturesOptimizer.h \
    MeshesOptimizer.h \
    BsaOptimizer.h \
    FilesystemOperations.h \
    PluginsOperations.h \
    AnimationsOptimizer.h \
    MainOptimizer.h \
    IntegrationTests.h \
    pch.h


PRECOMPILED_HEADER += pch.h

LIBS += -L$$PWD/libbsarch/ -llibbsarch

INCLUDEPATH += $$PWD/libbsarch
DEPENDPATH += $$PWD/libbsarch

CONFIG += c++17 precompile_header static suppress_vcproj_warnings

TRANSLATIONS += \
translations/AssetsOpt_en.ts \
translations/AssetsOpt_fr.ts \
translations/AssetsOpt_ja.ts \
translations/AssetsOpt_de.ts
