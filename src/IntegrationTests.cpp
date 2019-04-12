#include "IntegrationTests.h"

IntegrationTests::IntegrationTests(const QString& path) : dir(QDir(path)) {};

bool IntegrationTests::runAllTests()
{
    qDebug() << "tests folder: " << dir.path();

    QStringList dirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    QVector<bool> results;

    for(int i = 0 ; i < dirs.size() ; ++i)
    {
        const QString input = dir.filePath(dirs.at(i) + "/INPUT");
        const QString output = dir.filePath(dirs.at(i) + "/OUTPUT");
        const QString expected = dir.filePath(dirs.at(i) + "/EXPECTED");

        QString config = dir.filePath(dirs.at(i) + "/config.ini");
        QString CathedralIni = QCoreApplication::applicationDirPath() + "/Cathedral Assets Optimizer.ini";
        QFile::remove(CathedralIni);
        QFile::copy(config, CathedralIni);

        FilesystemOperations::copyDir(input, output, true);

        MainOptimizer optimizer;
        optimizer.mainProcess();

        results << FilesystemOperations::compareFolders(output, expected, true);

        QDir reset (output);
        reset.removeRecursively();
    }

    qDebug() << results;
    return true;
}



