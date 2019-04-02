#ifndef PLUGINSOPERATIONS_H
#define PLUGINSOPERATIONS_H

#include "pch.h"
#include "QLogger.h"

class PluginsOperations : public QObject
{
    Q_DECLARE_TR_FUNCTIONS(PluginsOperations)

public:
    /*!
     * \brief findPlugin
     * \param folderPath
     * \return
     */
    static QString findPlugin(const QString& folderPath);
    static void makeDummyPlugins(const QString& folderPath);
};

#endif // PLUGINSOPERATIONS_H
