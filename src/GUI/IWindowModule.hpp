/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Settings/BaseTypes.hpp"
#include "Utils/ConnectionWrapper.hpp"

namespace CAO {

class PatternSettings;
class GeneralSettings;

class IWindowModule : public QWidget, public ConnectionWrapper
{
    Q_OBJECT

public:
    explicit IWindowModule(QWidget *parent);

    void setup(PatternSettings &pSets, GeneralSettings &gSets);
    virtual QString name() = 0;

private:
    virtual void setUIData(const PatternSettings &pSets, const GeneralSettings &gSets);

    virtual void connectAll(PatternSettings &pSets, GeneralSettings &gSets) = 0;

    virtual bool isSupportedGame(Games game) = 0;
};
} // namespace CAO
