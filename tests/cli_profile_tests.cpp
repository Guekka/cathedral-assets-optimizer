#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "cli_profile.hpp"

#include <doctest/doctest.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>

namespace {

class TemporaryFile
{
public:
    TemporaryFile()
    {
        const auto id = std::chrono::steady_clock::now().time_since_epoch().count();
        path_         = std::filesystem::temp_directory_path()
                / ("cao-cli-profile-test-" + std::to_string(id) + ".json");
    }

    ~TemporaryFile()
    {
        std::error_code error;
        std::filesystem::remove(path_, error);
    }

    [[nodiscard]] auto path() const -> const std::filesystem::path & { return path_; }

private:
    std::filesystem::path path_;
};

} // namespace

TEST_CASE("JSON profile loading preserves profile values")
{
    TemporaryFile json_file;

    cao::Profile expected;
    expected.dry_run           = true;
    expected.gpu_index         = 3;
    expected.optimization_mode = cao::OptimizationMode::SeveralMods;
    expected.target_game       = btu::Game::FNV;
    expected.input_path        = std::filesystem::path("saved-mods");

    REQUIRE(cao::json::save_to_file(nlohmann::json(expected), json_file.path()));

    const auto loaded = cao::cli::load_profile_from_json(json_file.path());
    CHECK(loaded.dry_run == expected.dry_run);
    CHECK(loaded.gpu_index == expected.gpu_index);
    CHECK(loaded.optimization_mode == expected.optimization_mode);
    CHECK(loaded.target_game == expected.target_game);
    CHECK(loaded.input_path == expected.input_path);
}

TEST_CASE("JSON profile loading rejects missing and invalid files")
{
    TemporaryFile missing_file;
    CHECK_THROWS_AS(static_cast<void>(cao::cli::load_profile_from_json(missing_file.path())),
                    std::runtime_error);

    TemporaryFile invalid_file;
    {
        std::ofstream stream(invalid_file.path());
        REQUIRE(stream.good());
        stream << "{";
    }
    CHECK_THROWS_AS(static_cast<void>(cao::cli::load_profile_from_json(invalid_file.path())),
                    std::runtime_error);
}

TEST_CASE("per-mod profiles are isolated copies with single-mod mode")
{
    cao::Profile source;
    source.dry_run           = true;
    source.input_path        = std::filesystem::path("original-mods");
    source.optimization_mode = cao::OptimizationMode::SeveralMods;

    const auto first_path  = std::filesystem::path("first-mod");
    const auto second_path = std::filesystem::path("second-mod");
    const auto first       = cao::cli::profile_for_mod_path(source, first_path);
    const auto second      = cao::cli::profile_for_mod_path(source, second_path);

    CHECK(first.input_path == first_path);
    CHECK(first.optimization_mode == cao::OptimizationMode::SingleMod);
    CHECK(first.dry_run == source.dry_run);

    CHECK(second.input_path == second_path);
    CHECK(second.optimization_mode == cao::OptimizationMode::SingleMod);

    CHECK(source.input_path == std::filesystem::path("original-mods"));
    CHECK(source.optimization_mode == cao::OptimizationMode::SeveralMods);
}

TEST_CASE("per-mod profile rejects an empty path")
{
    const cao::Profile source;
    CHECK_THROWS_AS(static_cast<void>(cao::cli::profile_for_mod_path(source, {})), std::runtime_error);
}
