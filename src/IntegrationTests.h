#ifndef INTEGRATIONTESTS_H
#define INTEGRATIONTESTS_H

#include "pch.h"
#include "FilesystemOperations.h"
#include "MainOptimizer.h"

class IntegrationTests
{
public:
    IntegrationTests(const QString& path);

    bool runAllTests();

private:
    QDir m_dir;
};

#endif // INTEGRATIONTESTS_H
