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
    bool bExtractBsa{};
    bool bCreateBsa{};
    bool bPackExistingFiles{};

    bool bTgaConversion{};
    bool bBc7Conversion{};
    bool bNifscanOnTextures{};

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

    void extractBsa();
    void createBsa();
    void renameBsa();

    void tgaToDdsConversion(QDirIterator* it);
    void bc7TexturesConversion(QDirIterator* it);
    void nifscanTextures();

    void listMeshes();
    void optimizeMesh(QDirIterator* it);
    void textureCaseFixMesh(QDirIterator* it);

    void optimizeAnimation(QDirIterator* it);

    //Filesystem operations

    QString getPlugin();
    QString findSkyrimDirectory();
    void moveAssets(QString source, QString dest, bool allFiles);

    //Settings operations

    void saveSettings();
    void loadSettings();
    void resetToDefaultSettings();
    void printSettings();

    //Setters and getters

    void setDebugLog(QTextEdit* log);

private:
    QString modPath;
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

    QStringList textures;

    int dummyPluginsCounter;
};


#endif // OPTIMISER
