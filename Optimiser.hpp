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

#include <utility>



struct optOptions
{
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


public:
    Optimiser(QTextEdit* textedit, QTextEdit* debuglog, QProgressBar* bar);
    ~Optimiser();

    optOptions options;

    //Main functions

    bool setup();
    int mainProcess();
    void dryRun();

    //Assets optimization

    void bsaExtract(const QString& bsaPath);
    void bsaCreate();

    void texturesTgaToDds(QDirIterator* it);
    void texturesBc7Conversion(QDirIterator* it);

    void meshesList();
    void meshesOptimize(QDirIterator* it);
    void meshesTexturesCaseFix(QDirIterator* it);

    void animationsOptimize(QDirIterator* it);

    //Filesystem operations

    QString getPlugin();
    QString findSkyrimDirectory();
    bool moveAssets(QString destination);
    bool moveFiles(QString source, QString destination, bool overwriteExisting);

    //Settings operations

    void saveSettings();
    void loadSettings();
    void resetToDefaultSettings();
    void printSettings();

    //Setters and getters

    void setDebugLog(QTextEdit* log);

private:
    QDir modpathDir;
    QStringList modDirs;

    QTextEdit* log;
    QTextEdit* debugLog;
    QTextEdit* trash{};

    QProgressBar* progressBar;

    QStringList crashingMeshes;
    QStringList otherMeshes;
    QStringList headparts;
    QStringList customHeadparts;
};


#endif // OPTIMISER
