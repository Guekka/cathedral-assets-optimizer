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


QString PluginsOperations::findPlugin(const QString& folderPath, bsaRequired bsaType)
{
    QDirIterator it(folderPath);
    QStringList espName;
    QString bsaName;

    while (it.hasNext())
    {
        it.next();

        if(it.fileName().contains(QRegularExpression("\\.es[plm]$")))
        {
            espName << it.fileName();
            QLogger::QLog_Note("PluginsOperations", tr("Esp found: ") + espName.last());
        }

        if(it.fileName().endsWith(".bsa", Qt::CaseInsensitive))
            bsaName = it.fileName().chopped(4) + ".esp";
    }
    if(!bsaName.isEmpty() && espName.isEmpty())
        espName << bsaName;

    if(espName.isEmpty())
    {
        espName << QDir(folderPath).dirName() + ".esp";
        QLogger::QLog_Debug("PluginsOperations", "Using: " + espName.last() + " as esp name.");
    }

    QString returnedEsp;
    int counter = 0;

    do
    {
        for(auto esp : espName)
        {
            QFile bsa (esp.chopped(4) + ".bsa");
            QFile texturesBsa(esp.chopped(4) + " - Textures.bsa");

            bool isGood = (!texturesBsa.exists() && (bsaType == bsaRequired::texturesBsa || bsaType == bsaRequired::texturesAndStandardBsa))
                    || (!bsa.exists() && (bsaType == bsaRequired::standardBsa || bsaType == bsaRequired::texturesAndStandardBsa));

            if(isGood)
                returnedEsp = esp;
        }
        if(returnedEsp.isEmpty())
            returnedEsp = espName.last().chopped(4) + QString::number(counter) + ".esp";

        ++counter;
    }while(returnedEsp.isEmpty());

    return returnedEsp.remove(QRegularExpression("\\.es[plm]$"));
}


