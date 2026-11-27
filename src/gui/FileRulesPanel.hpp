/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "gui/file_rule_preview.hpp"
#include "settings/profile.hpp"
#include "settings/settings.hpp"

#include <QAbstractItemModel>
#include <QFutureWatcher>
#include <QListWidget>
#include <QListWidgetItem>
#include <QTimer>
#include <QWidget>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace Ui {
class FileRulesPanel;
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

class FileRulesPanel final : public QWidget
{
    Q_OBJECT

public:
    explicit FileRulesPanel(Settings &settings, QWidget *parent = nullptr);

    FileRulesPanel(const FileRulesPanel &) = delete;
    FileRulesPanel(FileRulesPanel &&)      = delete;

    auto operator=(const FileRulesPanel &) -> FileRulesPanel & = delete;
    auto operator=(FileRulesPanel &&) -> FileRulesPanel      & = delete;

    ~FileRulesPanel() override;

    void refresh();

signals:
    void settings_sync_requested();
    void selected_pattern_changed();

private:
    struct PreviewKey
    {
        std::u8string input_path;
        OptimizationMode mode;
        btu::Game target_game;

        auto operator==(const PreviewKey &) const -> bool = default;
    };

    struct PreviewScanResult
    {
        std::uint64_t generation;
        FileRulePreviewResult result;
    };

    struct NoInputState
    {
    };

    struct ScanningState
    {
    };

    struct ReadyState
    {
        FileRulePreviewScan scan;
        std::vector<FileRuleImpact> impacts;
    };

    struct FailedState
    {
        std::string error;
    };

    std::unique_ptr<Ui::FileRulesPanel> ui_;
    Settings &settings_; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
    QFutureWatcher<PreviewScanResult> preview_watcher_;
    QTimer preview_timer_;
    std::optional<PreviewKey> preview_key_;
    btu::Path preview_input_path_;
    std::uint64_t preview_generation_                                                 = 0;
    std::variant<NoInputState, ScanningState, ReadyState, FailedState> preview_state_ = NoInputState{};

    void update_patterns(std::u8string_view selected_pattern);
    void select_pattern(std::u8string pattern);
    void start_preview_scan();
    void move_pattern(const QModelIndex, int source_row, int, const QModelIndex, int destination_row);
    void create_pattern();
    void edit_current_pattern();
    void delete_current_pattern();
    void update_sample_result();
    void update_impact_preview();
    [[nodiscard]] auto file_count_text(std::size_t count) const -> QString;
};
} // namespace cao
