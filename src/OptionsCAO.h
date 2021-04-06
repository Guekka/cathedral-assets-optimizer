/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Profiles.h"
#include "pch.h"
#include "ui_mainWindow.h"

/*
namespace plog
{
    Record& operator<<(Record& record, const OptionsCAO& opt)
    {

    }
}*/

class OptionsCAO final : public QObject
{
public:
    void parseArguments(const QStringList &args);

    void saveToIni(QSettings *settings);
    void readFromIni(QSettings *settings);
#ifdef GUI
    void saveToUi(Ui::MainWindow *ui);
    void readFromUi(Ui::MainWindow *ui);
#endif

    /*!
   * \brief Checks if the current settings are allowed
   */
    QString isValid() const;

    /*--------------VARS-------------------*/
    bool bBsaExtract = false;
    bool bBsaCreate = false;
    bool bBsaDeleteBackup = false;
    bool bBsaLeastBSA = false;
    bool bBsaProcessContent = false;
    bool bBsaCreateDummies = true;

    bool bAnimationsOptimization = false;

    bool bDryRun = false;

    int iMeshesOptimizationLevel = 0;

    bool bMeshesHeadparts = true;
    bool bMeshesResave = false;

    bool bTexturesNecessary = true;
    bool bTexturesCompress = false;
    bool bTexturesMipmaps = false;

    bool bTexturesResizeSize = false;
    size_t iTexturesTargetHeight = 2048;
    size_t iTexturesTargetWidth = 2048;
    bool bTexturesResizeRatio = false;
    uint iTexturesTargetWidthRatio = 1;
    uint iTexturesTargetHeightRatio = 1;

    bool bDebugLog = false;

    /*!
   * \brief The optimization mode
   */
    enum OptimizationMode
    {
        SingleMod = 0,
        SeveralMods = 1
    } mode;

    Q_ENUM(OptimizationMode)

    /*!
    * \brief The path given by the user
    */
    QString userPath;
    /*-----------END OF VARS---------------*/
};
