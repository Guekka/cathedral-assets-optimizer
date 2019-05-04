#ifndef PLUGINSOPERATIONS_H
#define PLUGINSOPERATIONS_H

#include "pch.h"
#include "QLogger/QLogger.h"

class PluginsOperations : public QObject
{
    Q_DECLARE_TR_FUNCTIONS(PluginsOperations)

public:
    /*!
     * \brief Will try to find a plugin (.esp ; .esl ; .esm) in the given folder
     * \param folderPath The folder to check
     * \return a QString containing the name of the plugin. If no plugin is found, it will return the name of the directory.
     */
    static QString findPlugin(const QString& folderPath);
    /*!
     * \brief Will create enough plugins to load all BSAs
     * \param folderPath The folder to create plugins into
     */
    static void makeDummyPlugins(const QString& folderPath);
    /*!
     * \brief Checks if a bsa already has a plugin to load it
     * \param bsaPath The path of the bsa to check
     */
    static bool checkIfBsaHasPlugin(const QString& bsaPath);
};

#endif // PLUGINSOPERATIONS_H
