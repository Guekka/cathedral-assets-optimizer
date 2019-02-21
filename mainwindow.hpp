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

private:
    QFileDialog *fileDialog{};

    Ui::MainWindow *ui;

    Optimiser *optimizer;
};

#endif // MAINWINDOW_H
