#ifndef OPTIMISER_H
#define OPTIMISER_H

#include "pch.h"
#include "QLogger.h"


struct optOptions
{
    optOptions()
    {
        mode = 0;
        userPath = "";

        bBsaExtract = true;
        bBsaCreate = true;

        bAnimationsOptimization = true;

        bDryRun = false;
    }

    bool bBsaExtract{};
    bool bBsaCreate{};

    bool bAnimationsOptimization{};

    bool bDryRun{};

    int mode{};

    QString userPath;
};


class MainOptimizer : public QObject
{
    Q_OBJECT

public:
    MainOptimizer();

    optOptions options;

    //Main functions

    bool init();
    int mainProcess();

    //Settings operations

    void saveSettings();
    void loadSettings();
    void printSettings();

    void setLogLevel(const QLogger::LogLevel &value);

private:
    QDir modpathDir;
    QStringList modDirs;

    QLogger::QLoggerManager *logManager;
    QLogger::LogLevel logLevel;

    void dryRun();


signals:
    void progressBarMaximumChanged(int maximum);
    void progressBarIncrease();
    void progressBarReset();
    void progressBarBusy();
    void end();


};


#endif // OPTIMISER_H
