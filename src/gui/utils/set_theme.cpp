/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "logger.hpp"
#include "set_theme.hpp"

#include <QApplication>
#include <QFile>

namespace cao {

auto get_dark_style_sheet() -> QString
{
    QFile f(":qdarkstyle/style.qss");
    if (!f.open(QFile::ReadOnly | QFile::Text))
    {
        PLOG_ERROR << "Cannot set darkstyle";
        return {};
    }
    return f.readAll();
}

auto set_theme(GuiTheme theme) -> bool
{
    if (theme == GuiTheme::Light)
    {
        qApp->setStyleSheet("");
    }
    else if (theme == GuiTheme::Dark)
    {
        static const QString dark_sheet = get_dark_style_sheet();

        if (dark_sheet.isEmpty())
            return false;

        qApp->setStyleSheet(dark_sheet);
    }
    return true;
}
} // namespace cao
