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

#include <utility>



struct optOptions
{


    bool extractBsa{};
    bool createBsa{};
    bool renameBsa{};


    bool tgaConv{};
    bool bc7Conv{};
    bool nifscanTextures{};


    bool hardCrashingMeshes;
    bool otherMeshes;


    int mode{};

    QString userPath;
};


class Optimiser
{



public:

    Optimiser(QPlainTextEdit* textEdit, QProgressBar* bar);

    optOptions options;

    int mainProcess();

    bool extractBsa();
    bool createBsa();
    bool createTexturesBsa();
    bool renameBsa();


    bool tgaConv(QDirIterator* it);
    bool bc7Conv(QDirIterator* it);
    bool nifscanTextures();


    bool nifScan();
    bool nifOpt(QDirIterator* it);

    bool animOpt(QDirIterator* it);

    bool checkResourcesFolder(const QString& exe);

    QString findEspName();
    QString findSkyrimDir();

    void saveSettings();
    void loadSettings();

    bool getBsaOptBool();
    bool getTextOptBool();
    bool getNifOptBool();
    bool getAnimOptBool();

    void setBsaOptBool(bool state);
    void setTextOptBool(bool state);
    void setNifOptBool(bool state);
    void setAnimOptBool(bool state);

    void dryRun();

private:
    QString modPath;

    QPlainTextEdit* log;
    QProgressBar* progressBar;

    QStringList hardCrashingMeshes;
    QStringList otherMeshes;
    QStringList headparts;

    bool bsaOptBool{};
    bool textOptBool{};
    bool nifOptBool{};
    bool animOptBool{};
};


#endif // FUNCTIONS_H
