/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "file_rule_preview.hpp"

#include <btu/bsa/settings.hpp>
#include <btu/common/threading.hpp>
#include <btu/modmanager/mod_manager.hpp>

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
        collected_.apply([](auto &collected) { ++collected.skipped_archives; });
        return ArchiveTooLargeAction::Skip;
    }

    void process_file(const btu::modmanager::ModFile file) noexcept override
    {
        collected_.apply([&file](auto &collected) { collected.files.push_back(file.relative_path); });
    }

    [[nodiscard]] auto take() -> CollectedModFiles
    {
        return collected_.apply([](auto &collected) { return std::move(collected); });
    }

private:
    btu::common::synchronized<CollectedModFiles> collected_;
};
} // namespace

auto scan_file_rule_preview(const btu::Path &input_path,
                            const OptimizationMode mode,
                            const btu::Game target_game) -> FileRulePreviewResult
{
    auto result = FileRulePreviewScan{};

    try
    {
        if (!std::filesystem::is_directory(input_path))
            return std::unexpected<std::string>{"The selected input path is not a folder."};

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

    }
    catch (const std::exception &error)
    {
        return std::unexpected<std::string>{error.what()};
    }

    return result;
}
} // namespace cao
