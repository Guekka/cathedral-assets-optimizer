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
#include "Optimiser.hpp"

class MainWindow : public QWidget
{
    Q_DECLARE_TR_FUNCTIONS(MainWindow)

public:
    MainWindow();


private:
    QFileDialog *fileDialog;
    QPlainTextEdit *modpathTextEdit;
    QPlainTextEdit *mw_log;
    QPushButton *pathButton;
    QPushButton *processButton;
    QComboBox *dropDown;

    QCheckBox *extractBsaCheckbox;
    QCheckBox *deleteBsaCheckbox;
    QCheckBox *textOptCheckbox;
    QCheckBox *nifOptCheckbox;
    QCheckBox *createBsaCheckbox;
    QCheckBox *animOptCheckbox;

    QGridLayout *gridLayout;

    Optimiser *optimiser;
};

#endif // MAINWINDOW_H
