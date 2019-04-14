#ifndef OPTIMISER_H
#define OPTIMISER_H

#include "pch.h"
#include "QLogger.h"
#include "AnimationsOptimizer.h"
#include "BsaOptimizer.h"
#include "FilesystemOperations.h"
#include "MeshesOptimizer.h"
#include "PluginsOperations.h"
#include "TexturesOptimizer.h"

struct optOptions
{
    optOptions()
    {
        mode = 0;
        userPath = "";

        bBsaExtract = true;
        bBsaCreate = true;

        bAnimationsOptimization = true;

        bDryRun = false;
    }

    bool bBsaExtract{};
    bool bBsaCreate{};
    bool bBsaPackLooseFiles{};
    bool bBsaDeleteBackup{};
    bool bBsaSplitAssets{};

    bool bMeshesProcess{};

    bool bTexturesNecessaryOptimization{};
    bool bTexturesFullOptimization{};

    bool bAnimationsOptimization{};

    bool bDryRun{};

    int mode{};

    QString userPath;
};


class MainOptimizer : public QObject
{
    Q_OBJECT

public:
    MainOptimizer();

    optOptions options;

    int mainProcess();

    //Settings operations

    void loadSettings();

    void setLogLevel(const QLogger::LogLevel &value) { logLevel = value; }

private:
    QStringList modDirs;

    QLogger::QLoggerManager *logManager;
    QLogger::LogLevel logLevel;

    void init();
    void dryRun();
    bool checkRequirements();
    void fillModsLists();
    void optimizeAssets(const QString& folderPath);
    void dryOptimizeAssets(const QString& folderPath);



signals:
    void progressBarMaximumChanged(int maximum);
    void progressBarIncrease();
    void progressBarReset();
    void progressBarBusy();
    void end();
    void updateLog();
};


#endif // OPTIMISER_H
