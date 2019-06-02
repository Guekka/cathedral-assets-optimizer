/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.h"

enum ScanResult { doNotProcess = -1, good = 0, lightIssue = 1, criticalIssue = 2 };

class MeshesOptimizer : public QObject
{
    Q_DECLARE_TR_FUNCTIONS(MeshesOptimizer)

public:
    /*!
     * \brief Constructor that will read CustomHeadparts.txt and read settings from file
     */
    MeshesOptimizer(bool processHeadparts, int optimizationLevel);
    /*!
     * \brief Scans the selected meshes for issues
     * \param filePath The path of the mesh to scan
     * \return An enum with the scan results
     */
    ScanResult scan(const QString& filePath);
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

    void listHeadparts(const QString &directory);
private:
    QStringList headparts;

    bool bMeshesHeadparts{};
    int iMeshesOptimizationLevel{};

};
