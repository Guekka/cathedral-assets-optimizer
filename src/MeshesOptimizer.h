#ifndef MESHESOPTIMIZER_H
#define MESHESOPTIMIZER_H

#include "pch.h"
#include "QLogger.h"

class MeshesOptimizer : public QObject
{
    Q_DECLARE_TR_FUNCTIONS(MeshesOptimizer)

public:
    MeshesOptimizer();

    void list(const QString& folderPath);
    void optimize(const QString& filePath);
    void dryOptimize(const QString& filePath);
    //void meshesTexturesCaseFix(const QString& filePath); WIP

private:
    QStringList crashingMeshes;
    QStringList otherMeshes;
    QStringList headparts;
    QStringList customHeadparts;

    bool bMeshesHeadparts{};
    bool bMeshesNecessaryOptimization{};
    bool bMeshesMediumOptimization{};
    bool bMeshesFullOptimization{};

    void cleanMeshesLists();
    void listHeadparts(const QDir& directory);
};

#endif // MESHESOPTIMIZER_H
