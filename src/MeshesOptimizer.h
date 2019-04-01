#ifndef MESHESOPTIMIZER_H
#define MESHESOPTIMIZER_H

#include "pch.h"
#include "QLogger.h"

class MeshesOptimizer : public QObject
{
    Q_DECLARE_TR_FUNCTIONS(MeshesOptimizer)

public:
    MeshesOptimizer();

    void meshesList(const QString& folderPath);
    void meshesOptimize(const QString& filePath);
    //void meshesTexturesCaseFix(const QString& filePath); WIP

    bool bMeshesHeadparts{};
    bool bMeshesNecessaryOptimization{};
    bool bMeshesMediumOptimization{};
    bool bMeshesFullOptimization{};

private:
    QLogger::QLoggerManager *logManager;

    QStringList crashingMeshes;
    QStringList otherMeshes;
    QStringList headparts;
    QStringList customHeadparts;

    void cleanMeshesLists();
    void listHeadparts(const QDir& directory);
};

#endif // MESHESOPTIMIZER_H
