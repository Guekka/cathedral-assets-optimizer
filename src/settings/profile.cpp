#include "profile.hpp"

namespace cao {
[[nodiscard]] auto Profile::set_fnv_settings(Profile profile) noexcept -> Profile
{
    profile.bsa_allow_compression = false;
    profile.bsa_make_overrides    = true;

    profile.base_per_file_settings_.pack = false;

    auto fnv_lod_textures         = PerFileSettings::make_base(btu::Game::FNV);
    fnv_lod_textures.pattern      = Pattern(u8".*lod(_[nsdgpe])?\\.dds", Pattern::Type::Regex);
    fnv_lod_textures.tex_optimize = OptimizeType::None;

    auto fnv_ui_textures         = PerFileSettings::make_base(btu::Game::FNV);
    fnv_ui_textures.pattern      = Pattern(u8"textures/interface/.*\\.dds", Pattern::Type::Regex);
    fnv_ui_textures.tex_optimize = OptimizeType::None;

    auto fnv_dds         = PerFileSettings::make_base(btu::Game::FNV);
    fnv_dds.pattern      = Pattern(u8"*.dds", Pattern::Type::Wildcard);
    fnv_dds.tex.compress = true;
    fnv_dds.tex.resize   = btu::tex::Dimension{2048, 2048};

    auto fnv_tga         = PerFileSettings::make_base(btu::Game::FNV);
    fnv_tga.pattern      = Pattern(u8"*.tga", Pattern::Type::Wildcard);
    fnv_tga.tex_optimize = OptimizeType::None;

    auto fnv_meshes         = PerFileSettings::make_base(btu::Game::FNV);
    fnv_meshes.pattern      = Pattern(u8"*.nif", Pattern::Type::Wildcard);
    fnv_meshes.nif_optimize = OptimizeType::None;

    auto fnv_sounds    = PerFileSettings::make_base(btu::Game::FNV);
    fnv_sounds.pattern = Pattern(u8"*.[ow][ga][gv]", Pattern::Type::Wildcard);

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
} // namespace cao