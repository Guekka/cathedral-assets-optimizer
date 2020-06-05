/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.hpp"
#include <functional>

namespace CAO {
class UiException : public std::runtime_error
{
public:
    explicit UiException(const QString &str)
        : std::runtime_error(str.toStdString())
    {
    }
};

template<typename UiElement, typename Wrapper, typename UiReadFunc, typename UiSaveFunc>
void connectWrapper(UiElement &uiEl, Wrapper &wrapper, UiReadFunc &&readFunc, UiSaveFunc &&saveFunc)
{
    using wrapperType = std::remove_reference_t<decltype(wrapper)>;

    //Init data
    std::invoke(saveFunc, uiEl, wrapper());
    //Connect
    QObject::connect(&uiEl, readFunc, &wrapper, &wrapperType::setValue);
    QObject::connect(&wrapper, &wrapperType::valueChanged, &uiEl, [&wrapper, &uiEl, &saveFunc] {
        std::invoke(saveFunc, uiEl, wrapper());
    });
}

template<typename Wrapper>
void connectWrapper(QAbstractButton &uiEl, Wrapper &wrapper)
{
    connectWrapper(uiEl, wrapper, &QAbstractButton::toggled, &QAbstractButton::setChecked);
}

template<typename Wrapper>
void connectWrapper(QSpinBox &uiEl, Wrapper &wrapper)
{
    connectWrapper(uiEl, wrapper, QOverload<int>::of(&QSpinBox::valueChanged), &QSpinBox::setValue);
}

template<typename... UiElements>
void connectGroupBox(QGroupBox &box, UiElements &... uiEls)
{
    connect(box, &QGroupBox::toggled, uiEls..., &QWidget::setEnabled);
}

template<typename... UiElements>
void setEnabled(bool state, UiElements &... uiEls)
{
    (std::invoke(&QWidget::setEnabled, uiEls, state), ...);
}

template<typename... UiElements>
void setEnabled(bool state, UiElements *... uiEls)
{
    (std::invoke(&QWidget::setEnabled, uiEls, state), ...);
}

template<typename Data>
void setData(QComboBox &box, const QString &text, Data &&data)
{
    auto pos = box.findText(text, Qt::MatchFlag::MatchExactly);
    if (pos == -1)
        throw UiException(QString("UI was different than expected. Element '%1' not found").arg(text));
    box.setItemData(pos, data);
}

static bool selectText(QComboBox &box, const QString &text)
{
    int idx = box.findText(text);
    if (idx == -1)
        return false;

    box.setCurrentIndex(idx);
    return true;
}

} // namespace CAO
