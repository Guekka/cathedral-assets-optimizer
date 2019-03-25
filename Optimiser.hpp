#ifndef OPTIMISER
#define OPTIMISER

#include <QDirIterator>
#include <QCoreApplication>
#include <QProcess>
#include <QDebug>
#include <QMessageBox>
#include <QTextEdit>
#include <QSettings>
#include <QProgressBar>
#include <QCryptographicHash>
#include <QRegularExpression>
#include <QTextCodec>
#include <QDateTime>
#include <QThread>

#include <utility>

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

    void texturesTgaToDds(QDirIterator* it);
    void texturesBc7Conversion(QDirIterator* it);

    void meshesOptimize(QDirIterator* it);
    void meshesTexturesCaseFix(QDirIterator* it);

    void animationsOptimize(QDirIterator* it);

    //Filesystem operations

    QString getPlugin();
    QString findSkyrimDirectory();
    void splitAssets();
    void moveFiles(QString source, QString destination, bool overwriteExisting);

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

    QFile logFile;
    QTextStream logStream;
    QFile debugLogFile;
    QTextStream debugLogStream;

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


#endif // OPTIMISER
