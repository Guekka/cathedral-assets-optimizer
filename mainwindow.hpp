#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileDialog>
#include <QGridLayout>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QCheckBox>

class MainWindow : public QWidget
{

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
