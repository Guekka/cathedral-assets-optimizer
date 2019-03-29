#ifndef OPTIMISER_H
#define OPTIMISER_H

#include "pch.h"
#include "QLogger.h"

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

        bMeshesNecessaryOptimization = true;
        bMeshesMediumOptimization = false;
        bMeshesFullOptimization = false;

        bTexturesNecessaryOptimization = true;
        bTexturesFullOptimization = true;

        bAnimationsOptimization = true;

        bDryRun = false;
    }

    bool bBsaExtract{};
    bool bBsaCreate{};
    bool bBsaPackLooseFiles{};
    bool bBsaDeleteBackup{};
    bool bBsaSplitAssets{};

    bool bTexturesNecessaryOptimization{};
    bool bTexturesFullOptimization{};

    bool bMeshesHeadparts{};
    bool bMeshesNecessaryOptimization{};
    bool bMeshesMediumOptimization{};
    bool bMeshesFullOptimization{};

    bool bAnimationsOptimization{};

    bool bDryRun{};

    int mode{};

    QString userPath;
};


class Optimiser : public QObject
{
    Q_OBJECT

public:
    Optimiser();

    optOptions options;

    //Main functions

    bool setup();
    int mainProcess();

    //Assets optimization

    void bsaExtract(const QString& bsaPath);
    void bsaCreate();

    void texturesTgaToDds(const QString& filePath);
    void texturesBc7Conversion(const QString& filePath);

    void meshesOptimize(const QString& filePath);
    void meshesTexturesCaseFix(const QString& filePath);

    void animationsOptimize(const QString& filePath);

    //Filesystem operations

    QString getPlugin();
    QString findSkyrimDirectory();
    void splitAssets();
    void moveFiles(QString source, QString destination, bool overwriteExisting);
    QPair<qint64, qint64> assetsSize(const QString& path);

    //Settings operations

    void saveSettings();
    void loadSettings();
    void printSettings();


private:
    QDir modpathDir;
    QStringList modDirs;

    QStringList crashingMeshes;
    QStringList otherMeshes;
    QStringList headparts;
    QStringList customHeadparts;

    QLogger::LogLevel *logLevel;

    QLogger::QLoggerManager *logManager;
    //Main functions

    void dryRun();

    //Assets optimization

    void meshesList();



signals:
    void progressBarMaximumChanged(int maximum);
    void progressBarIncrease();
    void progressBarReset();
    void progressBarBusy();
    void end();


};


#endif // OPTIMISER_H
