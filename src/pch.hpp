// Add C includes here

#if defined __cplusplus
// Add C++ includes here

#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

/*---CORE---------------*/
#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QCommandLineParser>
#include <QCompleter>
#include <QCoreApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QDirIterator>
#include <QDoubleSpinBox>
#include <QFile>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QMap>
#include <QMenu>
#include <QMenuBar>
#include <QMutex>
#include <QPlainTextEdit>
#include <QProcess>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QRegularExpression>
#include <QSettings>
#include <QSpinBox>
#include <QString>
#include <QStringList>
#include <QTabWidget>
#include <QTextCodec>
#include <QTextStream>
#include <QTimer>
#include <QTranslator>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>
#include <QtConcurrent/QtConcurrent>
#include <QtMath>
/*----------------------*/

/*---GUI----------------*/
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QValidator>
#include <QWidget>
/*----------------------*/

/*--STD-----------------*/
#include <any>
#include <filesystem>
#include <fstream>
#include <optional>
#include <unordered_set>
#include <utility>
/*----------------------*/

/*--WINDOWS-------------*/
#include <wrl/client.h>
/*----------------------*/

/*---CUSTOM FILES-----*/
#include "Commands/Textures/TextureFormats.hpp"
#include "DirectXTex/DirectXTex.h"
#include "Logger.hpp"
#include "NIF/NifFile.h"
#include "hkxcmd/hkfutils.h"
#include "libbsarch/src/bs_archive_auto.hpp"
#include "pipes/include/pipes/pipes.hpp"
#include "json/single_include/nlohmann/json.hpp"
/*----------------------*/

#endif
