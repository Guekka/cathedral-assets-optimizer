#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QTranslator>
#include <QFileDialog>
#include <QGridLayout>
#include <QPlainTextEdit>
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

    void closeEvent(QCloseEvent *event);

private:
    QFileDialog *fileDialog{};
    Ui::MainWindow *ui;
    devModeUI *devmode;
    Optimiser *optimizer;

    bool bSimpleMode = true;
    bool bDarkMode = true;

    void loadUIFromVars();
    void saveSettings();
    void loadSettings();
};

#endif // MAINWINDOW_H
