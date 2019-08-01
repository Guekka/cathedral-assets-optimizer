/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "MainOptimizer.h"
#include "pch.h"

class Manager : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor that will perform a number of functions
     */
    explicit Manager(OptionsCAO &opt);
    /*!
     * \brief The main process
     */
    void runOptimization();
    /*!
     * \brief Print the progress to stdout
     * \param The text to display
     * \param The total number of files to process
     */
    void printProgress(const int &total, const QString &text);

private:
    /*!
     * \brief List all the directories to process
     * \param mode
     * \param userPath The path choosen by the user
     */
    void listDirectories();
    /*!
     * \brief List all the files in the modsToProcess list and store them. Also add their weights to filesWeight
     */
    void listFiles();
    /*!
     * \brief Read ignoredMods.txt and store it to a list
     */
    void readIgnoredMods();
    /*!
     * \brief The number of all files. Used to determine progress
     */
    int numberFiles = 0;
    /*!
     * \brief The number of completed files. Used to determine progress
     */
    int numberCompletedFiles = 0;
    /*!
     * \brief The optimization options, that will be given to the MainOptimizer
     */
    OptionsCAO &options;
    /*!
      * \brief The list of directories to process
      */
    QStringList modsToProcess;
    /*!
     * \brief Mods on this list won't be processed
     */
    QStringList ignoredMods;
    /*!
     * \brief Used to read the INI
     */
    QSettings *settings;
    /*!
     * \brief The files to process
     */
    QStringList files;
    /*!
     * \brief The animations to process. Separated, since they don't support multithreading
     */
    QStringList animations;
    /*!
     * \brief The BSAs to extract
     */
    QStringList BSAs;
signals:
    void progressBarTextChanged(QString, int, int);
};
