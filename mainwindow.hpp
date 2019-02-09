#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileDialog>
#include <QGridLayout>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QCoreApplication>

class MainWindow : public QWidget
{
    Q_DECLARE_TR_FUNCTIONS(MainWindow)

public:
    MainWindow();


private:
    QFileDialog *fileDialog;
    QPlainTextEdit *modpathTextEdit;
    QPlainTextEdit *log;
    QPushButton *pathButton;
    QPushButton *processButton;

    QCheckBox *extractBsaCheckbox;
    QCheckBox *deleteBsaCheckbox;
    QCheckBox *textOptCheckbox;
    QCheckBox *nifOptCheckbox;
    QCheckBox *createBsaCheckbox;
    QCheckBox *animOptCheckbox;

    QGridLayout *gridLayout;

};

#endif // MAINWINDOW_H
