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
class JSON;

class UISync final
{
public:
    QString key_; //Necessary to provide equality operator

    //Actual class content
    using uiRead = std::function<void(const MainWindow &window, JSON &json)>;
    using uiSave = std::function<void(MainWindow &window, const JSON &json)>;

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

    static std::string readText(const QLineEdit *widget);
    static std::vector<std::string> readText(const QPlainTextEdit *widget);

    template<typename returnValue>
    static returnValue readValue(const QComboBox *widget)
    {
        return widget->currentData().value<returnValue>();
    }

    template<typename Type>
    static std::vector<Type> readList(const QListWidget *widget)
    {
        QVector<Type> data;
        for (int i = 0; i < widget->count(); ++i)
        {
            const auto &entry = widget->item(i);
            const auto &value = entry->data(Qt::UserRole).value<Type>();
            if (!data.contains(value))
                data += value;
        }
        return data.toStdVector();
    }

    template<>
    static std::vector<std::string> readList(const QListWidget *widget)
    {
        QVector<std::string> data;
        for (int i = 0; i < widget->count(); ++i)
        {
            const auto &entry = widget->item(i);
            const std::string &text = entry->text().toStdString();
            if (!data.contains(text))
                data += text;
        }
        return data.toStdVector();
    }

    static void saveCheckbox(QCheckBox *widget, const bool &value);
    static void saveCheckbox(QRadioButton *widget, const bool &value);
    static void saveCheckbox(QAction *widget, const bool &value);
    static void saveCheckbox(QGroupBox *widget, const bool &value);

    static void saveMeshesOptLevel(MainWindow &window, const int &value);

    static void saveTab(QWidget *widget, const bool &value) { widget->setEnabled(value); }
    static void saveValue(QSpinBox *widget, const int &value) { widget->setValue(value); }
    static void saveValueGigabyte(QDoubleSpinBox *widget, const double &value) { widget->setValue(value / GigaByte); }

    static void saveText(QLineEdit *widget, const std::string &value)
    {
        widget->setText(QString::fromStdString(value));
    }
    static void saveText(QPlainTextEdit *widget, const std::vector<std::string> &value)
    {
        QString out;
        for (const auto &str : value)
            out += QString::fromStdString(str) + '\n';
        widget->setPlainText(out);
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
    static void saveList(QListWidget *widget, const std::vector<Type> &value)
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
    static void saveList(QListWidget *widget, const std::vector<std::string> &value)
    {
        widget->clear();
        for (const auto &subValue : value)
            widget->addItem(QString::fromStdString(subValue));
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
};

} // namespace CAO
