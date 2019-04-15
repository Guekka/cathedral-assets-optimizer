#ifndef MESHESOPTIMIZER_H
#define MESHESOPTIMIZER_H

#include "pch_core.h"
#include "QLogger.h"

class MeshesOptimizer : public QObject
{
    Q_DECLARE_TR_FUNCTIONS(MeshesOptimizer)

public:
    /*!
     * \brief Constructor that will read CustomHeadparts.txt and read settings from file
     */
    MeshesOptimizer();
    /*!
     * \brief List all the meshes that need to be optimized in the directory and split them between several lists according to their type
     * \param folderPath The folder to analyze
     */
    void list(const QString& folderPath);
    /*!
     * \brief Optimize the providen mesh according to its type
     * \param filePath The path of the mesh to optimize
     */
    void optimize(const QString& filePath);
    /*!
     * \brief Report the optimization that would be made on the file
     * \param filePath The path of the mesh to optimize
     */
    void dryOptimize(const QString& filePath);

private:
    QStringList crashingMeshes;
    QStringList otherMeshes;
    QStringList headparts;
    QStringList customHeadparts;

    bool bMeshesHeadparts{};
    int iMeshesOptimizationLevel{};

    void cleanMeshesLists();
    void listHeadparts(const QDir& directory);
};

#endif // MESHESOPTIMIZER_H
