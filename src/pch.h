// Add C includes here

#if defined __cplusplus
// Add C++ includes here

//Win7 compatibility
#undef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_WIN7
#include <Windows.h>

/*---CORE---------------*/
#include <utility>
#include <QCommandLineParser>
#include <QCompleter>
#include <QCoreApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QDirIterator>
#include <QFile>
#include <QInputDialog>
#include <QMap>
#include <QMutex>
#include <QProcess>
#include <QRegularExpression>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QTextCodec>
#include <QTextStream>
#include <QTimer>
#include <QTranslator>
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
#include <fstream>
#include <optional>
/*----------------------*/

/*--WINDOWS-------------*/
#include <wrl/client.h>
/*----------------------*/

/*---CUSTOM FILES-----*/
#include "DirectXTex.h"
#include "Logger.h"
#include "NifFile.hpp"
#include "TexturesFormats.h"
#include "libs/BSAUtil/include/BSACreate.hpp"
#include "libs/BSAUtil/include/BSAExtract.hpp"
/*----------------------*/

#endif
