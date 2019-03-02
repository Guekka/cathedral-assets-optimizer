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
    void loadUIFromVars();
    void closeEvent(QCloseEvent *event);
    void saveSettings();
    void loadSettings();

private:
    QFileDialog *fileDialog{};

    Ui::MainWindow *ui;

    devModeUI *devmode;

    Optimiser *optimizer;

    bool simpleMode = true;
    bool darkMode = true;
};

#endif // MAINWINDOW_H
