
/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Settings/Profiles.hpp"
#include "SettingsList.hpp"

#ifdef GUI
#include "ui_BSAFilesToPack.h"
#include "ui_mainWindow.h"
#endif

namespace CAO {
class Settings final
{
    Q_GADGET
public:
    void parseArguments(const QStringList &args);

    template<typename T>
    T getValue(const Setting &set) const
    {
        return _json.getValue<T>(set.jsonKey);
    }

    template<typename T>
    void setValue(const Setting &set, const T &value)
    {
        _json.setValue(set.jsonKey, value);
    }

    void saveToJSON(const QString &filepath) const;
    void readFromJSON(const QString &filepath);
#ifdef GUI
    void saveToUi(Ui::MainWindow &ui, Ui::BSAFilesToPack &bsUi);
    void readFromUi(Ui::MainWindow &ui, Ui::BSAFilesToPack &bsUi);
#endif

    /*!
   * \brief Checks if the current settings are allowed
   */
    QString isValid() const;

protected:
    JSON _json;
};
} // namespace CAO
