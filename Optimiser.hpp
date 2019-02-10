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

    void extractBsa();
    void textOpt();
    void nifOpt();
    void createBsa();
    void createTexturesBsa();
    void deleteBsa();
    void animOpt();

    bool setModPath(const QString& path);

    QString getmodPath() const;
    QString findEspName();
    QString findSkyrimDir();

private:
    QString modPath;
    QPlainTextEdit* log;

};


#endif // FUNCTIONS_H
