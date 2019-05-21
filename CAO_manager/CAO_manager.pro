QT -= gui

TARGET = Cathedral_Assets_Optimizer_manager
TEMPLATE = app

CONFIG += c++17 precompile_header console static suppress_vcproj_warnings

TRANSLATIONS += \
translations/AssetsOpt_en.ts \
translations/AssetsOpt_fr.ts \
translations/AssetsOpt_ja.ts \
translations/AssetsOpt_de.ts

CONFIG -= app_bundle

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        MainManager.cpp \
        ProcessManager.cpp \
        main.cpp

HEADERS += \
    MainManager.h \
    ProcessManager.h
