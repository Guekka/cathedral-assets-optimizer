/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.h"
#include "Games.h"
#include "../CAO_frontend/Custom.h"

struct OptionsCAO : public QObject
{
    OptionsCAO();
    OptionsCAO(const OptionsCAO& other);

    void parseArguments(const QStringList& args);

    void saveToIni(QSettings *settings);
    void readFromIni(QSettings *settings);

    void saveToUi(Ui::Custom* ui);
    void readFromUi(Ui::Custom* ui);

    /*!
     * \brief Checks if the current settings are allowed
     */
    bool isValid();

    /*--------------VARS-------------------*/
    bool bBsaExtract = false;
    bool bBsaCreate = false;
    bool bBsaDeleteBackup = false;
    bool bBsaProcessContent = false;

    bool bAnimationsOptimization = false;

    bool bDryRun = false;

    bool bMeshesHeadparts = false;
    int iMeshesOptimizationLevel = 0;
    int iTexturesOptimizationLevel = 0;

    int iLogLevel;
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

Q_DECLARE_METATYPE(OptionsCAO)

/*
namespace plog
{
    Record& operator<<(Record& record, const OptionsCAO& opt)
    {

    }
}*/

