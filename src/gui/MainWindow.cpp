/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "MainWindow.hpp"

#include "AdvancedAnimationsModule.hpp"
#include "AdvancedBSAModule.hpp"
#include "AdvancedMeshesModule.hpp"
#include "AdvancedTexturesModule.hpp"
#include "GeneralBSAModule.hpp"
#include "LevelSelector.hpp"
#include "PatternsManagerWindow.hpp"
#include "ProfilesManagerWindow.hpp"
#include "SelectGPUWindow.hpp"
#include "logger.hpp"
#include "manager.hpp"
#include "ui_MainWindow.h"
#include "utils/utils.hpp"

#include <plog/Log.h>

#include <QDesktopServices>
#include <QDragEnterEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QMimeData>
#include <QUrl>

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

void set_patterns_enabled(Ui::MainWindow &ui, bool state) noexcept
{
    const std::array patterns_object = std::to_array<QWidget *>({
        ui.patterns,
        ui.managePatterns,
        ui.patternsLabel,
    });

    for (auto *widget : patterns_object)
    {
        widget->setEnabled(state);
        widget->setVisible(state);
    }
}

void set_gui_level(ModuleDisplay &modules, Ui::MainWindow &ui, const Settings &settings) noexcept
{
    modules.clear_modules();
    set_patterns_enabled(ui, /*state=*/false);

    const auto level = settings.gui.gui_mode;

    ui.manageProfiles->setHidden(level == GuiMode::QuickOptimize);

    switch (level)
    {
        case GuiMode::QuickOptimize:
        {
            break;
        }
        case GuiMode::Medium:
        {
            modules.add_module(std::make_unique<GeneralBSAModule>());
            modules.add_module(std::make_unique<AdvancedBSAModule>());
            modules.add_module(std::make_unique<AdvancedTexturesModule>());
            break;
        }
        case GuiMode::Advanced:
        {
            modules.add_module(std::make_unique<GeneralBSAModule>());
            modules.add_module(std::make_unique<AdvancedBSAModule>());
            modules.add_module(std::make_unique<AdvancedMeshesModule>());
            modules.add_module(std::make_unique<AdvancedTexturesModule>());
            modules.add_module(std::make_unique<AdvancedAnimationsModule>());
            set_patterns_enabled(ui, /*state=*/true);
            break;
        }
    }

    modules.hide_unsupported(settings.current_profile().target_game);
}

void ui_to_settings(const Ui::MainWindow &ui, const ModuleDisplay &module_display, Settings &settings)
{
    settings.gui.gui_theme = ui.actionEnableDarkTheme->isChecked() ? GuiTheme::Dark : GuiTheme::Light;

    switch (settings.gui.gui_mode)
    {
        case GuiMode::QuickOptimize:
        {
            if (ui.profiles->currentText() == "SLE")
                settings.current_profile() = Profile::make_base(btu::Game::SLE);
            else if (ui.profiles->currentText() == "SSE")
                settings.current_profile() = Profile::make_base(btu::Game::SSE);
            else
                throw UiException("Invalid profile selected");
            break;
        }
        case GuiMode::Medium:
        case GuiMode::Advanced: break; // nothing to do
    }

    settings.current_profile().input_path        = ui.inputDirTextEdit->text().toStdString();
    settings.current_profile().dry_run           = ui.dryRunCheckBox->isChecked();
    settings.current_profile().mods_blacklist    = {}; // TODO
    settings.current_profile().optimization_mode = ui.modeChooserComboBox->currentData()
                                                       .value<OptimizationMode>();

    // has to be last because may rely on GuiSettings being already filled
    for (const auto *module : module_display.get_modules())
        module->ui_to_settings(settings);

} // namespace cao

void settings_to_ui(const Settings &settings, Ui::MainWindow &ui, ModuleDisplay &module_display) noexcept
{
    set_theme(settings.gui.gui_theme);

    // Cache current index to keep selected tab if possible.
    const auto old_tab_index = module_display.current_index();

    set_gui_level(module_display, ui, settings);

    ui.inputDirTextEdit->setText(to_qstring(settings.current_profile().input_path.u8string()));
    ui.dryRunCheckBox->setChecked(settings.current_profile().dry_run);
    bool success = select_data(*ui.modeChooserComboBox, settings.current_profile().optimization_mode);
    assert(success);

    const auto profiles = [&settings]() -> std::vector<std::u8string_view> {
        using namespace std::literals;

        switch (settings.gui.gui_mode)
        {
            case GuiMode::QuickOptimize: return {u8"SLE"sv, u8"SSE"sv};
            case GuiMode::Medium: [[fallthrough]];
            case GuiMode::Advanced: [[fallthrough]];
            default: return settings.list_profiles();
        }
    }();

    set_items(*ui.profiles, profiles, to_qstring);

    set_items(*ui.patterns, settings.current_profile().per_file_settings(), [](const auto *pfs) {
        return to_qstring(pfs->pattern.text());
    });

    success = select_text(*ui.patterns, to_qstring(current_per_file_settings(settings).pattern.text()));
    assert(success);

    success = select_text(*ui.profiles, to_qstring(settings.current_profile_name()));
    if (!success)
        success = select_text(*ui.profiles, to_qstring(profiles.at(0)));

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
    module_display_.set_tab_widget(ui_->tabWidget);

    setAcceptDrops(true);

    // Setting data for widgets
    set_data(*ui_->modeChooserComboBox, tr("One mod"), OptimizationMode::SingleMod);
    set_data(*ui_->modeChooserComboBox, tr("Several mods"), OptimizationMode::SeveralMods);

    // Connecting widgets that do not depend on current profile
    connect(ui_->manageProfiles, &QPushButton::pressed, this, [this] {
        ProfilesManagerWindow profiles_manager(settings_);
        profiles_manager.exec();
        settings_to_ui(settings_, *ui_, module_display_);
    });

    connect(ui_->managePatterns, &QPushButton::pressed, this, [this] {
        save_settings();
        PatternsManagerWindow patterns_manager(settings_);
        patterns_manager.exec();
        settings_to_ui(settings_, *ui_, module_display_);
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
        if (!settings_.set_current_profile(to_u8string(text)))
        {
            QMessageBox::critical(this, tr("Error"), tr("Could not set the current profile. Please restart the application"));
        }
        // Reset selected pattern to default.
        settings_.gui.selected_pattern = k_default_pattern.text();
        settings_to_ui(settings_, *ui_, module_display_);
    });

    connect(ui_->patterns, &QComboBox::activated, this, [this]() {
        save_settings();
        settings_.gui.selected_pattern = to_u8string(ui_->patterns->currentText());
        settings_to_ui(settings_, *ui_, module_display_);
    });

    connect(ui_->processButton, &QPushButton::pressed, this, &MainWindow::init_process);

    // Menu buttons
    connect(ui_->actionChange_level, &QAction::triggered, this, &MainWindow::run_gui_selector);

    auto url_opener = [](const char *url) { return [url] { QDesktopServices::openUrl(QUrl(url)); }; };

    connect(ui_->actionDocumentation, &QAction::triggered, this, url_opener(k_nexus_url));
    connect(ui_->actionDiscord, &QAction::triggered, this, url_opener(k_discord_url));
    connect(ui_->actionDonate, &QAction::triggered, this, url_opener(k_kofi_url));

    connect(ui_->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui_->actionAbout_Qt, &QAction::triggered, this, &QApplication::aboutQt);

    first_start(settings_.gui.first_run);

    if (settings_.gui.remember_gui_mode)
        settings_to_ui(settings_, *ui_, module_display_);
    else
        run_gui_selector();
}

/// @brief Checks if the settings are valid. Displays a message box if they are not.
/// @param settings Settings to check
/// @return True if the settings are valid, false otherwise
[[nodiscard]] auto check_settings(const Settings &settings) noexcept -> bool
{
    const auto &profile = settings.current_profile();

    if (profile.input_path.empty())
    {
        QMessageBox::critical(nullptr, QObject::tr("Error"), QObject::tr("The input path is empty"));
        return false;
    }

    if (!exists(profile.input_path))
    {
        QMessageBox::critical(nullptr,
                              QObject::tr("Error"),
                              QObject::tr("The input path %1 does not exist")
                                  .arg(to_qstring(profile.input_path.u8string())));
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

void MainWindow::init_process()
{
    save_settings();

    if (!check_settings(settings_))
        return;

    try
    {
        progress_window_ = std::make_unique<ProgressWindow>(
            LogReader(Settings::state_directory() / k_log_file_name));

        cao_process_ = std::jthread([this](std::stop_token &&stop_token) mutable {
            auto manager = Manager();

            connect(&manager, &Manager::files_counted, progress_window_.get(), &ProgressWindow::set_maximum);

            connect(&manager,
                    &Manager::files_processed,
                    progress_window_.get(),
                    [this](const std::filesystem::path &file, size_t count_since_last) {
                        const auto text = QString("Processing %1").arg(QString::fromStdString(file.string()));
                        progress_window_->step(std::optional(text), static_cast<int>(count_since_last));
                    });

            connect(&manager, &Manager::end, progress_window_.get(), &ProgressWindow::end);

            connect(&manager, &Manager::end, this, [this] {
                graceful_stop_dialog_.reset();
                cao_process_.reset();
                ui_->processButton->setDisabled(false);
            });

            manager.run_optimization(settings_, std::move(stop_token));
        });

        progress_window_->show();

        connect(progress_window_.get(),
                &ProgressWindow::cancelled,
                this,
                &MainWindow::stop_process_gracefully);

        ui_->processButton->setDisabled(true);
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

void MainWindow::run_gui_selector()
{
    auto level_selector = LevelSelector(settings_.gui);
    settings_.gui       = level_selector.run_selection();
    settings_to_ui(settings_, *ui_, module_display_);
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
