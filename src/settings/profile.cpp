#include "profile.hpp"

#include <algorithm>

namespace cao {
[[nodiscard]] auto Profile::set_fnv_settings(Profile profile) noexcept -> Profile
{
    profile.bsa_allow_compression = false;
    profile.bsa_make_overrides    = true;

    profile.base_per_file_settings_.pack = false;

    auto fnv_lod_atlases         = PerFileSettings::make_base(btu::Game::FNV);
    fnv_lod_atlases.pattern      = Pattern::wildcard(u8"textures/landscape/lod/*.dds");
    fnv_lod_atlases.tex_optimize = OptimizeType::None;

    auto fnv_lod_textures         = PerFileSettings::make_base(btu::Game::FNV);
    fnv_lod_textures.pattern      = Pattern::regular_expression(u8".*lod(_[nsdgpe])?\\.dds");
    fnv_lod_textures.tex_optimize = OptimizeType::None;

    auto fnv_ui_textures         = PerFileSettings::make_base(btu::Game::FNV);
    fnv_ui_textures.pattern      = Pattern::regular_expression(u8"textures/interface/.*\\.dds");
    fnv_ui_textures.tex_optimize = OptimizeType::None;

    auto fnv_dds         = PerFileSettings::make_base(btu::Game::FNV);
    fnv_dds.pattern      = Pattern::wildcard(u8"*.dds");
    fnv_dds.tex.compress = true;
    fnv_dds.tex.resize   = btu::tex::Dimension{2048, 2048};

    auto fnv_tga         = PerFileSettings::make_base(btu::Game::FNV);
    fnv_tga.pattern      = Pattern::wildcard(u8"*.tga");
    fnv_tga.tex_optimize = OptimizeType::None;

    auto fnv_meshes         = PerFileSettings::make_base(btu::Game::FNV);
    fnv_meshes.pattern      = Pattern::wildcard(u8"*.nif");
    fnv_meshes.nif_optimize = OptimizeType::None;

    auto fnv_sounds    = PerFileSettings::make_base(btu::Game::FNV);
    fnv_sounds.pattern = Pattern::wildcard(u8"*.[ow][ga][gv]");

    profile.append_per_file_settings(fnv_lod_atlases);
    profile.append_per_file_settings(fnv_lod_textures);
    profile.append_per_file_settings(fnv_ui_textures);
    profile.append_per_file_settings(fnv_dds);
    profile.append_per_file_settings(fnv_tga);
    profile.append_per_file_settings(fnv_meshes);
    profile.append_per_file_settings(fnv_sounds);

    return profile;
}

[[nodiscard]] auto Profile::make_base(btu::Game game) noexcept -> Profile
{
    Profile profile;
    profile.target_game             = game;
    profile.base_per_file_settings_ = PerFileSettings::make_base(game);

    switch (game)
    {
        case btu::Game::FNV: return set_fnv_settings(profile);
        default: return profile;
    }
}

auto analyze_file_rule_impact(const Profile &profile,
                              const std::vector<std::filesystem::path> &files,
                              const std::size_t max_examples) -> std::vector<FileRuleImpact>
{
    const auto per_file_settings = profile.per_file_settings();
    auto impacts                 = std::vector<FileRuleImpact>(per_file_settings.size());
    auto ordered_files           = std::vector<const std::filesystem::path *>{};
    ordered_files.reserve(files.size());
    for (const auto &file : files)
        ordered_files.push_back(&file);
    std::ranges::sort(ordered_files, [](const auto *left, const auto *right) { return *left < *right; });

    for (const auto *file : ordered_files)
    {
        auto first_matching_rule = per_file_settings.size();
        for (std::size_t index = 0; index < per_file_settings.size(); ++index)
        {
            if (!per_file_settings[index]->matches(*file))
                continue;

            ++impacts[index].matching_files;
            if (first_matching_rule == per_file_settings.size())
                first_matching_rule = index;
            else if (impacts[index].overridden_example_paths.size() < max_examples)
                impacts[index].overridden_example_paths.push_back(*file);
        }

        if (first_matching_rule == per_file_settings.size())
            first_matching_rule = per_file_settings.size() - 1;

        auto &applied_impact = impacts[first_matching_rule];
        ++applied_impact.applied_files;
        if (applied_impact.example_paths.size() < max_examples)
            applied_impact.example_paths.push_back(*file);
    }

    return impacts;
}

auto Profile::make_quick_task(const btu::Game game, const QuickTask task) noexcept -> Profile
{
    auto profile = make_base(game);
    if (std::holds_alternative<ExtractBsaTask>(task))
        profile.bsa_operation = BsaOperation::Extract;
    else if (const auto *create_task = std::get_if<CreateBsaTask>(&task))
    {
        profile.bsa_operation    = BsaOperation::Create;
        profile.bsa_remove_files = create_task->remove_packed_files;
    }

    return profile;
    return profile;
}
} // namespace cao