#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QTranslator>
#include <QFileDialog>
#include <QGridLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QMessageBox>
#include <QMainWindow>

#include "Optimiser.hpp"
#include "devmodeui.h"

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

    void saveUIToVars();
    void loadUIFromVars();
    void saveSettings();
    void loadSettings();
};

#endif // MAINWINDOW_H
