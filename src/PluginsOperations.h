#ifndef PLUGINSOPERATIONS_H
#define PLUGINSOPERATIONS_H

#include "pch_core.h"
#include "QLogger.h"

enum bsaRequired
{
    texturesBsa = 0,
    standardBsa,
    texturesAndStandardBsa
};

class PluginsOperations : public QObject
{
    Q_DECLARE_TR_FUNCTIONS(PluginsOperations)

public:
    /*!
     * \brief Will try to find a plugin (.esp ; .esl ; .esm) in the given folder. It will always return an esp that isn't associated to a BSA OR a textures BSA.
     * \param folderPath The folder to check
     * \param bsaType The type of BSA to load.
     * \return a QString containing the name of the plugin. If no plugin is found, it will return the name of the directory.
     */
    static QString findPlugin(const QString& folderPath, bsaRequired bsaType);
    /*!
     * \brief Will create enough plugins to load all BSAs
     * \param folderPath The folder to create plugins into
     */
    static void makeDummyPlugins(const QString& folderPath);
};

#endif // PLUGINSOPERATIONS_H
