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
            espName << it.fileName();


        if(it.fileName().endsWith(".bsa", Qt::CaseInsensitive) && !it.fileName().endsWith(" - Textures.bsa", Qt::CaseInsensitive))
            bsaName = it.fileName().chopped(4) + ".esp";
    }
    if(!bsaName.isEmpty() && espName.isEmpty())
        espName << bsaName;

    if(espName.isEmpty())
        espName << QDir(folderPath).dirName() + ".esp";


    QString returnedEsp;
    int counter = 0;

    do
    {
        for(auto esp : espName)
        {
            bool texturesBsaGood = !QFile(folderPath + "/" + esp.chopped(4) + " - Textures.bsa").exists() && bsaType == bsaRequired::texturesBsa;
            bool standardBsaGood = !QFile(folderPath + "/" +esp.chopped(4) + ".bsa").exists() && bsaType == bsaRequired::standardBsa;
            bool bothBsaGood = QFile(folderPath + "/" + esp.chopped(4) + " - Textures.bsa").exists()
                    && !QFile(folderPath + "/" +esp.chopped(4) + ".bsa").exists()
                    && bsaType == bsaRequired::texturesAndStandardBsa;

            if(texturesBsaGood || standardBsaGood || bothBsaGood)
                returnedEsp = esp;
        }
        if(returnedEsp.isEmpty())
            espName << espName.last().chopped(4) + QString::number(counter) + ".esp";

        ++counter;
    }while(returnedEsp.isEmpty());

    returnedEsp.remove(QRegularExpression("\\.es[plm]$"));
    QLogger::QLog_Note("PluginsOperations", "Using: " + returnedEsp + " as esp name.");

    return returnedEsp;
}


