/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Utils/TemplateMetaProgramming.hpp"
#include "pch.hpp"
#include <functional>
#include <QStandardItemModel>

namespace CAO {
class UiException : public std::runtime_error
{
public:
    explicit UiException(const QString &str)
        : std::runtime_error(str.toStdString())
    {
    }
};

template<typename UiElement,
         typename Wrapper,
         typename UiReadFunc,
         typename UiSaveFunc,
         typename ValueType = typename Wrapper::Type>
inline void connectWrapper(UiElement &uiEl,
                           Wrapper &wrapper,
                           UiReadFunc &&readFunc,
                           UiSaveFunc &&saveFunc,
                           ValueType fallback = ValueType{})
{
    using wrapperType = std::remove_reference_t<decltype(wrapper)>;

    //Init data
    std::invoke(std::forward<UiSaveFunc>(saveFunc), uiEl, wrapper.value_or(fallback));
    //Connect
    QObject::connect(&uiEl, std::forward<UiReadFunc>(readFunc), &wrapper, &wrapperType::setValue);
    QObject::connect(&wrapper,
                     &wrapperType::valueChanged,
                     &uiEl,
                     [&wrapper, &uiEl, saveFunc = std::forward<UiSaveFunc>(saveFunc)] {
                         std::invoke(saveFunc, uiEl, wrapper());
                     });
}

template<typename Wrapper, typename ValueType = typename Wrapper::Type>
inline void connectWrapper(QAbstractButton &uiEl, Wrapper &wrapper, ValueType fallback = ValueType{})
{
    connectWrapper(uiEl, wrapper, &QAbstractButton::toggled, &QAbstractButton::setChecked, fallback);
}

template<typename Wrapper, typename ValueType = typename Wrapper::Type>
inline void connectWrapper(QSpinBox &uiEl, Wrapper &wrapper, ValueType fallback = ValueType{})
{
    connectWrapper(uiEl, wrapper, QOverload<int>::of(&QSpinBox::valueChanged), &QSpinBox::setValue, fallback);
}

template<typename Wrapper, typename ValueType = typename Wrapper::Type>
inline void connectWrapper(QLineEdit &uiEl, Wrapper &wrapper, ValueType fallback = ValueType{})
{
    connectWrapper(uiEl, wrapper, &QLineEdit::textChanged, &QLineEdit::setText, fallback);
}

template<typename Wrapper, typename ValueType = typename Wrapper::Type>
inline void connectWrapper(QAction &uiEl, Wrapper &wrapper, ValueType fallback = ValueType{})
{
    connectWrapper(uiEl, wrapper, &QAction::triggered, &QAction::setChecked, fallback);
}

inline void connectGroupBox(QGroupBox *box, QWidget *uiEl)
{
    QObject::connect(box, &QGroupBox::toggled, uiEl, &QWidget::setEnabled);
}

inline void connectGroupBox(QGroupBox *box, QAbstractButton *uiEl)
{
    QObject::connect(box, &QGroupBox::toggled, uiEl, &QWidget::setEnabled);
    QObject::connect(box, &QGroupBox::toggled, uiEl, &QAbstractButton::setChecked);
}

template<typename... UiElements>
inline void connectGroupBox(QGroupBox *box, UiElements *... uiEls)
{
    (std::invoke([](QGroupBox *b, QWidget *w) { connectGroupBox(b, w); }, box, uiEls), ...);
}

template<typename... UiElements>
inline void setEnabled(bool state, UiElements &... uiEls)
{
    (std::invoke(&QWidget::setEnabled, uiEls, state), ...);
}

template<typename... UiElements>
inline void setEnabled(bool state, UiElements *... uiEls)
{
    (std::invoke(&QWidget::setEnabled, uiEls, state), ...);
}

template<typename Data>
inline void setData(QComboBox &box, const QString &text, const Data &data)
{
    const auto pos = box.findText(text, Qt::MatchFlag::MatchExactly);
    if (pos == -1)
        throw UiException(QString("UI was different than expected. Element '%1' not found").arg(text));
    box.setItemData(pos, data);
}

template<typename Data>
inline int findData(const QComboBox &box, const Data &data)
{
    for (int i = 0; i < box.count(); ++i)
        if (box.itemData(i).value<Data>() == data)
            return i;
    return -1;
};

template<typename Data>
inline bool selectData(QComboBox &box, const Data &data)
{
    int idx = box.findData(data);
    if (idx == -1)
        return false;

    box.setCurrentIndex(idx);
    return true;
}

inline bool selectText(QComboBox &box, const QString &text)
{
    int idx = box.findText(text);
    if (idx == -1)
        return false;

    box.setCurrentIndex(idx);
    return true;
}

inline void setItemEnabled(QComboBox &box, int idx, bool state)
{
    auto *model = qobject_cast<QStandardItemModel *>(box.model());
    auto *view  = qobject_cast<QListView *>(box.view());

    if (!model || !view)
        throw UiException("Couldn't get model / view of QComboBox");

    view->setRowHidden(idx, !state);
    QStandardItem *item = model->item(idx);

    const auto flags = state ? item->flags() & ~Qt::ItemIsEnabled : item->flags() | Qt::ItemIsEnabled;
    item->setFlags(flags);
}

} // namespace CAO
