#-------------------------------------------------
#
# Project created by QtCreator 2019-01-25T21:26:26
#
#-------------------------------------------------

QT       += widgets

TARGET = Bethesda_Assets_Optimizer
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

CONFIG += c++11 precompile_header


SOURCES += \
    main.cpp \
    Mainwindow.cpp \
    QLogger.cpp \
    TexturesOptimizer.cpp \
    MeshesOptimizer.cpp \
    BsaOptimizer.cpp \
    FilesystemOperations.cpp \
    PluginsOperations.cpp \
    AnimationsOptimizer.cpp \
    MainOptimizer.cpp

HEADERS += \
    pch.h \
    Mainwindow.h \
    QLogger.h \
    TexturesOptimizer.h \
    MeshesOptimizer.h \
    BsaOptimizer.h \
    FilesystemOperations.h \
    PluginsOperations.h \
    AnimationsOptimizer.h \
    MainOptimizer.h

PRECOMPILED_HEADER = pch.h

TRANSLATIONS += \
translations/AssetsOpt_fr.ts \
translations/AssetsOpt_ja.ts

FORMS += \
    mainwindow.ui \

RESOURCES += styles/qdarkstyle/style.qrc

docs.depends = $(SOURCES)
docs.commands = "E:\Edgar\Documents\Perso\Informatique\Code\Doxygen\doxygen.exe" "E:\Edgar\Documents\Perso\Informatique\Code\Qt Creator\SSE_Assets_Optimizer\doc\Doxyfile"

QMAKE_EXTRA_TARGETS = docs
