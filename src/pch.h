// Add C includes here

#if defined __cplusplus
// Add C++ includes here

#include <Windows.h>

/*---CORE---------------*/
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDateTime>
#include <QDirIterator>
#include <QFile>
#include <QMap>
#include <QMutex>
#include <QProcess>
#include <QRegularExpression>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QtConcurrent/QtConcurrent>
#include <QTextCodec>
#include <QTextStream>
#include <QTimer>
#include <QtMath>
#include <QTranslator>
#include <utility>
/*----------------------*/

/*---GUI----------------*/
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QWidget>
/*----------------------*/

/*--STD-----------------*/
#include <fstream>
#include <optional>
/*----------------------*/

/*--WINDOWS-------------*/
#include <wrl\client.h>
/*----------------------*/

/*---CUSTOM FILES-----*/
#include "Logger.h"
#include "libs/hkxcmd/hkfutils.h"
#include "libs/NIF/NifFile.h"
#include "libs/libbsarch/BSArchiveAuto.h"
#include "libs/DirectXTex/DirectXTex.h"
#include "TexturesFormats.h"
/*----------------------*/

#endif
