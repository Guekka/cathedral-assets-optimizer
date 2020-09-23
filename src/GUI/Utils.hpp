/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <QAbstractButton>
#include <QAction>
#include <QComboBox>
#include <QDialog>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QListView>
#include <QSpinBox>
#include <QStandardItemModel>

#include <functional>
#include <stdexcept>

#include "Utils/TemplateMetaProgramming.hpp"

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
auto connectWrapper(UiElement &uiEl,
                    Wrapper &wrapper,
                    UiReadFunc &&readFunc,
                    UiSaveFunc &&saveFunc,
                    ValueType fallback = ValueType{})
{
    using wrapperType = std::remove_reference_t<decltype(wrapper)>;

    //Init data
    std::invoke(std::forward<UiSaveFunc>(saveFunc), uiEl, wrapper.value_or(fallback));
    //Connect

    const auto lambda = [&wrapper,
                         &uiEl,
                         readFunc = std::forward<UiReadFunc>(readFunc),
                         saveFunc = std::forward<UiSaveFunc>(saveFunc)](ValueType value) {
        wrapper = value;
        std::invoke(saveFunc, uiEl, wrapper());
    };

    return QObject::connect(&uiEl, std::forward<UiReadFunc>(readFunc), &wrapper, lambda);
}

template<typename Wrapper, typename ValueType = typename Wrapper::Type>
auto connectWrapper(QAbstractButton &uiEl, Wrapper &wrapper, ValueType fallback = ValueType{})
{
    return connectWrapper(uiEl, wrapper, &QAbstractButton::toggled, &QAbstractButton::setChecked, fallback);
}

template<typename Wrapper, typename ValueType = typename Wrapper::Type>
auto connectWrapper(QSpinBox &uiEl, Wrapper &wrapper, ValueType fallback = ValueType{})
{
    return connectWrapper(uiEl,
                          wrapper,
                          QOverload<int>::of(&QSpinBox::valueChanged),
                          &QSpinBox::setValue,
                          fallback);
}

template<typename Wrapper, typename ValueType = typename Wrapper::Type>
auto connectWrapper(QDoubleSpinBox &uiEl, Wrapper &wrapper, ValueType fallback = ValueType{})
{
    return connectWrapper(uiEl,
                          wrapper,
                          QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                          &QDoubleSpinBox::setValue,
                          fallback);
}

template<typename Wrapper, typename ValueType = typename Wrapper::Type>
auto connectWrapper(QLineEdit &uiEl, Wrapper &wrapper, ValueType fallback = ValueType{})
{
    return connectWrapper(uiEl, wrapper, &QLineEdit::textChanged, &QLineEdit::setText, fallback);
}

template<typename Wrapper, typename ValueType = typename Wrapper::Type>
auto connectWrapper(QAction &uiEl, Wrapper &wrapper, ValueType fallback = ValueType{})
{
    return connectWrapper(uiEl, wrapper, &QAction::triggered, &QAction::setChecked, fallback);
}

template<typename... UiElements>
void connectGroupBox(QGroupBox *box, UiElements *... uiEls)
{
    struct Functor
    {
        void operator()(QGroupBox *box, QAbstractButton *uiEl)
        {
            QObject::connect(box, &QGroupBox::toggled, uiEl, &QWidget::setEnabled);
            QObject::connect(box, &QGroupBox::toggled, uiEl, &QAbstractButton::setChecked);
        }
        void operator()(QGroupBox *box, QWidget *uiEl)
        {
            QObject::connect(box, &QGroupBox::toggled, uiEl, &QWidget::setEnabled);
        }
    } functor;

    (std::invoke(functor, box, uiEls), ...);
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
void setData(QComboBox &box, const QString &text, const Data &data)
{
    const auto pos = box.findText(text, Qt::MatchFlag::MatchExactly);
    if (pos == -1)
        throw UiException(QString("UI was different than expected. Element '%1' not found").arg(text));
    box.setItemData(pos, data);
}

template<typename Data>
int findData(const QComboBox &box, const Data &data)
{
    for (int i = 0; i < box.count(); ++i)
        if (box.itemData(i).value<Data>() == data)
            return i;
    return -1;
};

template<typename Data>
bool selectData(QComboBox &box, const Data &data)
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
