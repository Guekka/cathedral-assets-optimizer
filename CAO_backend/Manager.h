/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.h"
#include "MainOptimizer.h"

enum OptimizationMode { singleMod = 0, severalMods = 1};

struct FilePathSize
{
    QString filepath;
    uint size;
};

class Manager : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor that will perform a number of functions
     */
    Manager();
    /*!
     * \brief The main process
     */
    void runOptimization();
    /*!
     * \brief Print the progress to stdout
     */
    void printProgress();

private:

    /*!
     * \brief Parse the command line arguments and store them to the ini file, then read the ini file.
     */
    void parseArguments();
    /*!
     * \brief Reads the ini file and import its content into the options struct
     */
    void readIni();
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
     * \brief The files to process
     */
    QVector<FilePathSize> files;
    /*!
     * \brief The animations to process. Separated, since they don't support multithreading
     */
    QVector<FilePathSize> animations;
    /*!
     * \brief The BSAs to extract
     */
    QVector<FilePathSize> BSAs;
    /*!
     * \brief List all the files in the modsToProcess list and store them. Also add their weights to filesWeight
     */
    void listFiles(const bool &calculateFileWeight);
    /*!
     * \brief The weight of all files. Used to determine progress
     */
    int filesWeight = 0;
    /*!
     * \brief The weight of completed files. Used to determine progress
     */
    int completedFilesWeight = 0;
    /*!
     * \brief Reset the ini
     */
    void resetIni();
    /*!
     * \brief Check if all the requirements are present
     * \return A bool indicating the success
     */
    bool checkRequirements();
    /*!
     * \brief Read the common ini and set the game
     */
    void setGame();
    /*!
     * \brief Read ignoredMods.txt and store it to a list
     */
    void readIgnoredMods();
    /*!
     * \brief The optimization options, that will be given to the MainOptimizer
     */
    OptOptionsCAO options;
    /*!
     * \brief The optimization mode
     */
    OptimizationMode mode;
    /*!
      * \brief The path given by the user
      */
    QString userPath{};
    /*!
      * \brief The list of directories to process
      */
    QVector<FilePathSize> modsToProcess{};
    /*!
     * \brief Mods on this list won't be processed
     */
    QStringList ignoredMods;
    /*!
     * \brief Used to read the INI
     */
    QSettings *settings;
};

