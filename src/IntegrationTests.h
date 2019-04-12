#ifndef INTEGRATIONTESTS_H
#define INTEGRATIONTESTS_H

#include "pch.h"
#include "MainOptimizer.h"
#include <QTemporaryDir>

class IntegrationTests
{
public:
    IntegrationTests(const QString& path);

    bool runAllTests();

private:
    QDir dir;
};

#endif // INTEGRATIONTESTS_H
