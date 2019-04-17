#include "QLogger.h"

/****************************************************************************************
 ** QLogger is a library to register and print logs into a file.
 ** Copyright (C) 2018  Francesc Martinez <es.linkedin.com/in/cescmm/en>
 **
 ** This library is free software; you can redistribute it and/or
 ** modify it under the terms of the GNU Lesser General Public
 ** License as published by the Free Software Foundation; either
 ** version 2.1 of the License, or (at your option) any later version.
 **
 ** This library is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 ** Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public
 ** License along with this library; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 ***************************************************************************************/

namespace QLogger
{
        LogLevel intToLogLevel(const int& value)
        {
            switch (value)
            {
            case 0:   return LogLevel::Trace;
            case 1:   return LogLevel::Debug;
            case 2:   return LogLevel::Note;
            case 3:   return LogLevel::Info;
            case 4:   return LogLevel::Warning;
            case 5:   return LogLevel::Error;
            case 6:   return LogLevel::Fatal;
            }
            return LogLevel();
        }

        int logLevelToInt(const LogLevel &level)
        {
            switch (level)
            {
            case LogLevel::Trace:   return 0;
            case LogLevel::Debug:   return 1;
            case LogLevel::Note:    return 2;
            case LogLevel::Info:    return 3;
            case LogLevel::Warning: return 4;
            case LogLevel::Error:   return 5;
            case LogLevel::Fatal:   return 6;
            }

            return int();
        }


       void QLog_Trace(const QString &module, const QString &message)
       {
           QLog_(module, LogLevel::Trace, message);
       }

       void QLog_Debug(const QString &module, const QString &message)
       {
           QLog_(module, LogLevel::Debug, message);
       }

       void QLog_Note(const QString &module, const QString &message)
       {
           QLog_(module, LogLevel::Note, message);
       }

       void QLog_Info(const QString &module, const QString &message)
       {
           QLog_(module, LogLevel::Info, message);
       }

       void QLog_Warning(const QString &module, const QString &message)
       {
           QLog_(module, LogLevel::Warning, message);
       }

       void QLog_Error(const QString &module, const QString &message)
       {
           QLog_(module, LogLevel::Error, message);
       }

       void QLog_Fatal(const QString &module, const QString &message)
       {
           QLog_(module, LogLevel::Fatal, message);
       }

   void QLog_(const QString &module, LogLevel level, const QString &message)
   {
       const auto manager = QLoggerManager::getInstance();

       QMutexLocker(&manager->mutex);

       const auto logWriter = manager->getLogWriter(module);

       if (logWriter && logWriter->getLevel() <= level)
               logWriter->write(module,message, level);
}

    //QLoggerManager
    QLoggerManager * QLoggerManager::INSTANCE = nullptr;

    QLoggerManager::QLoggerManager() : mutex(QMutex::Recursive)
    {
        start();

        QDir dir(QDir::currentPath());

        if (!dir.exists("logs"))
            dir.mkdir("logs");
    }

    QLoggerManager * QLoggerManager::getInstance()
    {
        if (!INSTANCE)
            INSTANCE = new QLoggerManager();

        return INSTANCE;
    }

    QString QLoggerManager::levelToStartingText(const LogLevel &level)
    {
        switch (level)
        {
        case LogLevel::Trace:   return "<font color=BlueViolet>Trace";
        case LogLevel::Debug:   return "<font color=Blue>Debug";
        case LogLevel::Note:    return "Note";
        case LogLevel::Info:    return "<font color=Green>Info";
        case LogLevel::Warning: return "<font color=Orange>Warning";
        case LogLevel::Error:   return "<font color=Red>Error";
        case LogLevel::Fatal:   return "<font color=DarkRed>Fatal";
        }

        return QString();
    }

    bool QLoggerManager::addDestination(const QString &fileDest, const QString &module, LogLevel level)
    {
        if (!moduleDest.contains(module))
        {
            const auto log = new QLoggerWriter(fileDest,level);
            moduleDest.insert(module, log);

            return true;
        }

        return false;
    }

    bool QLoggerManager::addDestination(const QString &fileDest, const QStringList &modules, LogLevel level)
    {
        bool allAdded = false;

        for (const auto &module : modules)
        {
            if (!moduleDest.contains(module))
            {
                const auto log = new QLoggerWriter(fileDest,level);
                moduleDest.insert(module, log);
                allAdded = true;
            }
        }

        return allAdded;
    }

    void QLoggerManager::setLogLevelForAllWriters(LogLevel level)
    {
        for (const auto& module : moduleDest)
        {
            module->setLevel(level);
        }
    }

    void QLoggerManager::closeLogger()
    {
        deleteLater();
        exit(0);
    }

    QLoggerWriter::QLoggerWriter(const QString &fileDestination, LogLevel level)
    {
        mFileDestination = "logs/" + fileDestination;
        m_level = level;
    }

    QString QLoggerWriter::renameFileIfFull()
    {
        const auto MAX_SIZE = 1024 * 4096;
        const auto toRemove = mFileDestination.section('.',-1);
        const auto fileNameAux = mFileDestination.left(mFileDestination.size() - toRemove.size()-1);
        auto renamed = false;
        auto newName = fileNameAux + "_%1__%2.html";

        QFile file(mFileDestination);

        //Renaming file if it's full
        if (file.size() >= MAX_SIZE)
        {
            //Creem un fixer nou
            const auto currentTime = QDateTime::currentDateTime();
            newName = newName.arg(currentTime.date().toString("dd_MM_yy"), (currentTime.time().toString("hh_mm_ss")));
            renamed = QFile::rename(mFileDestination, newName);
        }

        return renamed ? newName : "";
    }

    void QLoggerWriter::write(const QString &module, const QString &message, const LogLevel& messageLogLevel)
    {
        QFile file(mFileDestination);

        const auto newName = renameFileIfFull ();

        if (file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append))
        {
            QTextStream out(&file);
            const auto dtFormat = QDateTime::currentDateTime().toString("dd-MM hh:mm:ss");

            if (!newName.isEmpty())
                out << QString("%1 - Previous log %2\n").arg(dtFormat, newName);

            const auto logLevel = QLoggerManager::levelToStartingText(messageLogLevel);
            const auto text = QString("<pre>[%1] [%2] {%3} %4</font></pre>\n").arg(dtFormat, logLevel, module, message);

            out << text;
            file.close();
        }
    }
}


