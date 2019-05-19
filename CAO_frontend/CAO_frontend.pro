#-------------------------------------------------
#
# Project created by QtCreator 2019-01-25T21:26:26
#
#-------------------------------------------------

QT       += widgets

TARGET = Cathedral_Assets_Optimizer_manager
TEMPLATE = app

SOURCES += \
    SkyrimSE.cpp \
    main.cpp


HEADERS += \
    SkyrimSE.h \
    pch.h

FORMS +=  \
    SkyrimSE.ui

RESOURCES += styles/qdarkstyle/style.qrc
RC_FILE = Cathedral_Assets_Optimizer.rc

PRECOMPILED_HEADER += pch.h

CONFIG += c++17 precompile_header static suppress_vcproj_warnings

TRANSLATIONS += \
translations/AssetsOpt_en.ts \
translations/AssetsOpt_fr.ts \
translations/AssetsOpt_ja.ts \
translations/AssetsOpt_de.ts
