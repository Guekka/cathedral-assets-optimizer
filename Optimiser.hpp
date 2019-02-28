#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <QDirIterator>
#include <QCoreApplication>
#include <QProcess>
#include <QDebug>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QSettings>
#include <QProgressBar>
#include <QCryptographicHash>
#include <QTemporaryDir>
#include <QRegularExpression>

#include <utility>



struct optOptions
{


    bool extractBsa{};
    bool recreateBsa{};
    bool packExistingFiles{};

    bool tgaConv{};
    bool bc7Conv{};
    bool nifscanTextures{};

    bool hardCrashingMeshes;
    bool otherMeshes;

    bool animOptBool{};

    int mode{};

    bool dryRun{};

    QString userPath;
};


class Optimiser : public QObject
{


public:

    Optimiser(QPlainTextEdit* textEdit, QProgressBar* bar);

    optOptions options;

    int mainProcess();
    void dryRun();

    //Assets optimization

    void extractBsa();
    void createBsa();
    void renameBsa();


    void tgaConv(QDirIterator* it);
    void bc7Conv(QDirIterator* it);
    void nifscanTextures();


    void nifScan();
    void nifOpt(QDirIterator* it);


    void animOpt(QDirIterator* it);

    //Some operations

    QString findEspName();
    QString findSkyrimDir();

    void saveSettings();
    void loadSettings();
    void resetToDefaultSettings();

    //Filesystem operations

    void moveAssets(const QString& dest);


private:
    QString modPath;

    QPlainTextEdit* log;
    QProgressBar* progressBar;

    QStringList hardCrashingMeshes;
    QStringList otherMeshes;
    QStringList otherHeadparts;
    QStringList crashingHeadparts;

    int dummyPluginsCounter;

};


#endif // FUNCTIONS_H
