#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Optimiser.h"
#include "Devmodeui.h"
#include "pch.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_DECLARE_TR_FUNCTIONS(MainWindow)

public:
    MainWindow();
    ~MainWindow();

private:
    QFileDialog *fileDialog{};
    Ui::MainWindow *ui;
    devModeUI *devmode;
    Optimiser *optimizer;

    bool bSimpleMode = true;
    bool bDarkMode = true;
    bool bLockVariables = false;

    void saveUIToVars();
    void loadUIFromVars();
    void saveSettings();
    void loadSettings();

    int progressBarValue;

    QThread* workerThread;

};

#endif // MAINWINDOW_H
