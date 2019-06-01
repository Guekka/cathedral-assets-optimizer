/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.h"

class MeshesOptimizer : public QObject
{
    Q_DECLARE_TR_FUNCTIONS(MeshesOptimizer)

public:
    /*!
     * \brief Constructor that will read CustomHeadparts.txt and read settings from file
     */
    MeshesOptimizer(bool processHeadparts, int optimizationLevel);
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

    void listHeadparts(const QDir& directory);
private:
    QStringList headparts;

    bool bMeshesHeadparts{};
    int iMeshesOptimizationLevel{};

};
