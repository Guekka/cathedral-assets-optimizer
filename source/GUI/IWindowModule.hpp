/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Settings/Utils/QValueWrapper.hpp"
#include "pch.hpp"
#include <functional>

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
    virtual void disconnectAll()                                  = 0;

protected:
    template<typename UiElement, typename UiFunc, typename Wrapper>
    void connectWrapper(UiElement &uiEl, UiFunc &&uiFunc, Wrapper &wrapper)
    {
        connect(&uiEl, uiFunc, &wrapper, &std::remove_reference_t<decltype(wrapper)>::setValue);
    }

    template<typename Wrapper>
    void connectWrapper(QCheckBox &uiEl, Wrapper &wrapper)
    {
        connectWrapper(uiEl, &QCheckBox::toggled, wrapper);
    }

    template<typename... UiElements>
    void setEnabled(bool state, UiElements &... uiEls)
    {
        (std::invoke(&QWidget::setEnabled, uiEls, state), ...);
    }
};
} // namespace CAO
