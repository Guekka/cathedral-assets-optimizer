/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.h"
#include "MainOptimizer.h"

enum OptimizationMode { singleMod = 0, severalMods = 1};


class Manager : QObject
{
public:
    /*!
     * \brief Constructor that will perform a number of functions
     */
    Manager();
    /*!
     * \brief The main process
     */
    void runOptimization();

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
    /*!
     * \brief Check if all the requirements are present
     * \return A bool indicating the success
     */
    bool checkRequirements();

    OptOptions options;

    OptimizationMode mode;
    QString userPath{};
    QStringList modsToProcess{};

    QSettings *settings;

};

