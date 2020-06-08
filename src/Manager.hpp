/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "MainOptimizer.hpp"
#include "Utils/Filesystem.hpp"
#include "pch.hpp"

namespace CAO {
class Manager final : public QObject
{
    Q_OBJECT
public:
    /*!
   * \brief Constructor that will perform a number of functions
   */
    explicit Manager();
    /*!
   * \brief The main process
   */
    void runOptimization();
    /*!
   * \brief Print the progress to stdout
   * \param text The text to display
   * \param total The total number of files to process
   */
    void printProgress(int total, const QString &text);

private:
    /*!
   * \brief List all the directories to process
   */
    void listDirectories();
    /*!
   * \brief List all the files in the modsToProcess list and store them. Also add their weights to filesWeight
   */
    void listFiles();
    /*!
   * \brief The number of all files. Used to determine progress
   */
    int _numberFiles = 0;
    /*!
   * \brief The number of completed files. Used to determine progress
   */
    int _numberCompletedFiles = 0;
    /*!
    * \brief The list of directories to process
    */
    QStringList _modsToProcess;
    /*!
   * \brief Mods on this list won't be processed
   */
    QStringList _ignoredMods;
    /*!
   * \brief The files to process
   */
    QStringList _files;
    /*!
   * \brief The BSAs to extract
   */
    QStringList BSAs;
signals:
    void progressBarTextChanged(QString, int, int);
    void end();
};
} // namespace CAO
