/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "MainWindow.hpp"

#include "AdvancedAnimationsModule.hpp"
#include "AdvancedBSAModule.hpp"
#include "AdvancedMeshesModule.hpp"
#include "AdvancedTexturesModule.hpp"
#include "CustomizeOverviewModule.hpp"
#include "FileRulesPanel.hpp"
#include "GeneralBSAModule.hpp"
#include "ProfilesManagerWindow.hpp"
#include "SelectGPUWindow.hpp"
#include "logger.hpp"
#include "manager.hpp"
#include "ui_MainWindow.h"
#include "utils/utils.hpp"

#include <plog/Log.h>

#include <QCheckBox>
#include <QDesktopServices>
#include <QDragEnterEvent>
#include <QFileDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QMimeData>
#include <QSplitter>
#include <QUrl>
#include <algorithm>

namespace cao {
constexpr static inline auto k_discord_url = "https://discord.gg/SwfTzHGQcy";
constexpr static inline auto k_nexus_url   = "https://www.nexusmods.com/skyrimspecialedition/mods/23316";
constexpr static inline auto k_kofi_url    = "https://ko-fi.com/guekka";

auto get_dark_style_sheet() noexcept -> QString
{
    QFile f(":qdarkstyle/style.qss");
    if (!f.open(QFile::ReadOnly | QFile::Text))
    {
        PLOG_ERROR << "Cannot set dark style";
        return {};
    }
    return f.readAll();
}

auto set_theme(GuiTheme theme) noexcept -> bool
{
    if (theme == GuiTheme::Light)
    {
        qApp->setStyleSheet("");
    }
    else if (theme == GuiTheme::Dark)
    {
        static const QString dark_sheet = get_dark_style_sheet();

        if (dark_sheet.isEmpty())
            return false;

        qApp->setStyleSheet(dark_sheet);
    }
    return true;
}

void add_customize_modules(ModuleDisplay &modules)
{
    modules.add_module(std::make_unique<CustomizeOverviewModule>());
    modules.add_module(std::make_unique<GeneralBSAModule>());
    modules.add_module(std::make_unique<AdvancedBSAModule>());
    modules.add_module(std::make_unique<AdvancedMeshesModule>());
    modules.add_module(std::make_unique<AdvancedTexturesModule>());
    modules.add_module(std::make_unique<AdvancedAnimationsModule>());
}

void update_file_settings_context(Ui::MainWindow &ui, const Settings &settings)
{
    const auto &selected_settings = current_per_file_settings(settings);
    if (selected_settings.pattern.text() != k_default_pattern.text())
    {
        ui.settingsScopeDescription->setText(
            QObject::tr("Editing settings for %1. They apply only when this is the first matching rule.")
                .arg(pattern_display_name(selected_settings.pattern)));
        return;
    }

    auto active_rule_count = std::size_t{0};
    for (const auto *per_file_settings : settings.current_profile().per_file_settings())
    {
        if (per_file_settings->pattern.text() != k_default_pattern.text())
            ++active_rule_count;
    }

    if (active_rule_count == 0)
    {
        ui.settingsScopeDescription->setText(
            QObject::tr("These settings apply to all files in this profile."));
    }
    else
    {
        ui.settingsScopeDescription->setText(
            QObject::tr("Used when no file rule matches. %1 active rules override these defaults.")
                .arg(static_cast<qulonglong>(active_rule_count)));
    }
}

void update_quick_task_controls(Ui::MainWindow &ui)
{
    const auto task       = ui.quickTaskSelector->currentData().value<QuickTask>();
    const bool create_bsa = std::holds_alternative<CreateBsaTask>(task);
    ui.quickRemoveLooseFiles->setVisible(create_bsa);
    if (!create_bsa)
        ui.quickRemoveLooseFiles->setChecked(false);

    if (std::holds_alternative<PrepareSkyrimAssetsTask>(task))
    {
        ui.quickTaskDescription->setText(
            QObject::tr("Use CAO's recommended settings to prepare one or several Skyrim LE/SSE mods. "
                        "This task does not create or extract BSAs."));
    }
    else if (std::holds_alternative<ExtractBsaTask>(task))
    {
        ui.quickTaskDescription->setText(
            QObject::tr("Extract BSAs from the selected mod or mods. Existing loose files are not "
                        "overwritten, and other asset optimizations will not run."));
    }
    else
    {
        ui.quickTaskDescription->setText(
            QObject::tr("Pack eligible loose files into BSAs. Source files are preserved by default; "
                        "CAO may create dummy plugins so the game can load the archives."));
    }
}

auto quick_task_display_name(const QuickTask &task) -> QString
{
    if (std::holds_alternative<PrepareSkyrimAssetsTask>(task))
        return QObject::tr("Prepare Skyrim assets");
    if (std::holds_alternative<ExtractBsaTask>(task))
        return QObject::tr("Extract BSAs");
    return QObject::tr("Create BSAs");
}

auto build_run_context_summary(const Settings &settings,
                               const std::optional<QuickTask> quick_task,
                               const bool dry_run) -> QString
{
    auto summary      = QString{};
    const auto append = [&summary](const QString &line) {
        if (!summary.isEmpty())
            summary += QLatin1Char('\n');
        summary += line;
    };

    const auto &profile = settings.current_profile();
    append(QObject::tr("Input: %1").arg(to_qstring(settings.run_context.input_path.u8string())));
    append(settings.run_context.optimization_mode == OptimizationMode::SingleMod
               ? QObject::tr("Mod scope: One mod")
               : QObject::tr("Mod scope: Several mods; process each immediate subfolder separately"));
    append(QObject::tr("Target game: %1").arg(game_display_name(profile.target_game)));

    if (quick_task)
    {
        append(QObject::tr("Quick task: %1").arg(quick_task_display_name(*quick_task)));
        if (std::holds_alternative<PrepareSkyrimAssetsTask>(*quick_task))
        {
            append(QObject::tr("CAO will use its recommended Skyrim asset settings; no archive "
                               "creation or extraction is selected."));
        }
        else if (std::holds_alternative<ExtractBsaTask>(*quick_task))
        {
            append(QObject::tr("Existing loose files will not be overwritten."));
            append(dry_run ? QObject::tr("Dry run will preview extraction without extracting files or "
                                         "removing BSA archives.")
                           : QObject::tr("Successfully extracted BSA archives will be removed."));
        }
        else
        {
            append(profile.bsa_remove_files
                       ? QObject::tr("A real run will remove loose files included in verified archives.")
                       : QObject::tr("Loose source files will be preserved."));
            if (profile.bsa_make_dummy_plugins)
                append(QObject::tr(
                    "A real run may create dummy plugins so the game can load the archives."));
        }
    }
    else
    {
        append(QObject::tr("Profile: %1").arg(to_qstring(settings.current_profile_name())));
        const auto &selected_settings = current_per_file_settings(settings);
        const auto selected_scope     = selected_settings.pattern.text() == k_default_pattern.text()
                                            ? QObject::tr("Default file settings")
                                            : pattern_display_name(selected_settings.pattern);
        append(QObject::tr("File settings: %1").arg(selected_scope));
        append(
            QObject::tr("The file-impact preview shows settings routing; it does not preview processing."));

        switch (profile.bsa_operation)
        {
            case BsaOperation::None: break;
            case BsaOperation::Extract:
                append(QObject::tr("Existing loose files will not be overwritten."));
                append(dry_run ? QObject::tr("Dry run will preview extraction and asset processing without "
                                             "changing files.")
                               : QObject::tr("Successfully extracted BSA archives will be removed, then the "
                                             "selected asset settings will run."));
                break;
            case BsaOperation::Create:
                append(profile.bsa_remove_files
                           ? QObject::tr("A real run will remove loose files included in verified archives.")
                           : QObject::tr("Loose source files will be preserved."));
                if (profile.bsa_make_dummy_plugins)
                    append(QObject::tr(
                        "A real run may create dummy plugins so the game can load the archives."));
                break;
        }
    }

    append(dry_run ? QObject::tr("Action: Dry run. Planned processing will be shown without modifying files.")
                   : QObject::tr("Action: Run. Processing can modify or remove files; keep a backup."));
    return summary;
}

void ui_to_settings(const Ui::MainWindow &ui, const ModuleDisplay &module_display, Settings &settings)
{
    settings.gui.gui_theme = ui.actionEnableDarkTheme->isChecked() ? GuiTheme::Dark : GuiTheme::Light;

    settings.run_context.input_path        = ui.inputDirTextEdit->text().toStdString();
    settings.run_context.optimization_mode = ui.modeChooserComboBox->currentData().value<OptimizationMode>();
    settings.gui.quick_target_game         = ui.quickTargetGame->currentData().value<btu::Game>();

    if (settings.gui.workspace == Workspace::Customize)
    {
        settings.current_profile().mods_blacklist = {}; // TODO

        for (const auto *module : module_display.get_modules())
            module->ui_to_settings(settings);
    }
}

void settings_to_ui(const Settings &settings,
                    Ui::MainWindow &ui,
                    ModuleDisplay &module_display,
                    FileRulesPanel &file_rules_panel) noexcept
{
    set_theme(settings.gui.gui_theme);

    // Cache current index to keep selected tab if possible.
    const auto old_tab_index = module_display.current_index();

    const bool customize = settings.gui.workspace == Workspace::Customize;
    ui.workspaceTabs->setCurrentIndex(customize ? 1 : 0);
    ui.groupBox->setVisible(customize);

    ui.inputDirTextEdit->setText(to_qstring(settings.run_context.input_path.u8string()));
    bool success = select_data(*ui.modeChooserComboBox, settings.run_context.optimization_mode);
    show_message_box_on_failure(success);

    success = select_data(*ui.quickTargetGame, settings.gui.quick_target_game);
    assert(success);

    set_items(*ui.profiles, settings.list_profiles(), to_qstring);
    success = select_text(*ui.profiles, to_qstring(settings.current_profile_name()));
    assert(success);

    ui.profileTargetLabel->setText(
        QObject::tr("Target: %1").arg(game_display_name(settings.current_profile().target_game)));
    file_rules_panel.refresh();
    update_file_settings_context(ui, settings);

    module_display.hide_unsupported(settings.current_profile().target_game);
    for (auto *module : module_display.get_modules())
        module->setup(settings);

    module_display.set_current_index(old_tab_index);
}

void first_start(bool &first_run) noexcept
{
    if (!first_run)
        return;

    constexpr auto welcome
        = R"(It appears you are running CAO for the first time. All options have tooltips explaining what
          they do. If you need help, you can also <a href="%1">join us on Discord</a>.<br>
          <br>
          If you like my work, <a href="%2">please consider supporting me</a>.<br>
          Thanks for using CAO!)";

    first_run = false;
    QMessageBox box(QMessageBox::Information,
                    QObject::tr("Welcome to %1 %2")
                        .arg(QCoreApplication::applicationName(), QCoreApplication::applicationVersion()),
                    QObject::tr(welcome).arg(k_discord_url, k_kofi_url));
    box.setTextFormat(Qt::TextFormat::RichText);
    box.exec();
}

MainWindow::MainWindow(Settings settings, QWidget *parent)
    : QMainWindow(parent)
    , settings_(std::move(settings))
    , ui_(std::make_unique<Ui::MainWindow>())
{
    ui_->setupUi(this);
    file_rules_panel_ = std::make_unique<FileRulesPanel>(settings_, ui_->fileRulesHost);
    ui_->fileRulesLayout->addWidget(file_rules_panel_.get());

    auto *splitter = new QSplitter(Qt::Horizontal, ui_->customizePage);
    ui_->customizeContentLayout->removeWidget(ui_->fileRulesHost);
    ui_->customizeContentLayout->removeWidget(ui_->tabWidget);
    splitter->addWidget(ui_->fileRulesHost);
    splitter->addWidget(ui_->tabWidget);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({360, 540});
    ui_->customizeContentLayout->addWidget(splitter);

    module_display_.set_tab_widget(ui_->tabWidget);
    add_customize_modules(module_display_);

    setAcceptDrops(true);

    // Setting data for run-context widgets
    set_data(*ui_->modeChooserComboBox, tr("One mod"), OptimizationMode::SingleMod);
    set_data(*ui_->modeChooserComboBox, tr("Several mods"), OptimizationMode::SeveralMods);
    set_data(*ui_->quickTaskSelector, tr("Prepare Skyrim assets"), QuickTask{PrepareSkyrimAssetsTask{}});
    set_data(*ui_->quickTaskSelector, tr("Extract BSAs"), QuickTask{ExtractBsaTask{}});
    set_data(*ui_->quickTaskSelector, tr("Create BSAs"), QuickTask{CreateBsaTask{}});
    set_data(*ui_->quickTargetGame, tr("Skyrim LE (2011)"), btu::Game::SLE);
    set_data(*ui_->quickTargetGame, tr("Skyrim SE (2016)"), btu::Game::SSE);
    update_quick_task_controls(*ui_);

    // Connecting widgets that do not depend on current profile
    connect(ui_->manageProfiles, &QPushButton::pressed, this, [this] {
        save_settings();
        ProfilesManagerWindow profiles_manager(settings_);
        profiles_manager.exec();
        const auto current_settings = settings_.current_profile().per_file_settings();
        const auto selected_pattern = std::ranges::find(current_settings,
                                                        settings_.gui.selected_pattern,
                                                        [](const auto *per_file_settings) {
                                                            return per_file_settings->pattern.text();
                                                        });
        if (selected_pattern == current_settings.end())
            settings_.gui.selected_pattern = k_default_pattern.text();
        settings_to_ui(settings_, *ui_, module_display_, *file_rules_panel_);
    });

    connect(file_rules_panel_.get(), &FileRulesPanel::settings_sync_requested, this, [this] {
        ui_to_settings(*ui_, module_display_, settings_);
    });
    connect(file_rules_panel_.get(), &FileRulesPanel::selected_pattern_changed, this, [this] {
        update_file_settings_context(*ui_, settings_);
        for (auto *module : module_display_.get_modules())
            module->setup(settings_);
    });

    connect(ui_->actionSelect_GPU, &QAction::triggered, this, [this] {
        auto window = SelectGPUWindow();
        window.set_selected_index(settings_.current_profile().gpu_index);
        window.exec();

        const auto idx = window.get_selected_index();
        if (window.result() == QDialog::Accepted && idx.has_value())
            settings_.current_profile().gpu_index = idx.value();
    });

    connect(ui_->actionOpen_config_dir, &QAction::triggered, this, [this] {
        const auto dir = to_qstring(btu::fs::absolute(settings_.config_directory()).u8string());
        QDesktopServices::openUrl(QUrl("file:///" + dir));
    });

    connect(ui_->userPathButton, &QPushButton::pressed, this, [this] {
        const auto &current_path = ui_->inputDirTextEdit->text();

        const QString dir = QFileDialog::getExistingDirectory(this,
                                                              tr("Open Directory"),
                                                              current_path,
                                                              QFileDialog::ShowDirsOnly
                                                                  | QFileDialog::DontResolveSymlinks);

        if (dir.isEmpty())
            return;

        ui_->inputDirTextEdit->setText(dir);
    });

    connect(ui_->quickTargetGame, &QComboBox::activated, this, [this] {
        settings_.gui.quick_target_game = ui_->quickTargetGame->currentData().value<btu::Game>();
    });
    connect(ui_->quickTaskSelector, &QComboBox::currentIndexChanged, this, [this] {
        update_quick_task_controls(*ui_);
    });

    auto &common_settings = settings_.gui;
    ui_->actionEnableDarkTheme->setChecked(common_settings.gui_theme == GuiTheme::Dark);
    set_theme(common_settings.gui_theme);

    connect(ui_->actionEnableDarkTheme, &QAction::triggered, this, [this, &common_settings](bool state) {
        const GuiTheme theme      = state ? GuiTheme::Dark : GuiTheme::Light;
        common_settings.gui_theme = theme;
        ui_->actionEnableDarkTheme->setChecked(common_settings.gui_theme == GuiTheme::Dark);
        set_theme(theme);
    });

    connect(ui_->profiles, &QComboBox::activated, this, [this](int idx) {
        const QString &text = ui_->profiles->itemText(idx);
        save_settings();
        if (!settings_.set_current_profile(to_u8string(text)))
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("Could not set the current profile. Please restart the application"));
        }
        // Reset selected pattern to default.
        settings_.gui.selected_pattern = k_default_pattern.text();
        settings_to_ui(settings_, *ui_, module_display_, *file_rules_panel_);
    });

    connect(ui_->inputDirTextEdit, &QLineEdit::textChanged, this, [this](const QString &path) {
        settings_.run_context.input_path = path.toStdString();
        file_rules_panel_->refresh();
    });

    connect(ui_->modeChooserComboBox, &QComboBox::activated, this, [this] {
        settings_.run_context.optimization_mode = ui_->modeChooserComboBox->currentData()
                                                      .value<OptimizationMode>();
        file_rules_panel_->refresh();
    });

    connect(ui_->previewButton, &QPushButton::pressed, this, [this] { init_process(true); });
    connect(ui_->processButton, &QPushButton::pressed, this, [this] { init_process(false); });

    auto url_opener = [](const char *url) { return [url] { QDesktopServices::openUrl(QUrl(url)); }; };

    connect(ui_->actionDocumentation, &QAction::triggered, this, url_opener(k_nexus_url));
    connect(ui_->actionDiscord, &QAction::triggered, this, url_opener(k_discord_url));
    connect(ui_->actionDonate, &QAction::triggered, this, url_opener(k_kofi_url));

    connect(ui_->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui_->actionAbout_Qt, &QAction::triggered, this, &QApplication::aboutQt);

    first_start(settings_.gui.first_run);
    settings_to_ui(settings_, *ui_, module_display_, *file_rules_panel_);

    connect(ui_->workspaceTabs, &QTabWidget::currentChanged, this, [this](const int index) {
        ui_to_settings(*ui_, module_display_, settings_);
        settings_.gui.workspace = index == 0 ? Workspace::QuickTasks : Workspace::Customize;
        settings_to_ui(settings_, *ui_, module_display_, *file_rules_panel_);
        save_settings();
    });
}

/// @brief Checks if the settings are valid. Displays a message box if they are not.
/// @param settings Settings to check
/// @return True if the settings are valid, false otherwise
[[nodiscard]] auto check_settings(const Settings &settings) noexcept -> bool
{
    const auto &profile = settings.current_profile();
    const auto &context = settings.run_context;

    if (context.input_path.empty())
    {
        QMessageBox::critical(nullptr, QObject::tr("Error"), QObject::tr("The input path is empty"));
        return false;
    }

    if (!exists(context.input_path))
    {
        QMessageBox::critical(nullptr,
                              QObject::tr("Error"),
                              QObject::tr("The input path %1 does not exist")
                                  .arg(to_qstring(context.input_path.u8string())));
        return false;
    }

    if (profile.per_file_settings().empty())
    {
        QMessageBox::critical(nullptr,
                              QObject::tr("Error"),
                              QObject::tr("Per file settings are empty. If you don't know how to fix this "
                                          "issue, reset CAO by deleting %1")
                                  .arg(to_qstring(Settings::state_directory().u8string())));
        return false;
    }

    // TODO: refactor to increase extensibility and add more checks. The more, the better.
    return true;
}

MainWindow::~MainWindow() = default;

void MainWindow::init_process(const bool dry_run)
{
    auto quick_task   = settings_.gui.workspace == Workspace::QuickTasks
                            ? std::optional<QuickTask>{ui_->quickTaskSelector->currentData().value<QuickTask>()}
                            : std::nullopt;
    auto *create_task = quick_task ? std::get_if<CreateBsaTask>(&*quick_task) : nullptr;
    if (create_task != nullptr)
        create_task->remove_packed_files = ui_->quickRemoveLooseFiles->isChecked();
    const bool remove_packed_files = create_task != nullptr && create_task->remove_packed_files;
    if (!dry_run && remove_packed_files)
    {
        const auto choice
            = QMessageBox::warning(this,
                                   tr("Remove packed source files?"),
                                   tr("After each archive is created and verified, CAO will remove the loose "
                                      "files it contains. This cannot be undone. Continue?"),
                                   QMessageBox::Yes | QMessageBox::No,
                                   QMessageBox::No);
        if (choice != QMessageBox::Yes)
            return;
    }

    settings_.run_context.dry_run = dry_run;
    save_settings();

    auto quick_profile = std::optional<Profile>{};
    if (quick_task)
        quick_profile = Profile::make_quick_task(settings_.gui.quick_target_game, *quick_task);

    try
    {
        auto run_context_settings                = settings_;
        run_context_settings.run_context.dry_run = dry_run;
        auto run_settings = make_run_settings(run_context_settings, std::move(quick_profile));
        if (!check_settings(run_settings))
            return;

        const auto run_context = build_run_context_summary(run_settings, quick_task, dry_run);
        if (!dry_run)
        {
            QMessageBox confirmation(QMessageBox::Warning,
                                     tr("Review this run"),
                                     run_context,
                                     QMessageBox::NoButton,
                                     this);
            auto *run_button    = confirmation.addButton(tr("Run"), QMessageBox::AcceptRole);
            auto *cancel_button = confirmation.addButton(QMessageBox::Cancel);
            confirmation.setDefaultButton(cancel_button);
            confirmation.exec();
            if (confirmation.clickedButton() != run_button)
                return;
        }

        settings_.run_context.dry_run = dry_run;
        save_settings();

        progress_window_ = std::make_unique<ProgressWindow>(
            LogReader(Settings::state_directory() / k_log_file_name));
        progress_window_->set_run_context(run_context, dry_run);

        cao_process_ = std::jthread([this, run_settings = std::move(run_settings), quick_task](
                                        std::stop_token &&stop_token) mutable {
            auto manager = Manager();

            connect(&manager,
                    &Manager::stage_changed,
                    progress_window_.get(),
                    &ProgressWindow::set_stage);
            connect(&manager, &Manager::files_counted, progress_window_.get(), &ProgressWindow::set_maximum);

            connect(&manager,
                    &Manager::files_processed,
                    progress_window_.get(),
                    [this](const std::filesystem::path &file, size_t count_since_last) {
                        const auto text = file == std::filesystem::path{"archive packing"}
                                              ? tr("Packing archives")
                                              : tr("Processing %1")
                                                    .arg(QString::fromStdString(file.string()));
                        progress_window_->step(std::optional(text), static_cast<int>(count_since_last));
                    });

            connect(&manager, &Manager::end, progress_window_.get(), &ProgressWindow::finish);

            connect(&manager, &Manager::end, this, [this](bool) {
                graceful_stop_dialog_.reset();
                cao_process_.reset();
                ui_->processButton->setDisabled(false);
                ui_->previewButton->setDisabled(false);
            });

            manager.run_optimization(std::move(run_settings), std::move(stop_token), quick_task);
        });

        progress_window_->show();

        connect(progress_window_.get(),
                &ProgressWindow::cancelled,
                this,
                &MainWindow::stop_process_gracefully);

        ui_->processButton->setDisabled(true);
        ui_->previewButton->setDisabled(true);
    }
    catch (const std::exception &e)
    {
        QMessageBox box(QMessageBox::Critical,
                        tr("Error"),
                        tr("An exception has been encountered and the process was forced to stop: %1")
                            .arg(e.what()));
        box.exec();
        stop_process_gracefully();
    }
}

void MainWindow::stop_process_gracefully()
{
    if (cao_process_)
    {
        cao_process_->request_stop();

        graceful_stop_dialog_ = std::make_unique<QProgressDialog>();
        graceful_stop_dialog_->setLabelText(
            tr("Waiting for the process to end... Please do not close the application."));
        graceful_stop_dialog_->setRange(0, 0);
        graceful_stop_dialog_->setModal(/*modal*/ true);
        graceful_stop_dialog_->setCancelButton(nullptr);

        graceful_stop_dialog_->show();
    }
}

void MainWindow::save_settings() noexcept
{
    ui_to_settings(*ui_, module_display_, settings_);

    if (!cao::save_settings(settings_))
    {
        QMessageBox::critical(this, tr("Error"), tr("Your settings could not be saved to disk."));
    }
}

void MainWindow::about() noexcept
{
    constexpr auto message =
        R"( %1 version %2 built on %3
            Made by G'k
            This program is distributed in the hope that it will be useful but WITHOUT ANY WARRANTY.
            See the Mozilla Public License)";

    const QString text = tr(message).arg(QCoreApplication::applicationName(),
                                         QCoreApplication::applicationVersion(),
                                         __DATE__);

    QMessageBox::about(this, tr("About"), text);
}

[[maybe_unused]] void MainWindow::closeEvent(QCloseEvent *event)
{
    stop_process_gracefully();
    save_settings();
    event->accept();
    exit(0);
}

[[maybe_unused]] void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls())
        e->acceptProposedAction();
}

[[maybe_unused]] void MainWindow::dropEvent(QDropEvent *e)
{
    const QString &file_name = e->mimeData()->urls().at(0).toLocalFile();
    if (std::filesystem::is_directory(file_name.toStdString()))
        ui_->inputDirTextEdit->setText(QDir::cleanPath(file_name));
}
} // namespace cao
