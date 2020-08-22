/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.hpp"

namespace CAO {

class PatternSettings;
class GeneralSettings;

class IWindowModule : public QWidget
{
    Q_OBJECT

public:
    explicit IWindowModule(QWidget *parent)
        : QWidget(parent)
    {
    }

    virtual void connectAll(PatternSettings &, GeneralSettings &) = 0;
    virtual void disconnectAll()
    {
        for (auto *obj : this->findChildren<QObject *>())
            obj->disconnect();
    }

    virtual QString name() = 0;
};
} // namespace CAO
