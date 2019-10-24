/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "PluginsOperations.h"
#include "Profiles.h"
#include "pch.h"

enum ScanResult
{
    doNotProcess = -1,
    good = 0,
    lightIssue = 1,
    criticalIssue = 2
};

class MeshesOptimizer final : public QObject
{
    Q_DECLARE_TR_FUNCTIONS(MeshesOptimizer)

public:
    /*!
   * \brief Constructor that will read CustomHeadparts.txt and read settings from file
   */
    MeshesOptimizer(const bool &processHeadparts, const int &optimizationLevel, const bool &resaveMeshes);
    /*!
   * \brief Scans the selected meshes for issues
   * \param nif The mesh to scan
   * \return An enum with the scan results
   */
    ScanResult scan(NifFile &nif) const;
    /*!
   * \brief Optimize the providen mesh according to its type
   * \param filePath The path of the mesh to optimize
   */
    void optimize(const QString &filepath);
    /*!
   * \brief Report the optimization that would be made on the file
   * \param filePath The path of the mesh to optimize
   */
    void dryOptimize(const QString &filepath) const;

    void listHeadparts(const QString &directory);
    /*!
     * \brief If the mesh references a TGA texture, it will replace it with DDS. 
     * \param file The mesh to process
     */
    bool renameReferencedTexturesExtension(NifFile &file);

    std::tuple<bool, NifFile> loadMesh(const QString &filepath) const;
    bool saveMesh(NifFile &nif, const QString &filepath) const;

private:
    QStringList headparts;

    bool bMeshesHeadparts;
    bool bMeshesResave;
    int iMeshesOptimizationLevel;
};
