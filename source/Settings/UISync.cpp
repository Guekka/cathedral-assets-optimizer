/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "UISync.hpp"
#include "GUI/MainWindow.hpp"

bool CAO::UISync::readCheckbox(const QAction *widget)
{
    return widget->isChecked();
}

bool CAO::UISync::readCheckbox(const QRadioButton *widget, const QGroupBox *parent)
{
    if (!parent->isChecked())
        return false;
    return widget->isChecked();
}

bool CAO::UISync::readCheckbox(const QGroupBox *widget)
{
    return widget->isChecked();
}

int CAO::UISync::readMeshesOptLevel(const MainWindow &window)
{
    auto& mainUi = window.mainUI();
    if (!mainUi.meshesGroupBox->isChecked())
        return 0;
    if (mainUi.meshesNecessaryOptimizationRadioButton->isChecked())
        return 1;
    if (mainUi.meshesMediumOptimizationRadioButton->isChecked())
        return 2;
    if (mainUi.meshesFullOptimizationRadioButton->isChecked())
        return 3;
    return 0;
}

bool CAO::UISync::readTab(const QWidget *widget)
{
    return widget->isEnabled();
}

int CAO::UISync::readValue(const QSpinBox *widget)
{
    return widget->value();
}

double CAO::UISync::readGigaByteValue(const QDoubleSpinBox *widget)
{
    return widget->value() * GigaByte;
}

QString CAO::UISync::readText(const QLineEdit *widget)
{
    return widget->text();
}

QStringList CAO::UISync::readText(const QPlainTextEdit *widget)
{
    return widget->toPlainText().split('\n');
}

void CAO::UISync::saveCheckbox(QCheckBox *widget, const bool &value)
{
    widget->setChecked(value);
}

void CAO::UISync::saveCheckbox(QRadioButton *widget, const bool &value)
{
    widget->setChecked(value);
}

void CAO::UISync::saveCheckbox(QAction *widget, const bool &value)
{
    widget->setChecked(value);
}

void CAO::UISync::saveCheckbox(QGroupBox *widget, const bool &value)
{
    widget->setChecked(value);
}

void CAO::UISync::saveMeshesOptLevel(MainWindow &window, const int &value)
{
    switch (value) {
    case 0:
        window.mainUI().meshesGroupBox->setChecked(false);
        return;
    case 1:
        window.mainUI().meshesNecessaryOptimizationRadioButton->setChecked(true);
        return;
    case 2:
        window.mainUI().meshesMediumOptimizationRadioButton->setChecked(true);
        return;
    case 3:
        window.mainUI().meshesFullOptimizationRadioButton->setChecked(true);
        return;
    }
}

bool CAO::UISync::readCheckbox(const QCheckBox *widget)
{
    return widget->isChecked();
}
