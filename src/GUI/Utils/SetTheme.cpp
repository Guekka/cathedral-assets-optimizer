/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "SetTheme.hpp"

#include "Logger.hpp"

#include <QApplication>
#include <QFile>

namespace cao {

QString getDarkStyleSheet()
{
    QFile f(":qdarkstyle/style.qss");
    if (!f.open(QFile::ReadOnly | QFile::Text))
    {
        PLOG_ERROR << "Cannot set darkstyle";
        return QString();
    }
    return QString(f.readAll());
}

bool setTheme(GuiTheme theme)
{
    if (theme == GuiTheme::Light)
    {
        qApp->setStyleSheet("");
    }
    else if (theme == GuiTheme::Dark)
    {
        static const QString darkSheet = getDarkStyleSheet();

        if (darkSheet.isEmpty())
            return false;

        qApp->setStyleSheet(darkSheet);
    }
    return true;
}
} // namespace cao
