/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.h"
#include "Games.h"
#include "ui_mainWindow.h"

/*
namespace plog
{
    Record& operator<<(Record& record, const OptionsCAO& opt)
    {

    }
}*/


class OptionsCAO : public QObject
{
public:
    OptionsCAO();
    OptionsCAO(const OptionsCAO& other);

    void parseArguments(const QStringList& args);

    void saveToIni(QSettings *settings);
    void readFromIni(QSettings *settings);
#ifdef GUI
    void saveToUi(Ui::MainWindow* ui);
    void readFromUi(Ui::MainWindow* ui);
#endif

    /*!
     * \brief Checks if the current settings are allowed
     */
    QString isValid();

    /*--------------VARS-------------------*/
    bool bBsaExtract = false;
    bool bBsaCreate = false;
    bool bBsaDeleteBackup = false;
    bool bBsaProcessContent = false;

    bool bAnimationsOptimization = false;

    bool bDryRun = false;

    bool bMeshesHeadparts = false;
    bool bMeshesResave = true;

    int iMeshesOptimizationLevel = 0;

    int iTexturesOptimizationLevel = 0;

    bool bTexturesResizeSize = false;
    size_t iTexturesTargetHeight = 2048;
    size_t iTexturesTargetWidth = 2048;
    bool bTexturesResizeRatio = false;
    uint iTexturesTargetWidthRatio = 1;
    uint iTexturesTargetHeightRatio = 1;

    int iLogLevel = 4;
    /*!
     * \brief The optimization mode
     */
    enum OptimizationMode { singleMod = 0, severalMods = 1} mode;
    Q_ENUM(OptimizationMode)

    /*!
      * \brief The path given by the user
      */
    QString userPath;
    /*-----------END OF VARS---------------*/
};

