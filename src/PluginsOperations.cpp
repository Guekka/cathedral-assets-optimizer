#include "PluginsOperations.h"

void PluginsOperations::makeDummyPlugins(const QString& folderPath)
{
    QLogger::QLog_Trace("PluginsOperations", "Entering makeDummyPluginsfunction: creating enough dummy plugins to load BSAs");

    QDirIterator it(folderPath);

    while (it.hasNext())
    {
        QString espName;

        it.next();

        if(it.fileName().endsWith(".bsa", Qt::CaseInsensitive) && it.fileName().contains("- Textures", Qt::CaseInsensitive))
        {
            espName = it.fileName().remove(" - Textures.bsa", Qt::CaseInsensitive) + ".esp";
            QFile::copy(QCoreApplication::applicationDirPath() + "/resources/BlankSSEPlugin.esp", folderPath + "/" + espName);
            QLogger::QLog_Trace("PluginsOperations", "Created textures bsa plugin:" + espName);
        }
        else if(it.fileName().endsWith(".bsa", Qt::CaseInsensitive))
        {
            espName = it.fileName().remove(".bsa", Qt::CaseInsensitive) + ".esp";
            QFile::copy(QCoreApplication::applicationDirPath() + "/resources/BlankSSEPlugin.esp", folderPath + "/" + espName);
            QLogger::QLog_Trace("PluginsOperations", "Created standard bsa plugin:" + espName);
        }
    }
    QLogger::QLog_Trace("PluginsOperations", "Exiting makeDummyPlugins function");
}


QString PluginsOperations::findPlugin(const QString& folderPath) //Find esp/esl/esm name using an iterator and regex. Also creates a plugin if there isn't one.
{
    QDirIterator it(folderPath);
    QString espName;
    QString bsaName;

    while (it.hasNext())
    {
        it.next();

        if(it.fileName().contains(QRegularExpression("\\.es[plm]$")))
        {
            espName=it.fileName();
            QLogger::QLog_Note("PluginsOperations", tr("Esp found: ") + espName);
            return espName;
        }

        if(it.fileName().endsWith(".bsa", Qt::CaseInsensitive))
            bsaName = it.fileName().chopped(4) + ".esp";
    }
    if(!bsaName.isEmpty())
        return bsaName;

    espName = QDir(folderPath).dirName() + ".esp";
    QLogger::QLog_Debug("PluginsOperations", "Using: " + espName + " as esp name.");
    return espName.chopped(4);
}


