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
    void createTexturesBsa();
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

    //Filesystem operations

    void moveToTemp();
    void deleteEmptyDirs(QString path);
    qint64 dirSize(QString Path);

    //Getters and setters

    bool getBsaOptBool();
    bool getTextOptBool();
    bool getNifOptBool();
    bool getAnimOptBool();

    void setBsaOptBool(bool state);
    void setTextOptBool(bool state);
    void setNifOptBool(bool state);
    void setAnimOptBool(bool state);

private:
    QString modPath;

    QPlainTextEdit* log;
    QProgressBar* progressBar;

    QStringList hardCrashingMeshes;
    QStringList otherMeshes;
    QStringList otherHeadparts;
    QStringList crashingHeadparts;

    bool bsaOptBool{};
    bool textOptBool{};
    bool nifOptBool{};
    bool animOptBool{};
};


#endif // FUNCTIONS_H
