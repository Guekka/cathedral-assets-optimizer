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

namespace cao {
class UiException : public std::runtime_error
{
public:
    explicit UiException(const QString &str)
        : std::runtime_error(str.toStdString())
    {
    }
};

/**
 * @brief Connects a QGroupBox to multiple UiElements
 *
 * This function connects a QGroupBox to multiple UiElements, allowing for easy manipulation of the group box properties.
 * The UiElements can be any QWidget, but also QAbstractButton, in which case the button will be unchecked when the group
 * box is unchecked.
 *
 * @param box The QGroupBox to be connected
 * @param ui_els The UiElements (variadic) to be connected to the group box
 */
template<typename... UiElements>
void connect_group_box(QGroupBox *box, UiElements *...ui_els)
{
    struct Functor
    {
        void operator()(QGroupBox *box, QAbstractButton *ui_el)
        {
            operator()(box, static_cast<QWidget *>(ui_el));
            QObject::connect(box, &QGroupBox::toggled, ui_el, [ui_el](bool checked) {
                if (!checked)
                    ui_el->setChecked(false);
            });
        }

        void operator()(QGroupBox *box, QWidget *ui_el)
        {
            QObject::connect(box, &QGroupBox::toggled, ui_el, &QWidget::setEnabled);
        }
    } functor;

    (std::invoke(functor, box, ui_els), ...);
}

template<typename Data>
void set_data(QComboBox &box, const QString &text, const Data &data)
{
    const auto pos = box.findText(text, Qt::MatchFlag::MatchExactly);
    if (pos == -1)
        throw UiException(QString("UI was different than expected. Element '%1' not found").arg(text));
    box.setItemData(pos, QVariant::fromValue(data));
}

template<typename Data>
int find_data(const QComboBox &box, const Data &data)
{
    for (int i = 0; i < box.count(); ++i)
        if (box.itemData(i).value<Data>() == data)
            return i;
    return -1;
};

template<typename Data>
bool select_data(QComboBox &box, const Data &data)
{
    int idx = box.findData(QVariant::fromValue(data));
    if (idx == -1)
        return false;

    box.setCurrentIndex(idx);
    return true;
}

[[nodiscard]] inline auto to_qstring(std::u8string_view u) -> QString
{
    return QString::fromUtf8(reinterpret_cast<const char *>(u.data()), u.size());
}

[[nodiscard]] inline auto to_u8string(QString u) -> std::u8string
{
    auto data = u.toUtf8();
    return {reinterpret_cast<const char8_t *>(data.constData()), static_cast<size_t>(data.size())};
}

} // namespace cao
