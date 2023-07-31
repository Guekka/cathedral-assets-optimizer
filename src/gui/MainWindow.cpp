/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "MainWindow.hpp"

#include "AdvancedAnimationsModule.hpp"
#include "AdvancedBSAModule.hpp"
#include "AdvancedMeshesModule.hpp"
#include "AdvancedTexturesModule.hpp"
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
#include <QUrl>
#include <QtConcurrent/QtConcurrent>

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

void set_gui_level(ModuleDisplay &modules, Ui::MainWindow &ui, GuiMode level) noexcept
{
    modules.clear_modules();
    set_patterns_enabled(ui, /*state=*/false);

    switch (level)
    {
        case GuiMode::QuickOptimize:
        {
            break;
        }
        case GuiMode::Medium:
        {
            modules.add_module(std::make_unique<AdvancedBSAModule>());
            modules.add_module(std::make_unique<AdvancedTexturesModule>());
            break;
        }
        case GuiMode::Advanced:
        {
            modules.add_module(std::make_unique<AdvancedBSAModule>());
            modules.add_module(std::make_unique<AdvancedMeshesModule>());
            modules.add_module(std::make_unique<AdvancedTexturesModule>());
            modules.add_module(std::make_unique<AdvancedAnimationsModule>());
            set_patterns_enabled(ui, /*state=*/true);
            break;
        }
    }
}

void ui_to_settings(const Ui::MainWindow &ui, const ModuleDisplay &module_display, Settings &settings) noexcept
{
    settings.gui = {
        .gui_theme         = ui.actionEnableDarkTheme->isChecked() ? GuiTheme::Dark : GuiTheme::Light,
        .remember_gui_mode = settings.gui.remember_gui_mode, // changed by level selector
        .gui_mode          = settings.gui.gui_mode,
        .first_run         = false,                         // if we are here, it's not the first run anymore
        .selected_pattern  = settings.gui.selected_pattern, // TODO
        .gpu_index         = settings.gui.gpu_index,        // changed by gpu selector
    };

    settings.current_profile().input_path        = ui.inputDirTextEdit->text().toStdString();
    settings.current_profile().dry_run           = ui.dryRunCheckBox->isChecked();
    settings.current_profile().mods_blacklist    = {}; // TODO
    settings.current_profile().optimization_mode = ui.modeChooserComboBox->currentData()
                                                       .value<OptimizationMode>();
    settings.current_profile().target_game = btu::Game::SSE; // TODO

    // has to be last because may rely on GuiSettings being already filled
    for (const auto *module : module_display.get_modules())
        module->ui_to_settings(settings);

} // namespace cao

void settings_to_ui(const Settings &settings, Ui::MainWindow &ui, ModuleDisplay &module_display) noexcept
{
    set_theme(settings.gui.gui_theme);

    // TODO

    set_gui_level(module_display, ui, settings.gui.gui_mode);

    ui.profiles->clear();
    for (const auto &profile : settings.list_profiles())
        ui.profiles->addItem(QString::fromUtf8(profile.data(), static_cast<qsizetype>(profile.size())));
}

void first_start(bool &first_run) noexcept
{
    constexpr auto welcome
        = R"(It appears you are running CAO for the first time. All options have tooltips explaining what
          they do. If you need help, you can also <a href="%1">join us on Discord</a>.<br>
          <br>
          If you like my work, <a href="%2">please consider supporting me</a>.<br>
          Thanks for using CAO!)";

    if (std::exchange(first_run, true))
    {
        QMessageBox box(QMessageBox::Information,
                        QObject::tr("Welcome to %1 %2")
                            .arg(QCoreApplication::applicationName(), QCoreApplication::applicationVersion()),
                        QObject::tr(welcome).arg(k_discord_url, k_kofi_url));
        box.setTextFormat(Qt::TextFormat::RichText);
        box.exec();
    }
}

MainWindow::MainWindow()
    : ui_(std::make_unique<Ui::MainWindow>())
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
        PatternsManagerWindow patterns_manager(settings_);
        patterns_manager.exec();
        settings_to_ui(settings_, *ui_, module_display_);
    });

    auto &common_settings = settings_.gui;
    ui_->actionEnableDarkTheme->setChecked(common_settings.gui_theme == GuiTheme::Dark);
    set_theme(common_settings.gui_theme);

    connect(ui_->actionEnableDarkTheme, &QAction::triggered, [this, &common_settings](bool state) {
        const GuiTheme theme      = state ? GuiTheme::Dark : GuiTheme::Light;
        common_settings.gui_theme = theme;
        ui_->actionEnableDarkTheme->setChecked(common_settings.gui_theme == GuiTheme::Dark);
        set_theme(theme);
    });

    connect(ui_->profiles, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int idx) {
        if (!settings_.set_current_profile(idx))
            return; // TODO: error message
        // TODO
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
    run_gui_selector();
}

/// @brief Checks if the settings are valid. Displays a message box if they are not.
/// @param settings Settings to check
/// @return True if the settings are valid, false otherwise
[[nodiscard]] auto check_settings(const Settings &settings) noexcept -> bool
{
    // TODO

    // trick to avoid always true warning. This is UB, but we don't care
    return reinterpret_cast<std::uintptr_t>(&settings) != 0;
}

MainWindow::~MainWindow() = default;

/*
void MainWindow::connectThis()
{
    auto &generalSettings = currentProfile().getGeneralSettings();

    connect(ui_->actionSelect_GPU, &QAction::triggered, this, [] {
        SelectGPUWindow window;
        window.setSelectedIndex(getProfiles().commonSettings().iGPUIndex());
        window.exec();
        if (window.result() == QDialog::Accepted)
        {
            auto idx = window.getSelectedIndex();
            if (idx.has_value())
                getProfiles().commonSettings().iGPUIndex = idx.value();
        }
    });

    connect(ui_->userPathButton, &QPushButton::pressed, this, [&generalSettings, this] {
        const QString &currentPath = generalSettings.sInputPath.value_or(QDir::currentPath());

        const QString &dir = QFileDialog::getExistingDirectory(this,
                                                               tr("Open Directory"),
                                                               currentPath,
                                                               QFileDialog::ShowDirsOnly
                                                                   | QFileDialog::DontResolveSymlinks);

        if (dir.isEmpty())
            return;

        generalSettings.sInputPath = dir;
        ui_->inputDirTextEdit->setText(dir);
    });

    selectText(*ui_->patterns, generalSettings.sCurrentPattern.value_or("*"));
    connect(ui_->patterns,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            &generalSettings.sCurrentPattern,
            [this, &generalSettings](int idx) {
                generalSettings.sCurrentPattern = ui_->patterns->itemText(idx);
                reconnectThis();
            });

    const int currentModeIndex = ui_->modeChooserComboBox->findData(generalSettings.eMode());
    ui_->modeChooserComboBox->setCurrentIndex(currentModeIndex);

    connect(ui_->modeChooserComboBox,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            &generalSettings.eMode,
            [&generalSettings, this](int idx) {
                const auto data       = ui_->modeChooserComboBox->itemData(idx);
                generalSettings.eMode = data.value<OptimizationMode>();
            });

    // connectWrapper(*ui_->dryRunCheckBox, generalSettings.bDryRun);
    // connectWrapper(*ui_->inputDirTextEdit, generalSettings.sInputPath);
    // connectWrapper(*ui_->actionRedirect_output, generalSettings.bEnableOutputPath);

    connect(ui_->actionSet_output_path, &QAction::triggered, this, [&generalSettings, this] {
        const QString &inPath      = generalSettings.sInputPath.value_or(QDir::currentPath());
        const QString &currentPath = generalSettings.sOutputPath.value_or(inPath);

        const QString &dir = QFileDialog::getExistingDirectory(this,
                                                               tr("Open Directory"),
                                                               currentPath,
                                                               QFileDialog::ShowDirsOnly
                                                                   | QFileDialog::DontResolveSymlinks);

        if (dir.isEmpty())
            return;

        //Creating an empty file in order to mark the dir as "not empty"
        //This will prevent CAO from deleting the root output dir if there's no output
        QFile file(dir + "/__cao_output_dir");
        file.open(QFile::WriteOnly);

        generalSettings.sOutputPath = dir;
    });
     FIXME

}

void MainWindow::updateProfiles()
{
    auto *profiles              = ui_->Settings;
    const QString &previousText = profiles->currentText();

    {
        QSignalBlocker blocker(profiles);
        ProfilesManagerWindow(getProfiles()).updateProfiles(*profiles);
    }
    selectText(*profiles, previousText);
    reconnectThis();
}

void MainWindow::updatePatterns()
{
    auto *patterns              = ui_->patterns;
    const QString &previousText = patterns->currentText();

    {
        QSignalBlocker blocker(patterns);
        PatternsManagerWindow(currentProfile()).updatePatterns(*ui_->patterns);
    }
    selectText(*patterns, previousText);
    reconnectThis();
}*/

void MainWindow::init_process()
{
    auto settings = settings_;
    ui_to_settings(*ui_, module_display_, settings);

    // TODO: remove this
    settings                              = Settings::make_base();
    settings.current_profile().input_path = ui_->inputDirTextEdit->text().toStdString();
    settings.current_profile().base_per_file_settings.tex.compress = false;
    settings.current_profile().base_per_file_settings.tex.resize   = btu::tex::Dimension{
          .w = 512,
          .h = 512,
    };

    if (!check_settings(settings))
        return;

    if (!save_settings(settings))
    {
        QMessageBox::critical(this, tr("Error"), tr("Your settings could not be saved to disk. Aborting."));
        return;
    }

    try
    {
        cao_process_     = std::make_unique<Manager>(settings);
        progress_window_ = std::make_unique<ProgressWindow>(LogReader(Settings::data_directory()
                                                                      / k_log_file_name),
                                                            this);

        connect(cao_process_.get(),
                &Manager::files_counted,
                progress_window_.get(),
                &ProgressWindow::set_maximum);

        connect(cao_process_.get(),
                &Manager::file_processed,
                progress_window_.get(),
                [this](const std::filesystem::path &file) {
                    const auto text = QString("Processing %1").arg(QString::fromStdString(file.string()));
                    progress_window_->step(std::optional(text));
                });

        connect(cao_process_.get(), &Manager::end, progress_window_.get(), &ProgressWindow::end);
        connect(cao_process_.get(), &Manager::end, this, &MainWindow::end_process);

        connect(progress_window_.get(), &ProgressWindow::cancelled, this, &MainWindow::end_process);

        progress_window_->show();

        // we don't care about the future, we just want to run the process in another thread
        [[maybe_unused]] auto future = QtConcurrent::run(&Manager::run_optimization, cao_process_.get());
    }
    catch (const std::exception &e)
    {
        QMessageBox box(QMessageBox::Critical,
                        tr("Error"),
                        tr("An exception has been encountered and the process was forced to stop: %1")
                            .arg(e.what()));
        box.exec();
        end_process();
    }
}

void MainWindow::end_process()
{
    ui_->processButton->setDisabled(false);

    if (cao_process_)
        cao_process_->disconnect();

    if (progress_window_)
    {
        progress_window_->end();
        progress_window_->disconnect();
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
        R"(%1 %2
            Made by G'k
            This program is distributed in the hope that it will be useful but WITHOUT ANY WARRANTY.
            See the Mozilla Public License)";

    const QString text = QString("%1 %2 %3")
                             .arg(QCoreApplication::applicationName(),
                                  QCoreApplication::applicationVersion(),
                                  tr(message));

    QMessageBox::about(this, tr("About"), text);
}

[[maybe_unused]] void MainWindow::closeEvent(QCloseEvent *event)
{
    end_process();
    event->accept();
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
