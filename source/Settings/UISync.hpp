/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "BaseTypes.hpp"
#include "pch.hpp"

//Here we are implementing functions for reading different UI widgets

namespace CAO {
class MainWindow;

class UISync final
{
public:
    QString key_; //Necessary to provide equality operator

    //Actual class content
    using uiRead = std::function<void(const MainWindow &window, nlohmann::json &json)>;
    using uiSave = std::function<void(MainWindow &window, const nlohmann::json &json)>;

    UISync(QString key, uiRead r, uiSave s)
        : key_(std::move(key))
        , read(std::move(r))
        , save(std::move(s))
    {}

    UISync() = default;

    const uiRead read;
    const uiSave save;

    bool operator==(const UISync &rh) const { return rh.key_ == this->key_; }
    bool operator!=(const UISync &rh) const { return !(*this == rh); }

    //All the functions to read and save the UI are declared below
    static bool readCheckbox(const QCheckBox *widget);
    static bool readCheckbox(const QAction *widget);
    static bool readCheckbox(const QRadioButton *widget, const QGroupBox *parent);
    static bool readCheckbox(const QGroupBox *widget);

    static int readMeshesOptLevel(const MainWindow &window);

    static bool readTab(const QWidget *widget);
    static int readValue(const QSpinBox *widget);
    static double readGigaByteValue(const QDoubleSpinBox *widget);

    static QString readText(const QLineEdit *widget);
    static QStringList readText(const QPlainTextEdit *widget);

    template<typename returnValue>
    static returnValue readValue(const QComboBox *widget)
    {
        return widget->currentData().value<returnValue>();
    }

    //SFINAE for container only
    template<typename Container, typename StoredType = typename Container::value_type>
    static Container readList(const QListWidget *widget)
    {
        QVector<StoredType> data;
        for (int i = 0; i < widget->count(); ++i)
        {
            const auto &entry = widget->item(i);
            const auto &value = entry->data(Qt::UserRole).value<StoredType>();
            if (!data.contains(value))
                data += value;
        }
        return data.toStdVector();
    }

    template<>
    static QStringList readList(const QListWidget *widget)
    {
        QStringList data;
        for (int i = 0; i < widget->count(); ++i)
        {
            const auto &entry = widget->item(i);
            const QString &text = entry->text();
            if (!data.contains(text))
                data.push_back(text);
        }
        return data;
    }

    template<typename T>
    static T nullReadFunc([[maybe_unused]] T value)
    {
        return value;
    }

    static void saveCheckbox(QCheckBox *widget, const bool &value);
    static void saveCheckbox(QRadioButton *widget, const bool &value);
    static void saveCheckbox(QAction *widget, const bool &value);
    static void saveCheckbox(QGroupBox *widget, const bool &value);

    static void saveMeshesOptLevel(MainWindow &window, const int &value);

    static void saveTab(QWidget *widget, const bool &value) { widget->setEnabled(value); }
    static void saveValue(QSpinBox *widget, const int &value) { widget->setValue(value); }
    static void saveValueGigabyte(QDoubleSpinBox *widget, const double &value) { widget->setValue(value / GigaByte); }

    static void saveText(QLineEdit *widget, const QString &value) { widget->setText(value); }
    static void saveText(QPlainTextEdit *widget, const QStringList &value)
    {
        widget->setPlainText(value.join('\n'));
    }

    template<typename Type>
    static void saveValue(QComboBox *widget, const Type &value)
    {
        for (int i = 0; i < widget->count(); ++i)
        {
            if (widget->itemData(i).value<Type>() == value)
            {
                widget->setCurrentIndex(i);
                return;
            }
        }
    }

    template<typename Type>
    static void saveList(QListWidget *widget, const Type &value)
    {
        widget->clear();
        for (const auto &subValue : value)
        {
            auto item = new QListWidgetItem;
            item->setData(subValue);
            widget->addItem(item);
        }
    }

    template<>
    static void saveList(QListWidget *widget, const QStringList &value)
    {
        widget->clear();
        for (const auto &subValue : value)
            widget->addItem(subValue);
    }

    template<>
    static void saveList(QListWidget *widget, const std::vector<DXGI_FORMAT> &value)
    {
        widget->clear();
        for (const DXGI_FORMAT &format : value)
        {
            auto item = new QListWidgetItem(dxgiFormatToString(format));
            item->setData(Qt::UserRole, format);
            widget->addItem(item);
        }
    }

    static void nullSaveFunc([[maybe_unused]] std::any b, [[maybe_unused]] std::any a) {}
};

} // namespace CAO
