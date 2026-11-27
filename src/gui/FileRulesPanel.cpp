/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "FileRulesPanel.hpp"

#include "ui_FileRulesPanel.h"
#include "utils/utils.hpp"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFontMetrics>
#include <QFormLayout>
#include <QFutureWatcher>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSignalBlocker>
#include <QStringList>
#include <QtConcurrent/QtConcurrentRun>
#include <algorithm>
#include <filesystem>
#include <iterator>
#include <stdexcept>
#include <utility>

namespace cao {
namespace {
enum class MatchMode : int
{
    Simple,
    Wildcard,
    Regex,
};

[[nodiscard]] auto is_default_rule(const PerFileSettings &settings) noexcept -> bool
{
    return settings.pattern.text() == k_default_pattern.text();
}

class RuleEditorDialog final : public QDialog
{
public:
    RuleEditorDialog(const Profile &profile,
                     const PerFileSettings *current,
                     std::u8string_view initial_source,
                     QWidget *parent)
        : QDialog(parent)
        , profile_(profile)
        , current_(current)
    {
        setWindowTitle(current == nullptr ? tr("New file rule") : tr("Edit file rule"));
        setMinimumWidth(440);

        auto *form = new QFormLayout(this);
        auto *explanation
            = new QLabel(tr("Choose files by folder and extension, or use an advanced match expression."),
                         this);
        explanation->setWordWrap(true);
        form->addRow(explanation);

        name_ = new QLineEdit(this);
        name_->setPlaceholderText(tr("Optional; describe what this rule is for"));
        form->addRow(tr("Rule name"), name_);

        mode_ = new QComboBox(this);
        mode_->addItem(tr("Folder and file extension (recommended)"), static_cast<int>(MatchMode::Simple));
        mode_->addItem(tr("Glob pattern (advanced)"), static_cast<int>(MatchMode::Wildcard));
        mode_->addItem(tr("Regular expression (advanced)"), static_cast<int>(MatchMode::Regex));
        form->addRow(tr("Match files using"), mode_);

        folder_ = new QLineEdit(this);
        folder_->setPlaceholderText(tr("Any folder, or e.g. textures/interface"));
        folder_label_ = new QLabel(tr("Folder"), this);
        form->addRow(folder_label_, folder_);

        extension_ = new QLineEdit(this);
        extension_->setPlaceholderText(tr("Any file type, or e.g. dds"));
        extension_label_ = new QLabel(tr("File extension"), this);
        form->addRow(extension_label_, extension_);

        pattern_       = new QLineEdit(this);
        pattern_label_ = new QLabel(tr("Match expression"), this);
        form->addRow(pattern_label_, pattern_);

        simple_help_
            = new QLabel(tr("Folder paths can appear anywhere in a relative path and include subfolders; "
                            "extension matching ignores letter case."),
                         this);
        simple_help_->setWordWrap(true);
        form->addRow(simple_help_);

        advanced_help_
            = new QLabel(tr("Glob and regular-expression matches apply to the complete relative file path."),
                         this);
        advanced_help_->setWordWrap(true);
        form->addRow(advanced_help_);

        if (current_ == nullptr)
        {
            copy_settings_from_ = new QComboBox(this);
            for (const auto *settings : profile_.per_file_settings())
            {
                const auto label = is_default_rule(*settings) ? tr("Everything else (default)")
                                                              : pattern_display_name(settings->pattern);
                copy_settings_from_->addItem(label, to_qstring(settings->pattern.text()));
            }

            auto source_index = copy_settings_from_->findData(to_qstring(initial_source));
            if (source_index < 0)
                source_index = copy_settings_from_->findData(to_qstring(k_default_pattern.text()));
            if (source_index >= 0)
                copy_settings_from_->setCurrentIndex(source_index);

            form->addRow(tr("Copy settings from"), copy_settings_from_);
        }

        auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
        form->addRow(buttons);

        connect(mode_, &QComboBox::currentIndexChanged, this, [this] { update_mode_fields(); });
        connect(buttons, &QDialogButtonBox::accepted, this, [this] { accept_rule(); });
        connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

        if (current_ != nullptr)
            populate_from_current();

        update_mode_fields();
    }

    [[nodiscard]] auto pattern() const -> const Pattern & { return *pattern_result_; }

    [[nodiscard]] auto copy_settings_from() const -> std::u8string
    {
        return to_u8string(copy_settings_from_->currentData().toString());
    }

private:
    const Profile &profile_;
    const PerFileSettings *current_;
    QLineEdit *name_               = nullptr;
    QComboBox *mode_               = nullptr;
    QLineEdit *folder_             = nullptr;
    QLabel *folder_label_          = nullptr;
    QLineEdit *extension_          = nullptr;
    QLabel *extension_label_       = nullptr;
    QLineEdit *pattern_            = nullptr;
    QLabel *pattern_label_         = nullptr;
    QLabel *simple_help_           = nullptr;
    QLabel *advanced_help_         = nullptr;
    QComboBox *copy_settings_from_ = nullptr;
    std::optional<Pattern> pattern_result_;

    void populate_from_current()
    {
        const auto &current_pattern = current_->pattern;
        name_->setText(to_qstring(current_pattern.name()));

        if (const auto *rule = current_pattern.simple_rule())
        {
            mode_->setCurrentIndex(mode_->findData(static_cast<int>(MatchMode::Simple)));
            folder_->setText(to_qstring(rule->folder));
            extension_->setText(to_qstring(rule->extension));
            return;
        }

        const auto mode = current_pattern.type() == Pattern::Type::Regex ? MatchMode::Regex
                                                                         : MatchMode::Wildcard;
        mode_->setCurrentIndex(mode_->findData(static_cast<int>(mode)));
        pattern_->setText(to_qstring(current_pattern.text()));
    }

    void update_mode_fields()
    {
        const auto mode   = static_cast<MatchMode>(mode_->currentData().toInt());
        const bool simple = mode == MatchMode::Simple;
        folder_label_->setVisible(simple);
        folder_->setVisible(simple);
        extension_label_->setVisible(simple);
        extension_->setVisible(simple);
        pattern_label_->setVisible(!simple);
        pattern_->setVisible(!simple);
        simple_help_->setVisible(simple);
        advanced_help_->setVisible(!simple);
    }

    void accept_rule()
    {
        const auto name = to_u8string(name_->text().trimmed());
        const auto mode = static_cast<MatchMode>(mode_->currentData().toInt());
        if (mode == MatchMode::Simple && folder_->text().trimmed().isEmpty()
            && extension_->text().trimmed().isEmpty())
        {
            QMessageBox::warning(this,
                                 tr("Missing match conditions"),
                                 tr("Choose a folder or file extension."));
            return;
        }

        if (mode != MatchMode::Simple && pattern_->text().trimmed().isEmpty())
        {
            QMessageBox::warning(this, tr("Missing match expression"), tr("Enter a match expression."));
            return;
        }

        try
        {
            Pattern candidate = [&]() -> Pattern {
                switch (mode)
                {
                    case MatchMode::Simple:
                        return Pattern::simple_rule(to_u8string(folder_->text().trimmed()),
                                                    to_u8string(extension_->text().trimmed()));
                    case MatchMode::Wildcard:
                        return Pattern::wildcard(to_u8string(pattern_->text().trimmed()));
                    case MatchMode::Regex:
                        return Pattern::regular_expression(to_u8string(pattern_->text().trimmed()));
                }

                throw std::logic_error("Unknown file-rule match mode");
            }();
            candidate.set_name(name);

            for (const auto *settings : profile_.per_file_settings())
            {
                if (settings == current_)
                    continue;

                if (!name.empty() && settings->pattern.name() == std::u8string_view{name})
                {
                    QMessageBox::warning(this, tr("Duplicate rule name"), tr("Choose a different rule name."));
                    return;
                }

                if (settings->pattern.text() == candidate.text())
                {
                    QMessageBox::warning(this,
                                         tr("Duplicate rule"),
                                         tr("A rule with these match conditions already exists."));
                    return;
                }
            }

            pattern_result_ = std::move(candidate);
            QDialog::accept();
        }
        catch (const std::invalid_argument &)
        {
            QMessageBox::warning(this,
                                 tr("Invalid rule"),
                                 tr("Use a relative folder without wildcards or '..', and an extension "
                                    "without wildcards or path "
                                    "separators."));
        }
        catch (const std::regex_error &)
        {
            QMessageBox::warning(this,
                                 tr("Invalid regular expression"),
                                 tr("Check the regular expression and try again."));
        }
    }
};
} // namespace

PatternItem::PatternItem(PerFileSettings &pfs, QListWidget *parent)
    : QListWidgetItem(parent, UserType)
    , pfs_(pfs)
{
    setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled);
}

FileRulesPanel::FileRulesPanel(Settings &settings, QWidget *parent)
    : QWidget(parent)
    , ui_(std::make_unique<Ui::FileRulesPanel>())
    , settings_(settings)
{
    ui_->setupUi(this);
    ui_->impactSummaryLabel->setTextFormat(Qt::PlainText);
    ui_->editPushButton->setEnabled(false);
    ui_->removePushButton->setEnabled(false);
    ui_->defaultRuleButton->setChecked(settings_.gui.selected_pattern == k_default_pattern.text());

    connect(ui_->patterns, &QListWidget::currentRowChanged, this, [this](const int current) {
        const bool has_selection = current >= 0;
        ui_->editPushButton->setEnabled(has_selection);
        ui_->removePushButton->setEnabled(has_selection);
        if (has_selection)
        {
            const QSignalBlocker button_blocker(ui_->defaultRuleButton);
            ui_->defaultRuleButton->setChecked(false);
            const auto *selected = static_cast<const PatternItem *>(ui_->patterns->currentItem());
            select_pattern(selected->per_file_settings().pattern.text());
        }
        else if (!ui_->defaultRuleButton->isChecked())
        {
            select_pattern(k_default_pattern.text());
        }
        update_sample_result();
        update_impact_preview();
    });
    connect(ui_->patterns, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *) {
        edit_current_pattern();
    });
    connect(ui_->patterns->model(), &QAbstractItemModel::rowsMoved, this, &FileRulesPanel::move_pattern);
    connect(ui_->newPushButton, &QPushButton::pressed, this, &FileRulesPanel::create_pattern);
    connect(ui_->editPushButton, &QPushButton::pressed, this, &FileRulesPanel::edit_current_pattern);
    connect(ui_->removePushButton, &QPushButton::pressed, this, &FileRulesPanel::delete_current_pattern);
    connect(ui_->samplePathEdit, &QLineEdit::textChanged, this, [this] { update_sample_result(); });
    connect(ui_->defaultRuleButton, &QPushButton::clicked, this, [this] {
        const QSignalBlocker button_blocker(ui_->defaultRuleButton);
        const QSignalBlocker list_blocker(ui_->patterns);
        ui_->defaultRuleButton->setChecked(true);
        ui_->patterns->setCurrentRow(-1);
        ui_->editPushButton->setEnabled(false);
        ui_->removePushButton->setEnabled(false);
        select_pattern(k_default_pattern.text());
        update_sample_result();
        update_impact_preview();
    });

    preview_timer_.setSingleShot(true);
    preview_timer_.setInterval(300);
    connect(&preview_timer_, &QTimer::timeout, this, &FileRulesPanel::start_preview_scan);

    connect(&preview_watcher_, &QFutureWatcher<PreviewScanResult>::finished, this, [this] {
        auto result = preview_watcher_.result();
        if (result.generation != preview_generation_)
            return;

        if (result.result)
        {
            auto scan    = std::move(result.result).value();
            auto impacts = analyze_file_rule_impact(settings_.current_profile(), scan.files);
            preview_state_ = ReadyState{std::move(scan), std::move(impacts)};
        }
        else
        {
            preview_state_ = FailedState{std::move(result.result).error()};
        }
        update_patterns(settings_.gui.selected_pattern);
    });

    refresh();
}

void FileRulesPanel::refresh()
{
    if (settings_.gui.workspace != Workspace::Customize)
        return;

    const auto input_path = settings_.run_context.input_path;
    const auto key        = PreviewKey{
               .input_path  = input_path.u8string(),
               .mode        = settings_.run_context.optimization_mode,
               .target_game = settings_.current_profile().target_game,
    };

    if (!preview_key_ || *preview_key_ != key)
    {
        preview_key_        = key;
        preview_input_path_ = input_path;
        ++preview_generation_;
        preview_timer_.stop();

        if (key.input_path.empty())
        {
            preview_state_ = NoInputState{};
        }
        else
        {
            preview_state_ = ScanningState{};
            preview_timer_.start();
        }
    }

    update_patterns(settings_.gui.selected_pattern);
}

void FileRulesPanel::start_preview_scan()
{
    if (!preview_key_ || preview_key_->input_path.empty())
        return;

    const auto generation = preview_generation_;
    const auto key        = *preview_key_;
    const auto input_path = preview_input_path_;
    preview_watcher_.setFuture(
        QtConcurrent::run([generation, input_path, mode = key.mode, target_game = key.target_game] {
            return PreviewScanResult{
                generation,
                scan_file_rule_preview(input_path, mode, target_game),
            };
        }));
}

void FileRulesPanel::select_pattern(std::u8string pattern)
{
    if (settings_.gui.selected_pattern == pattern)
        return;

    emit settings_sync_requested();
    settings_.gui.selected_pattern = std::move(pattern);
    emit selected_pattern_changed();
}

FileRulesPanel::~FileRulesPanel() = default;

void FileRulesPanel::update_patterns(const std::u8string_view selected_pattern)
{
    auto *ready = std::get_if<ReadyState>(&preview_state_);
    if (ready != nullptr)
        ready->impacts = analyze_file_rule_impact(settings_.current_profile(), ready->scan.files);

    auto &list = *ui_->patterns;
    const QSignalBlocker list_blocker(&list);
    const QSignalBlocker default_button_blocker(ui_->defaultRuleButton);
    list.setCurrentItem(nullptr);
    list.clear();

    int row                = 0;
    int selected_row       = -1;
    auto per_file_settings = settings_.current_profile().per_file_settings();
    for (PerFileSettings *pfs : per_file_settings)
    {
        if (is_default_rule(*pfs))
            continue;

        auto *item             = new PatternItem(*pfs, &list);
        const auto description = pattern_match_description(pfs->pattern);
        const auto name        = pattern_display_name(pfs->pattern);
        auto item_text         = tr("%1. %2").arg(row + 1).arg(name);
        if (ready != nullptr && static_cast<std::size_t>(row) < ready->impacts.size())
        {
            const auto &impact        = ready->impacts[static_cast<std::size_t>(row)];
            item_text                 = tr("%1 (%2)").arg(item_text, file_count_text(impact.applied_files));
            const auto shadowed_files = impact.matching_files - impact.applied_files;
            auto impact_text = tr("This rule is applied to %1.").arg(file_count_text(impact.applied_files));
            if (shadowed_files > 0)
                impact_text += tr(" It matches %1 that earlier rules handle.")
                                   .arg(file_count_text(shadowed_files));
            item->setToolTip(QString("%1\n%2").arg(description, impact_text));
        }
        else
        {
            item->setToolTip(description);
        }
        item->setText(item_text);
        if (pfs->pattern.text() == selected_pattern)
            selected_row = row;
        ++row;
    }

    if (selected_row < 0 && selected_pattern != k_default_pattern.text() && list.count() > 0)
        selected_row = 0;
    if (selected_row >= 0)
    {
        ui_->defaultRuleButton->setChecked(false);
        list.setCurrentRow(selected_row);
    }
    else
    {
        ui_->defaultRuleButton->setChecked(true);
    }
    ui_->editPushButton->setEnabled(selected_row >= 0);
    ui_->removePushButton->setEnabled(selected_row >= 0);

    auto default_rule_text = tr("Everything else (default)");
    if (ready != nullptr && !ready->impacts.empty())
        default_rule_text += tr(" (%1)").arg(file_count_text(ready->impacts.back().applied_files));
    ui_->defaultRuleButton->setText(default_rule_text);

    update_sample_result();
    update_impact_preview();
}

void FileRulesPanel::move_pattern(
    const QModelIndex, const int source_row, int, const QModelIndex, const int destination_row)
{
    const auto *selected        = static_cast<const PatternItem *>(ui_->patterns->currentItem());
    const auto selected_pattern = selected == nullptr ? std::u8string{}
                                                      : selected->per_file_settings().pattern.text();

    emit settings_sync_requested();
    settings_.current_profile().move_per_file_settings(source_row, destination_row);
    update_patterns(selected_pattern);
}

void FileRulesPanel::create_pattern()
{
    const auto *selected      = static_cast<const PatternItem *>(ui_->patterns->currentItem());
    const auto initial_source = selected == nullptr ? k_default_pattern.text()
                                                    : selected->per_file_settings().pattern.text();
    RuleEditorDialog dialog(settings_.current_profile(), nullptr, initial_source, this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    const auto source_key = dialog.copy_settings_from();
    auto all_settings     = settings_.current_profile().per_file_settings();
    const auto base       = std::ranges::find(all_settings, source_key, [](const auto *pfs) {
        return pfs->pattern.text();
    });
    if (base == all_settings.end())
    {
        QMessageBox::critical(this, tr("Error"), tr("The selected settings source no longer exists."));
        return;
    }

    emit settings_sync_requested();
    PerFileSettings new_settings = **base;
    new_settings.pattern         = dialog.pattern();
    const auto new_pattern       = new_settings.pattern.text();
    settings_.current_profile().prepend_per_file_settings(std::move(new_settings));
    settings_.gui.selected_pattern = new_pattern;
    update_patterns(new_pattern);
    emit selected_pattern_changed();
}

void FileRulesPanel::edit_current_pattern()
{
    auto *item = static_cast<PatternItem *>(ui_->patterns->currentItem());
    if (item == nullptr)
        return;

    auto &per_file_settings = item->per_file_settings();
    const auto old_pattern  = per_file_settings.pattern.text();
    RuleEditorDialog dialog(settings_.current_profile(), &per_file_settings, old_pattern, this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    emit settings_sync_requested();
    per_file_settings.pattern = dialog.pattern();
    const auto new_pattern    = per_file_settings.pattern.text();
    if (settings_.gui.selected_pattern == old_pattern)
        settings_.gui.selected_pattern = new_pattern;
    update_patterns(new_pattern);
    if (settings_.gui.selected_pattern == new_pattern)
        emit selected_pattern_changed();
}

void FileRulesPanel::delete_current_pattern()
{
    const auto *item = static_cast<const PatternItem *>(ui_->patterns->currentItem());
    if (item == nullptr)
        return;

    const auto current_pattern = item->per_file_settings().pattern.text();
    const auto current_name    = pattern_display_name(item->per_file_settings().pattern);
    const auto button
        = QMessageBox::warning(this,
                               tr("Remove file rule"),
                               tr("Remove the rule '%1'? This action cannot be undone.").arg(current_name),
                               QMessageBox::No | QMessageBox::Yes);

    if (button != QMessageBox::Yes)
        return;

    emit settings_sync_requested();
    if (!settings_.current_profile().remove_per_file_settings(current_pattern))
    {
        QMessageBox::critical(this, tr("Error"), tr("The file rule could not be removed."));
        return;
    }

    const bool removed_selected = settings_.gui.selected_pattern == current_pattern;
    if (removed_selected)
        settings_.gui.selected_pattern = k_default_pattern.text();

    update_patterns(settings_.gui.selected_pattern);
    if (removed_selected)
        emit selected_pattern_changed();
}

void FileRulesPanel::update_sample_result()
{
    const auto sample = to_u8string(ui_->samplePathEdit->text().trimmed());
    if (sample.empty())
    {
        ui_->sampleResultLabel->setText(tr("Enter a relative path to preview rule matching."));
        return;
    }

    auto normalized_sample = sample;
    std::ranges::replace(normalized_sample, u8'\\', u8'/');
    const std::filesystem::path path{normalized_sample};
    if (path.is_absolute() || (normalized_sample.size() > 1 && normalized_sample[1] == u8':'))
    {
        ui_->sampleResultLabel->setText(tr("Use a relative file path, not an absolute path."));
        return;
    }

    for (const auto &component : path)
    {
        const bool is_parent = component == std::filesystem::path{u8".."};
        if (is_parent)
        {
            ui_->sampleResultLabel->setText(tr("The test path cannot go above the input files."));
            return;
        }
    }

    const auto all_settings      = settings_.current_profile().per_file_settings();
    const auto first_match       = std::ranges::find_if(all_settings,
                                                  [&path](const auto *pfs) { return pfs->matches(path); });
    const auto *winner           = first_match == all_settings.end() ? all_settings.back() : *first_match;
    const auto winner_is_default = is_default_rule(*winner);
    const auto winner_name       = winner_is_default ? tr("the default settings")
                                                     : pattern_display_name(winner->pattern);
    if (ui_->defaultRuleButton->isChecked())
    {
        ui_->sampleResultLabel->setText(
            winner_is_default ? tr("Default file settings will apply to this path.")
                              : tr("The default settings are overridden by %1.").arg(winner_name));
        return;
    }

    const auto *selected = static_cast<const PatternItem *>(ui_->patterns->currentItem());
    if (selected == nullptr)
    {
        ui_->sampleResultLabel->setText(tr("Select a rule or the default settings to test this path."));
        return;
    }

    const auto &selected_settings = selected->per_file_settings();
    if (!selected_settings.matches(path))
    {
        ui_->sampleResultLabel->setText(tr("This rule does not match. %1 will apply.").arg(winner_name));
    }
    else if (winner == &selected_settings)
    {
        ui_->sampleResultLabel->setText(tr("This is the first matching rule and its settings will apply."));
    }
    else
    {
        ui_->sampleResultLabel->setText(
            tr("This rule matches, but %1 comes first and will apply instead.").arg(winner_name));
    }
}

void FileRulesPanel::update_impact_preview()
{
    ui_->impactSummaryLabel->setToolTip({});
    if (std::holds_alternative<NoInputState>(preview_state_))
    {
        ui_->impactSummaryLabel->setText(
            tr("Choose an input folder in the main window to preview real files."));
        return;
    }

    if (std::holds_alternative<ScanningState>(preview_state_))
    {
        ui_->impactSummaryLabel->setText(tr("Scanning files from the selected input folder..."));
        return;
    }

    if (const auto *failed = std::get_if<FailedState>(&preview_state_))
    {
        ui_->impactSummaryLabel->setText(
            tr("The file preview could not be loaded: %1\nCheck that the input folder exists and is readable, "
               "then choose it again to retry the preview.")
                .arg(QString::fromStdString(failed->error)));
        return;
    }

    const auto *ready = std::get_if<ReadyState>(&preview_state_);
    if (ready == nullptr)
    {
        ui_->impactSummaryLabel->setText(tr("The file-rule preview is unavailable."));
        return;
    }

    if (ready->scan.files.empty())
    {
        ui_->impactSummaryLabel->setText(tr("No files were found in the selected input folder."));
        return;
    }

    if (ready->impacts.empty())
    {
        ui_->impactSummaryLabel->setText(tr("The file-rule preview is unavailable."));
        return;
    }

    const auto all_settings = settings_.current_profile().per_file_settings();
    const auto *selected    = static_cast<const PatternItem *>(ui_->patterns->currentItem());
    auto impact_index       = all_settings.size() - 1;
    if (selected != nullptr)
    {
        const auto it = std::ranges::find(all_settings, &selected->per_file_settings());
        if (it == all_settings.end())
        {
            ui_->impactSummaryLabel->setText(tr("The selected rule is no longer available."));
            return;
        }
        impact_index = static_cast<std::size_t>(std::distance(all_settings.begin(), it));
    }

    if (impact_index >= ready->impacts.size())
    {
        ui_->impactSummaryLabel->setText(tr("The file-rule preview is unavailable."));
        return;
    }

    const auto &impact = ready->impacts[impact_index];
    auto summary       = selected == nullptr
                             ? tr("Default settings apply to %1.").arg(file_count_text(impact.applied_files))
                             : QString{};
    auto full_summary  = summary;

    const auto shadowed_files = impact.matching_files > impact.applied_files
                                    ? impact.matching_files - impact.applied_files
                                    : std::size_t{0};
    if (selected != nullptr && impact.applied_files == 0 && impact.matching_files == 0)
    {
        summary = tr("No files in this folder match this rule.");
    }
    else if (selected != nullptr && impact.applied_files == 0)
    {
        summary = tr("This rule matches %1, but an earlier rule handles all of them.")
                      .arg(file_count_text(impact.matching_files));
    }
    else if (selected != nullptr)
    {
        summary = tr("This rule is used by %1 in this folder.").arg(file_count_text(impact.applied_files));
        if (shadowed_files > 0)
            summary += tr(" It also matches %1 handled by earlier rules.").arg(file_count_text(shadowed_files));
    }
    full_summary = summary;

    if (selected != nullptr)
    {
        const auto default_summary = tr("\nDefault settings apply to %1.")
                                         .arg(file_count_text(ready->impacts.back().applied_files));
        summary += default_summary;
        full_summary += default_summary;
    }

    const auto display_path = [this](const QString &path) {
        const auto width = std::max(ui_->impactSummaryLabel->width(), 400);
        return QFontMetrics{ui_->impactSummaryLabel->font()}.elidedText(path, Qt::ElideMiddle, width);
    };

    QStringList examples;
    QStringList full_examples;
    for (const auto &path : impact.example_paths)
    {
        const auto path_text = to_qstring(path.u8string());
        examples.push_back(display_path(path_text));
        full_examples.push_back(path_text);
    }
    if (!examples.isEmpty())
    {
        summary += tr("\nFiles using this rule: %1").arg(examples.join(", "));
        full_summary += tr("\nFiles using this rule: %1").arg(full_examples.join(", "));
    }

    QStringList overridden_examples;
    QStringList full_overridden_examples;
    for (const auto &path : impact.overridden_example_paths)
    {
        const auto path_text = to_qstring(path.u8string());
        overridden_examples.push_back(display_path(path_text));
        full_overridden_examples.push_back(path_text);
    }
    if (!overridden_examples.isEmpty())
    {
        summary += tr("\nFiles handled by earlier rules: %1").arg(overridden_examples.join(", "));
        full_summary += tr("\nFiles handled by earlier rules: %1").arg(full_overridden_examples.join(", "));
    }

    if (ready->scan.skipped_archives > 0)
    {
        const auto skipped_summary = ready->scan.skipped_archives == 1
                                         ? tr("\nOne oversized archive was excluded from the preview.")
                                         : tr("\n%1 oversized archives were excluded from the preview.")
                                               .arg(static_cast<qulonglong>(ready->scan.skipped_archives));
        summary += skipped_summary;
        full_summary += skipped_summary;
    }

    if (summary != full_summary)
        ui_->impactSummaryLabel->setToolTip(full_summary);
    ui_->impactSummaryLabel->setText(summary);
}

auto FileRulesPanel::file_count_text(const std::size_t count) const -> QString
{
    if (count == 1)
        return tr("1 file");

    return tr("%1 files").arg(static_cast<qulonglong>(count));
}

} // namespace cao
