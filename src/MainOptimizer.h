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

/*!
 * \brief Manages the optimization options
 */

struct optOptions
{
    optOptions()
    {
        mode = 0;
        userPath = "";

        bBsaExtract = true;
        bBsaCreate = true;
        bBsaPackLooseFiles = false;
        bBsaDeleteBackup = false;
        bBsaSplitAssets = false;
        bBsaMergeLoose = false;

        bMeshesProcess = true;

        bTexturesNecessaryOptimization = true;
        bTexturesFullOptimization = true;

        bAnimationsOptimization = true;

        bDryRun = false;
    }

    QString userPath;
    int mode{};

    bool bBsaExtract{};
    bool bBsaCreate{};
    bool bBsaPackLooseFiles{};
    bool bBsaDeleteBackup{};
    bool bBsaSplitAssets{};
    bool bBsaMergeLoose{};

    bool bMeshesProcess{};

    bool bTexturesNecessaryOptimization{};
    bool bTexturesFullOptimization{};

    bool bAnimationsOptimization{};

    bool bDryRun{};
};

/*!
 * \brief Coordinates all the subclasses in order to optimize BSAs, textures, meshes and animations
 */

class MainOptimizer : public QObject
{
    Q_OBJECT

public:
    MainOptimizer();

    optOptions options;

    int mainProcess();

    //Settings operations

    /*!
     * \brief Loads settings from the ini file to variables
     */
    void loadSettings();
    /*!
     * \brief setLogLevel
     * \param value
     */

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
};


#endif // OPTIMISER_H
