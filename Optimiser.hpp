#ifndef OPTIMISER
#define OPTIMISER

#include <QDirIterator>
#include <QCoreApplication>
#include <QProcess>
#include <QDebug>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QSettings>
#include <QProgressBar>
#include <QCryptographicHash>
#include <QRegularExpression>

#include <utility>



struct optOptions
{
    bool bExtractBsa{};
    bool bCreateBsa{};
    bool bPackExistingFiles{};

    bool bTgaConversion{};
    bool bBc7Conversion{};
    bool bNifscanOnTextures{};

    bool bOptimizeHardCrashingMeshes;
    bool bOptimizeOtherMeshes;

    bool bOptimizeAnimations{};

    bool bDryRun{};

    int mode{};

    QString userPath;
};


class Optimiser : public QObject
{


public:
    Optimiser(QPlainTextEdit* textedit, QPlainTextEdit* debuglog, QProgressBar* bar);
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

    void optimizeAnimation(QDirIterator* it);

    //Filesystem operations

    QString findEspName();
    QString findSkyrimDirectory();
    void moveAssets(QString dest);

    //Settings operations

    void saveSettings();
    void loadSettings();
    void resetToDefaultSettings();
    void printSettings();

    //Setters and getters

    void setDebugLog(QPlainTextEdit* log);

private:
    QString modPath;
    QStringList modDirs;

    QPlainTextEdit* log;
    QPlainTextEdit* debugLog;
    QPlainTextEdit* trash;

    QProgressBar* progressBar;

    QStringList hardCrashingMeshes;
    QStringList otherMeshes;
    QStringList otherHeadparts;
    QStringList crashingHeadparts;

    int dummyPluginsCounter;
};


#endif // OPTIMISER
