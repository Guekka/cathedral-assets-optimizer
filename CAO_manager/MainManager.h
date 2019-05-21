/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <QStringList>
#include <QSettings>
#include <QDirIterator>
#include <QTextStream>
#include <QCommandLineParser>

enum OptimizationMode { singleMod = 0, severalMods = 1};

struct optOptions
{
    bool bBsaExtract{};
    bool bBsaCreate{};
    bool bBsaDeleteBackup{};
    bool bAnimationsOptimization{};
    bool bDryRun{};

    int iMeshesOptimizationLevel{};
    int iTexturesOptimizationLevel{};

    OptimizationMode mode;

    int iLogLevel{};

    QString userPath{};
    QStringList modsToProcess{};
};

class MainManager : QObject
{
public:
    MainManager();
    /*!
     * \brief Check the options and the provided file
     * \param filePath The file to check
     * \return A QString containing the argument to give to Cathedral Assets Optimizer backend in order to process the selected file
     */
    QString getArguments(const QString& filePath);

private:
    /*!
     * \brief Reads the ini file and import its content into the options struct
     */
    void  readIni();
    /*!
     * \brief Parse the command line arguments and store them to the ini file, then read the ini file.
     */
    void parseArguments();
    /*!
     * \brief Checks if the selected settings are allowed
     * \return
     */
    bool checkSettings();
    /*!
     * \brief List all the directories to process
     * \param mode
     * \param userPath The path choosen by the user
     */
    void listDirectories();
    /*!
     * \brief The optimization options
     */
    optOptions options;
    /*!
     * \brief The files to process
     */
    QFileInfoList files;
    /*!
     * \brief List all the files in the modsToProcess list and store them. Also add their weights to filesWeight
     */
    void listFiles();
    /*!
     * \brief Print the progress to stdout
     */
    void printProgress();
    /*!
     * \brief The weight of all files. Used to determine progress
     */
    int filesWeight;
    /*!
     * \brief The weight of completed files. Used to determine progress
     */
    int completedFilesWeight;
    /*!
     * \brief Reset the ini
     */
    void resetIni();

};

