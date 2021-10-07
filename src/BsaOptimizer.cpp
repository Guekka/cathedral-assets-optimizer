/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "BsaOptimizer.h"
#include "OptionsCAO.h"
#include "PluginsOperations.h"

BSAOptimizer::BSAOptimizer()
{
    //Reading filesToNotPack to add them to the list.
    //Done in the constructor since the file won't change at runtime.

    QFile &&filesToNotPackFile = Profiles::getFile("FilesToNotPack.txt");

    auto lines = FilesystemOperations::readFile(filesToNotPackFile, [](QString &line) {
        line = QDir::fromNativeSeparators(line);
    });

    for (auto &&line : lines)
        filesToNotPack.emplace_back(std::move(line).toStdWString());

    if (filesToNotPack.empty()) {
        PLOG_ERROR << "FilesToNotPack.txt not found. This can cause a number of issues. For "
                      "example, for Skyrim, "
                      "animations will be packed to BSA, preventing them from being detected "
                      "by FNIS and Nemesis.";
    }
}

btu::bsa::Settings getSettings()
{
    auto sets = btu::bsa::Settings::get(Profiles::bsaGame());
    sets.maxSize = Profiles::maxBsaUncompressedSize();
    return sets;
}

void BSAOptimizer::extract(const QString &bsaPath, const bool &deleteBackup) const
{
    try {
        btu::bsa::unpack(btu::bsa::UnpackSettings{bsaPath.toStdU16String(), deleteBackup, false});
    } catch (const std::exception &e) {
        PLOG_ERROR << e.what();
        PLOG_ERROR << "An error occured during the extraction of: " + bsaPath + '\n'
                          + "Please extract it manually. The BSA was not deleted.";
        return;
    }

    if (!deleteBackup)
        backup(bsaPath);

    PLOG_INFO << "BSA successfully extracted: " + bsaPath;
}
// const std::vector<btu::bsa::Path>& files,
void handle_errors(std::vector<std::pair<btu::bsa::Path, std::string>> errs) {
    if (errs.empty())
        return;

    PLOG_WARNING << "The following files failed to be packed. They will be "
                    "renamed to *.caobad:\n";
    for (auto&& [file, err] : errs) {
        PLOG_WARNING << file.native() << " : " << err;
        std::filesystem::rename(file,
                                file.native() + BETHUTIL_BSA_STR(".caobad"));
    }
}

void BSAOptimizer::packAll(const QString &folderPath, const OptionsCAO &options) const
{
    PLOG_VERBOSE << "Packing all loose files into BSAs";

    const auto game = getSettings();
    const auto dir = folderPath.toStdU16String();

    btu::bsa::clean_dummy_plugins(dir, game);

    auto bsas = btu::bsa::split(
        dir, game,
        [this](const btu::bsa::Path& dir,
               btu::bsa::fs::directory_entry const& fileinfo) {
            return btu::bsa::defaultIsAllowedPath(dir, fileinfo) &&
                   isAllowedFile(dir, fileinfo);
        });
    btu::bsa::merge(bsas, options.bBsaLeastBSA ? btu::bsa::MergeBoth
                                               : btu::bsa::MergeIncompressible);

    for (auto &&bsa : bsas) {
        try {
            const auto files = std::vector(bsa.begin(), bsa.end());
            const auto errs = btu::bsa::write(options.bBsaCompress,
                                              std::move(bsa), game, dir);
            handle_errors(errs);
            if (options.bBsaDeleteSource) {
                std::for_each(files.begin(), files.end(), [](auto&& p) {
                    try {
                        std::filesystem::remove(p);
                    } catch (const std::exception& e) {
                        PLOG_ERROR << "Failed to remove packed file: "
                                   << p.native();
                    }
                });
            }

        } catch (const std::exception& e) {
            PLOG_ERROR << QString("An error occurred while packing BSAs: \n%2").arg(e.what());
        }
    }

    if (options.bBsaCreateDummies)
        btu::bsa::make_dummy_plugins(dir, game);
}

QString BSAOptimizer::backup(const QString &bsaPath) const
{
    QFile bsaBackupFile(bsaPath + ".bak");
    const QFile bsaFile(bsaPath);

    if (!bsaBackupFile.exists())

        while (bsaBackupFile.exists()) {
            if (bsaFile.size() == bsaBackupFile.size())
                QFile::remove(bsaBackupFile.fileName());
            else
                bsaBackupFile.setFileName(bsaBackupFile.fileName() + ".bak");
        }

    QFile::rename(bsaPath, bsaBackupFile.fileName());

    PLOG_VERBOSE << "Backuping BSA : " << bsaPath << " to " << bsaBackupFile.fileName();

    return bsaBackupFile.fileName();
}

// Should put these algorithms somewhere else

auto str_compare(bool caseSensitive = true)
{
    return [caseSensitive](char ch1, char ch2) {
        if (!caseSensitive) {
            ch1 = std::toupper(ch1);
            ch2 = std::toupper(ch2);
        }

        return ch1 == ch2;
    };
}

template<class CharT>
size_t str_find(std::basic_string<CharT> const &string,
                std::basic_string<CharT> const &snippet,
                bool caseSensitive = true,
                size_t fromPos = 0)
{
    auto pred = str_compare(caseSensitive);
    using namespace std;

    if (cbegin(string) + fromPos > cend(string))
        return std::string::npos;

    auto it = search(cbegin(string) + fromPos, cend(string), cbegin(snippet), cend(snippet), pred);

    if (it != cend(string))
        return it - cbegin(string);
    else
        return std::string::npos; // not found
}

bool BSAOptimizer::isAllowedFile([[maybe_unused]] btu::bsa::Path const &dir,
                                 btu::bsa::fs::directory_entry const &fileinfo) const
{
    for (const auto &fileToNotPack : filesToNotPack) {
        const auto &path = fileinfo.path().native();
        if (str_find(path, fileToNotPack, false) != std::string::npos)
            return false;
    }

    return true;
}
