#-------------------------------------------------
#
# Project created by QtCreator 2019-01-25T21:26:26
#
#-------------------------------------------------

VERSION = 4.2.5

QT       += widgets

TARGET = Cathedral_Assets_Optimizer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17 precompile_header static suppress_vcproj_warnings

SOURCES += \
    ../../QLibbsarch/BSArchive.cpp \
    ../../QLibbsarch/BSArchiveAuto.cpp \
    ../../QLibbsarch/BSArchiveEntries.cpp \
    ../../QLibbsarch/QLibbsarch.cpp \
    ModGroups.cpp \
    main.cpp \
    mainwindow.cpp \
    QLogger/QLogger.cpp \
    TexturesOptimizer.cpp \
    MeshesOptimizer.cpp \
    BsaOptimizer.cpp \
    FilesystemOperations.cpp \
    PluginsOperations.cpp \
    AnimationsOptimizer.cpp \
    MainOptimizer.cpp \
    IntegrationTests.cpp


HEADERS += \
    ../../QLibbsarch/BSArchive.h \
    ../../QLibbsarch/BSArchiveAuto.h \
    ../../QLibbsarch/BSArchiveEntries.h \
    ../../QLibbsarch/QLibbsarch.h \
    ../../QLibbsarch/libbsarch.h \
    ModGroups.h \
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

TRANSLATIONS += \
translations/AssetsOpt_fr.ts \
translations/AssetsOpt_ja.ts \
translations/AssetsOpt_de.ts

FORMS +=  mainwindow.ui

RESOURCES += styles/qdarkstyle/style.qrc

LIBS += -L$$PWD/../../QLibbsarch/ -llibbsarch

INCLUDEPATH += $$PWD/../../QLibbsarch
DEPENDPATH += $$PWD/../../QLibbsarch
