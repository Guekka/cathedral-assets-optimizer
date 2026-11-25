/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "settings/profile.hpp"
#include "settings/settings.hpp"

#include <QAbstractItemModel>
#include <QDialog>
#include <QFutureWatcher>
#include <QListWidget>
#include <QListWidgetItem>
#include <cstddef>
#include <filesystem>
#include <string>
#include <vector>

class QComboBox;

namespace Ui {
class PatternsManagerWindow;
} // namespace Ui

namespace cao {
class Settings;

class PatternItem : public QListWidgetItem
{
public:
    explicit PatternItem(PerFileSettings &pfs, QListWidget *parent = nullptr);

    [[nodiscard]] auto per_file_settings() const noexcept -> PerFileSettings & { return pfs_; }

private:
    PerFileSettings &pfs_; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
};

class PatternsManagerWindow final : public QDialog
{
    Q_OBJECT

public:
    explicit PatternsManagerWindow(Settings &settings, QWidget *parent = nullptr);

    PatternsManagerWindow(const PatternsManagerWindow &) = delete;
    PatternsManagerWindow(PatternsManagerWindow &&)      = delete;

    auto operator=(const PatternsManagerWindow &) -> PatternsManagerWindow & = delete;
    auto operator=(PatternsManagerWindow &&) -> PatternsManagerWindow      & = delete;

    ~PatternsManagerWindow() override; // = default

private:
    struct PreviewScanResult
    {
        std::vector<std::filesystem::path> files;
        std::size_t skipped_archives = 0;
        std::string error;
    };

    enum class PreviewState
    {
        NoInput,
        Scanning,
        Ready,
        Failed,
    };

    std::unique_ptr<Ui::PatternsManagerWindow> ui_;
    Settings &settings_; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
    QFutureWatcher<PreviewScanResult> preview_watcher_;
    PreviewScanResult preview_scan_result_;
    std::vector<FileRuleImpact> rule_impacts_;
    PreviewState preview_state_ = PreviewState::NoInput;

    [[nodiscard]] static auto scan_input_files(btu::Path input_path,
                                               OptimizationMode mode,
                                               btu::Game target_game) -> PreviewScanResult;
    void update_patterns(std::u8string_view selected_pattern = {});
    void move_pattern(const QModelIndex, int source_row, int, const QModelIndex, int destination_row);
    void create_pattern();
    void edit_current_pattern();
    void delete_current_pattern();
    void update_sample_result();
    void update_impact_preview();
    [[nodiscard]] auto file_count_text(std::size_t count) const -> QString;
};
} // namespace cao
