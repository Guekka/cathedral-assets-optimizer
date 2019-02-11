#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <QDirIterator>
#include <QProcess>
#include <QDebug>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QSettings>
#include <utility>

class Optimiser
{

public:

    Optimiser(QString mod, QPlainTextEdit* textEdit);

    int mainProcess();

    bool extractBsa();
    bool textOpt();
    bool nifOpt();
    bool createBsa();
    bool createTexturesBsa();
    bool deleteBsa();
    bool animOpt();

    bool setUserPath(const QString& path);
    bool checkResourcesFolder(const QString& exe);

    QString getUserPath() const;
    QString findEspName();
    QString findSkyrimDir();

    void setMode(int index);

    void setExtractBsaBool(bool state);
    void setTextOptBool(bool state);
    void setNifOptBool(bool state);
    void setCreateBsaBool(bool state);
    void setDeleteBsaBool(bool state);
    void setAnimOptBool(bool state);

    bool getExtractBsaBool();
    bool getTextOptBool();
    bool getNifOptBool();
    bool getCreateBsaBool();
    bool getDeleteBsaBool();
    bool getAnimOptBool();

    void saveSettings();
    void loadSettings();

private:
    QString modPath;
    QString userPath;

    QPlainTextEdit* log;

    bool extractBsaBool{};
    bool textOptBool{};
    bool nifOptBool{};
    bool createBsaBool{};
    bool deleteBsaBool{};
    bool animOptBool{};

    int mode{};

};


#endif // FUNCTIONS_H
