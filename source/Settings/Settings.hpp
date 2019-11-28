/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Settings/AdvancedSettings.hpp"
#include "Settings/Profiles.hpp"
#include "Settings/StandardSettings.hpp"
#include "pch.hpp"
#ifdef GUI
#include "ui_mainWindow.h"
#endif

namespace CAO {
class Settings final
{
    using json = nlohmann::json;
    Q_GADGET
public:
    Settings();
    Settings(const Settings &other) = default;

    template<class T>
    T getMandatoryValue(const StandardSettings::StandardKey &key) const
    {
        return StandardSettings::toJSONKey(key, _json).get<T>();
    }

    template<>
    QString getMandatoryValue(const StandardSettings::StandardKey &key) const
    {
        auto value = StandardSettings::toJSONKey(key, _json);
        return QString::fromStdString(value.get<std::string>());
    }

    template<class T>
    T getMandatoryValue(const AdvancedSettings::AdvancedKey &key) const
    {
        return AdvancedSettings::toJSONKey(key, _json).get<T>();
    }

    template<>
    QString getMandatoryValue(const AdvancedSettings::AdvancedKey &key) const
    {
        auto value = AdvancedSettings::toJSONKey(key, _json);
        return QString::fromStdString(value.get<std::string>());
    }

    template<class T>
    T getOptionalValue(const QString &key) const
    {
        return splitKey(key).get<T>();
    }

    template<>
    QString getOptionalValue(const QString &key) const
    {
        auto value = splitKey(key);
        return QString::fromStdString(value.get<std::string>());
    }

    template<class T>
    void setValue(const QString &key, const T &value)
    {
        splitKey(key) = value;
    }

    template<>
    void setValue(const QString &key, const QString &value)
    {
        auto &jKey = splitKey(key);
        jKey = value.toStdString();
    }

    template<class T>
    void setValue(const StandardSettings::StandardKey &key, const T &value)
    {
        StandardSettings::toJSONKey(key, _json) = value;
    }

    template<>
    void setValue(const StandardSettings::StandardKey &key, const QString &value)
    {
        StandardSettings::toJSONKey(key, _json) = value.toStdString();
    }

    template<class T>
    void setValue(const AdvancedSettings::AdvancedKey &key, const T &value)
    {
        AdvancedSettings::toJSONKey(key, _json) = value;
    }

    template<>
    void setValue(const AdvancedSettings::AdvancedKey &key, const QString &value)
    {
        AdvancedSettings::toJSONKey(key, _json) = value.toStdString();
    }

    void parseArguments(const QStringList &args);

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

protected:
    json _json;

    json &splitKeySafe(const QString &key);
    const json &splitKeySafe(const QString &key) const;

    json &splitKey(const QString &key);
    const json &splitKey(const QString &key) const;
};
} // namespace CAO
