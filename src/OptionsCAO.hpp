/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Profiles.hpp"
#include "pch.hpp"
#ifdef GUI
#include "GUI/ui_mainWindow.h"
#endif

namespace CAO {
class OptionsCAO final
{
    Q_GADGET
public:
    OptionsCAO();
    OptionsCAO(const OptionsCAO &other) = default;

    enum StandardKeys
    {
        bDebugLog,
        bDryRun,
        eMode,
        sUserPath,

        bBsaExtract,
        bBsaCreate,
        bBsaDeleteBackup,
        bBsaProcessContent,

        bTexturesNecessary,
        bTexturesCompress,
        bTexturesMipmaps,

        bTexturesResizeSize,
        iTexturesTargetHeight,
        iTexturesTargetWidth,

        bTexturesResizeRatio,
        iTexturesTargetWidthRatio,
        iTexturesTargetHeightRatio,

        iMeshesOptimizationLevel,

        bMeshesHeadparts,
        bMeshesResave,

        bAnimationsOptimization,
    };

    void parseArguments(const QStringList &args);

    template<class T>
    T getMandatoryValue(const QString &key) const;
    template<class T>
    T getMandatoryValue(const StandardKeys &key) const;

    template<class T>
    T getOptionalValue(const QString &key) const;

    template<class T>
    void setValue(const QString &key, const T &value);

    template<class T>
    void setValue(const StandardKeys &key, const T &value);

    void saveToJSON(const QString &filepath) const;
    void readFromJSON(const QString &filepath);
#ifdef GUI
    void saveToUi(Ui::MainWindow *ui);
    void readFromUi(Ui::MainWindow *ui);
#endif

    /*!
   * \brief Checks if the current settings are allowed
   */
    QString isValid() const;

    enum OptimizationMode
    {
        SingleMod = 0,
        SeveralMods = 1
    } mode;
    Q_ENUM(OptimizationMode)

protected:
    json _json;
    json &enumToKey(const StandardKeys &sKey);
    const json &enumToKey(const StandardKeys &sKey) const;

    json &splitKeySafe(const QString &key);
    const json &splitKeySafe(const QString &key) const;

    json &splitKey(const QString &key);
    const json &splitKey(const QString &key) const;

private:
    QMutex *mutex;
};
} // namespace CAO
