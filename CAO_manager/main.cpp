/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include <QCoreApplication>
#include "MainManager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    try
    {
        MainManager manager;
    }
    catch(std::exception e)
    {
        QTextStream(stderr) << e.what();
        return 1;
    }

    return a.exec();
}
