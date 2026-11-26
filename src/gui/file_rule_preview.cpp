/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "file_rule_preview.hpp"

#include <btu/bsa/settings.hpp>
#include <btu/modmanager/mod_manager.hpp>

#include <algorithm>
#include <mutex>
#include <stdexcept>
#include <utility>

namespace cao {
namespace {
struct CollectedModFiles
{
    std::vector<std::filesystem::path> files;
    std::size_t skipped_archives = 0;
};

class FilePathVisitor final : public btu::modmanager::ModFolderIterator
{
public:
    [[nodiscard]] auto archive_too_large(const btu::Path & /*archive_path*/,
                                         ArchiveTooLargeState /*state*/) noexcept
        -> ArchiveTooLargeAction override
    {
        const auto lock = std::scoped_lock(mutex_);
        ++skipped_archives_;
        return ArchiveTooLargeAction::Skip;
    }

    void process_file(const btu::modmanager::ModFile file) noexcept override
    {
        const auto lock = std::scoped_lock(mutex_);
        files_.push_back(file.relative_path);
    }

    [[nodiscard]] auto take() -> CollectedModFiles { return {std::move(files_), skipped_archives_}; }

private:
    std::mutex mutex_;
    std::vector<std::filesystem::path> files_;
    std::size_t skipped_archives_ = 0;
};
} // namespace

auto scan_file_rule_preview(const btu::Path &input_path,
                            const OptimizationMode mode,
                            const btu::Game target_game) -> FileRulePreviewScan
{
    auto result = FileRulePreviewScan{};

    try
    {
        if (!std::filesystem::is_directory(input_path))
        {
            result.error = "The selected input path is not a folder.";
            return result;
        }

        const auto bsa_settings      = btu::bsa::Settings::get(target_game);
        const auto collect_mod_files = [&result, &bsa_settings](const btu::Path &mod_path) {
            auto mod     = btu::modmanager::ModFolder(mod_path, bsa_settings);
            auto visitor = FilePathVisitor{};
            mod.iterate(visitor);

            auto collected = visitor.take();
            result.skipped_archives += collected.skipped_archives;
            for (auto &file : collected.files)
                result.files.push_back(std::move(file));
        };

        switch (mode)
        {
            case OptimizationMode::SingleMod: collect_mod_files(input_path); break;
            case OptimizationMode::SeveralMods:
                for (const auto &entry : std::filesystem::directory_iterator(input_path))
                {
                    if (entry.is_directory())
                        collect_mod_files(entry.path());
                }
                break;
        }

        std::ranges::sort(result.files);
    }
    catch (const std::exception &error)
    {
        result.files.clear();
        result.skipped_archives = 0;
        result.error            = error.what();
    }

    return result;
}
} // namespace cao
