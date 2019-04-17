#ifndef MAIN_OPTIMIZER_H
#define MAIN_OPTIMIZER_H

#include "pch_core.h"
#include "QLogger.h"
#include "AnimationsOptimizer.h"
#include "BsaOptimizer.h"
#include "FilesystemOperations.h"
#include "MeshesOptimizer.h"
#include "PluginsOperations.h"
#include "TexturesOptimizer.h"

struct optOptions
{
    bool bBsaExtract{};
    bool bBsaCreate{};
    bool bBsaPackLooseFiles{};
    bool bBsaDeleteBackup{};
    bool bBsaSplitAssets{};
    bool bAnimationsOptimization{};
    bool bDryRun{};

    int iMeshesOptimizationLevel{};
    int iTexturesOptimizationLevel{};
    int iMode{};

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

    static void resetSettings();

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
