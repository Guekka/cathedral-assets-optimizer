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
    bool bsaOpt{};

    bool extractBsa{};
    bool createBsa{};
    bool renameBsa{};


    bool textOpt{};

    bool tgaConv{};
    bool bc7Conv{};
    bool nifscanTextures{};


    bool nifOpt{};

    bool hardCrashingNif;

    bool animOpt{};


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


    bool nifOpt(QDirIterator* it);

    bool animOpt(QDirIterator* it);

    bool checkResourcesFolder(const QString& exe);

    QString findEspName();
    QString findSkyrimDir();

    void saveSettings();
    void loadSettings();

private:
    QString modPath;

    QPlainTextEdit* log;
    QProgressBar* progressBar;



};


#endif // FUNCTIONS_H
