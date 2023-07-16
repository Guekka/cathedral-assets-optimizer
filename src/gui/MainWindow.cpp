/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "MainWindow.hpp"

#include "PatternsManagerWindow.hpp"
#include "ProfilesManagerWindow.hpp"
#include "SelectGPUWindow.hpp"
#include "gui/utils/set_theme.hpp"
#include "manager.hpp"
#include "settings/settings.hpp"
#include "ui_MainWindow.h"
#include "utils/utils.hpp"

#include <QDesktopServices>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QUrl>
#include <QtConcurrent/QtConcurrent>

namespace cao {

void ui_to_settings(const Ui::MainWindow &ui, Settings &settings)
{
    // TODO
}

void settings_to_ui(const Settings &settings, Ui::MainWindow &ui)
{
    // TODO
}

MainWindow::MainWindow() noexcept
    : ui_(std::make_unique<Ui::MainWindow>())
{
    ui_->setupUi(this);
    setAcceptDrops(true);

    // Init UI view
    ui_->tabWidget->setHidden(true);

    //Setting data for widgets
    setData(*ui_->modeChooserComboBox, tr("One mod"), SingleMod);
    setData(*ui_->modeChooserComboBox, tr("Several mods"), SeveralMods);

    //Connecting widgets that do not depend on current profile

    QObject::connect(ui_->manageProfiles, &QPushButton::pressed, this, [this] {
        ProfilesManagerWindow profiles_manager(settings_);
        profiles_manager.exec();
        settings_to_ui(settings_, *ui_);
    });

    QObject::connect(ui_->managePatterns, &QPushButton::pressed, this, [this] {
        PatternsManagerWindow patterns_manager(settings_);
        patterns_manager.exec();
        settings_to_ui(settings_, *ui_);
    });

    auto &common_settings = settings_.gui;

    ui_->actionEnableDarkTheme->setChecked(common_settings.gui_theme == GuiTheme::Dark);
    set_theme(common_settings.gui_theme);

    QObject::connect(ui_->actionEnableDarkTheme, &QAction::triggered, [this, &common_settings](bool state) {
        const GuiTheme theme      = state ? GuiTheme::Dark : GuiTheme::Light;
        common_settings.gui_theme = theme;
        ui_->actionEnableDarkTheme->setChecked(common_settings.gui_theme == GuiTheme::Dark);
        set_theme(theme);
    });

    // connectWrapper(*ui_->actionDelete_empty_directories, commonSettings.bDeleteEmptyFolders);

    QObject::connect(ui_->profiles, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int idx) {
        if (!settings_.set_current_profile(idx))
            return; // TODO: error message
        reconnectThis();
    });

    QObject::connect(ui_->processButton, &QPushButton::pressed, this, &MainWindow::initProcess);

    //Menu buttons
    QObject::connect(ui_->actionDocumentation, &QAction::triggered, this, [&] {
        QDesktopServices::openUrl(QUrl("https://www.nexusmods.com/skyrimspecialedition/mods/23316"));
    });

    QObject::connect(ui_->actionDiscord, &QAction::triggered, this, [&] {
        QDesktopServices::openUrl(QUrl("https://discordapp.com/invite/B9abN8d"));
    });

    QObject::connect(ui_->actionDonate, &QAction::triggered, this, [] {
        QDesktopServices::openUrl(QUrl("https://ko-fi.com/guekka"));
    });

    QObject::connect(ui_->actionAbout, &QAction::triggered, this, [this] {
        constexpr auto message =
            R"(Made by G'k
            This program is distributed in the hope that it will be useful but WITHOUT ANY WARRANTY.
            See the Mozilla Public License)";

        const QString text = QString("%1 %2 %3")
                                 .arg(QCoreApplication::applicationName(),
                                      QCoreApplication::applicationVersion(),
                                      tr(message));

        QMessageBox::about(this, tr("About"), text);
    });

    QObject::connect(ui_->actionAbout_Qt, &QAction::triggered, this, [this] { QMessageBox::aboutQt(this); });

    firstStart();
}

MainWindow::~MainWindow() = default;

void MainWindow::addModule(IWindowModule *module)
{
    auto *tabs = ui_->tabWidget;

    tabs->addTab(module, module->name());
    tabs->setCurrentIndex(0);

    connectModule(*module);

    tabs->setHidden(false);
}

void MainWindow::clearModules()
{
    auto &tabs = ui_->tabWidget;
    //Iterating backwards as tabs are contained in a stack
    for (int i = tabs->count() - 1; i > -1; --i)
    {
        tabs->widget(0)->deleteLater();
        tabs->removeTab(0);
    }
    tabs->setHidden(true);
}

void MainWindow::setPatternsEnabled(bool state)
{
    const std::array patterns_object = std::to_array<QWidget *>({
        ui_->patterns,
        ui_->managePatterns,
        ui_->patternsLabel,
    });

    if (!selectText(*ui_->patterns, "*"))
    {
        QMessageBox::critical(this,
                              tr("Pattern not found"),
                              tr("Couldn't find the pattern '*'. Please reinstall the application"));
    }

    for (auto *widget : patterns_object)
    {
        widget->setEnabled(state);
        widget->setVisible(state);
    }
}

void MainWindow::setLevelSelectorHandler(const std::function<void()> &callback)
{
    QObject::connect(ui_->actionChange_level, &QAction::triggered, this, callback);
}
/*
void MainWindow::initSettings()
{
    const QString targetProfile = getProfiles().commonSettings().profile;
    updateProfiles();
    updatePatterns();
    selectText(*ui_->Settings, targetProfile);
}
 */

std::vector<IWindowModule *> MainWindow::getModules()
{
    std::vector<IWindowModule *> modules;
    for (int i = 0; i < ui_->tabWidget->count(); i++)
        if (auto mod = dynamic_cast<IWindowModule *>(ui_->tabWidget->widget(i)); mod)
            modules.emplace_back(mod);

    return modules;
}

void MainWindow::connectModule(IWindowModule &mod)
{
    /*
    const auto currentPattern = currentProfile().getGeneralSettings().sCurrentPattern.value_or("*");
    auto &pattern             = currentProfile().getPatterns().getSettingsByName(currentPattern);
    mod.setup(pattern, currentProfile().getGeneralSettings());
     FIXME
    */
}

void MainWindow::reconnectModules()
{
    for (auto *mod : getModules())
    {
        mod->disconnectAll();
        connectModule(*mod);
    }
}

void MainWindow::freezeModules(bool state)
{
    for (auto *mod : getModules())
        mod->setDisabled(state);
}

void MainWindow::connectThis()
{
    /*
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
     */
}

void MainWindow::disconnectThis()
{
    ConnectionWrapper::disconnectAll();
}

void MainWindow::reconnectThis()
{
    disconnectThis();
    connectThis();
    reconnectModules();
}
/*
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

void MainWindow::loadUi()
{
    set_theme(settings_.gui.gui_theme);

    //    ui_->profiles->setCurrentIndex(ui_->profiles->findText(getProfiles().currentProfileName())); FIXME
}

void MainWindow::resetUi()
{
    ui_->tabWidget->clear();
}

void MainWindow::initProcess()
{
    if (!save_settings(settings_))
        ; // TODO: error

    ui_->processButton->setDisabled(true);

    try
    {
        caoProcess_     = std::make_unique<Manager>(settings_);
        progressWindow_ = std::make_unique<ProgressWindow>(""); // FIXME: log path

        connect(caoProcess_.get(),
                &Manager::file_processed,
                progressWindow_.get(),
                [this](const std::filesystem::path &file) {
                    // TODO
                });

        connect(caoProcess_.get(), &Manager::end, progressWindow_.get(), [this] {
            progressWindow_->update(tr("Done"), 1, 1);
        });

        connect(caoProcess_.get(), &Manager::end, this, &MainWindow::endProcess);
        connect(progressWindow_.get(), &ProgressWindow::closed, this, &MainWindow::endProcess);

        progressWindow_->show();

        freezeModules();

        QtConcurrent::run(caoProcess_.get(), &Manager::run_optimization);
    }
    catch (const std::exception &e)
    {
        QMessageBox box(QMessageBox::Critical,
                        tr("Error"),
                        tr("An exception has been encountered and the process was forced to stop: ")
                            + QString(e.what()));
        box.exec();
        endProcess();
    }
}

void MainWindow::endProcess()
{
    ui_->processButton->setDisabled(false);

    if (caoProcess_)
        caoProcess_->disconnect();

    if (progressWindow_)
    {
        progressWindow_->end();
        progressWindow_->disconnect();
    }

    freezeModules(false);
}

void MainWindow::firstStart()
{
    constexpr auto welcome
        = R"(It appears you are running CAO for the first time. All options have tooltips explaining what
          they do. If you need help, you can also <a href="https://discordapp.com/invite/B9abN8d">join us
          on Discord</a>. A dark theme is also available.

          If you like my work, <a href="https://ko-fi.com/guekka">please consider supporting me</a>.
          Thanks for using CAO!)";

    if (std::exchange(settings_.gui.first_run, true))
    {
        QMessageBox box(QMessageBox::Information,
                        tr("Welcome to %1 %2")
                            .arg(QCoreApplication::applicationName(), QCoreApplication::applicationVersion()),
                        tr(welcome));
        box.setTextFormat(Qt::TextFormat::RichText);
        box.exec();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    endProcess();
    event->accept();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls())
        e->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *e)
{
    const QString &fileName = e->mimeData()->urls().at(0).toLocalFile();
    QDir dir;
    if (dir.exists(fileName))
        ui_->inputDirTextEdit->setText(QDir::cleanPath(fileName));
}

} // namespace cao
