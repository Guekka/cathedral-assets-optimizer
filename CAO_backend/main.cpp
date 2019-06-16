/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "Manager.h"
#include "IntegrationTests.h"

int main(int argc, char *argv[])
{   
    QCoreApplication app(argc, argv);

    //If tests are enabled, run tests instead of running standard process

    if constexpr(/* DISABLES CODE */ (false)) //TODO find a better way to enable tests
    {
        IntegrationTests tests(QCoreApplication::arguments().at(1));
        return tests.runAllTests();
    }

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(), "translations");
    QCoreApplication::installTranslator(&qtTranslator);

    QTranslator AssetsOptTranslator;
    AssetsOptTranslator.load("AssetsOpt_" + QLocale::system().name(), "translations");
    QCoreApplication::installTranslator(&AssetsOptTranslator);
    try
    {
        Manager manager;
        manager.runOptimization();
    }
    catch(const std::runtime_error& e)
    {
        PLOG_FATAL << e.what();
        return 1;
    }
    return 0;
}
