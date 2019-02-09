#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <QDirIterator>
#include <QProcess>
#include <QDebug>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QSettings>

class Optimiser
{

public:

    Optimiser(QString mod, QPlainTextEdit* textEdit);

    void extractBsa(bool deleteBsa);
    void textOpt();
    void nifOpt();
    void createBsa();
    void animOpt();

private:
    QString modPath;
    QPlainTextEdit log;

};




#endif // FUNCTIONS_H
