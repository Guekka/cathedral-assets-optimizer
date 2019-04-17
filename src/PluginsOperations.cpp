#include "PluginsOperations.h"

void PluginsOperations::makeDummyPlugins(const QString& folderPath)
{
    QLogger::QLog_Trace("PluginsOperations", "Entering makeDummyPluginsfunction: creating enough dummy plugins to load BSAs");

    QDirIterator it(folderPath);

    while (it.hasNext())
    {
        QString espName;

        it.next();

        if(!checkIfBsaHasPlugin(it.filePath()) && it.fileName().endsWith(".bsa", Qt::CaseInsensitive))
        {
            if(it.fileName().contains("- Textures", Qt::CaseInsensitive))
            {
                espName = it.fileName().remove(" - Textures.bsa", Qt::CaseInsensitive) + ".esp";
                QLogger::QLog_Trace("PluginsOperations", "Created textures bsa plugin:" + espName);
            }

            else
            {
                espName = it.fileName().remove(".bsa", Qt::CaseInsensitive) + ".esp";
                QLogger::QLog_Trace("PluginsOperations", "Created standard bsa plugin:" + espName);
            }
            QFile::copy(QCoreApplication::applicationDirPath() + "/resources/BlankSSEPlugin.esp", folderPath + "/" + espName);
        }
    }
    QLogger::QLog_Trace("PluginsOperations", "Exiting makeDummyPlugins function");
}


QString PluginsOperations::findPlugin(const QString& folderPath)
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
        }

        if(it.fileName().endsWith(".bsa", Qt::CaseInsensitive))
            bsaName = it.fileName().chopped(4) + ".esp";
    }
    if(!bsaName.isEmpty() && espName.isEmpty())
        espName = bsaName;

    if(espName.isEmpty())
    {
        espName = QDir(folderPath).dirName() + ".esp";
        QLogger::QLog_Debug("PluginsOperations", "Using: " + espName + " as esp name.");
    }

    return espName.remove(QRegularExpression("\\.es[plm]$"));
}



bool PluginsOperations::checkIfBsaHasPlugin(QString bsaPath)
{
    QString bsaName = QFileInfo(bsaPath).fileName();
    bsaName.remove(".bsa");
    bsaName.remove(" - Textures.bsa"); // x.esp will also load x - Textures.bsa

    QString eslName = bsaName + ".esl";
    QString esmName = bsaName + ".esm";
    QString espName = bsaName + ".esp";

    bool hasEsl = QFile(eslName).exists();
    bool hasEsm = QFile(esmName).exists();
    bool hasEsp = QFile(espName).exists();

    if(hasEsl || hasEsm || hasEsp)
        return true;

    else
        return false;
}
