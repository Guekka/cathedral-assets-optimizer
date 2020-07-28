/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "MainWindow.hpp"
#include "Manager.hpp"
#include "SelectGPUWindow.hpp"
#include "Utils.hpp"

namespace CAO {
MainWindow::MainWindow()
    : ui_(new ::Ui::MainWindow)
{
    ui_->setupUi(this);
    setAcceptDrops(true);

    //Setting data for widgets
    setData(*ui_->modeChooserComboBox, "One mod", SingleMod);
    setData(*ui_->modeChooserComboBox, "Several mods", SeveralMods);

    //Connecting widgets that do not depend on current profile
    connect(ui_->newProfilePushButton, &QPushButton::pressed, this, &MainWindow::createProfile);
    connect(ui_->processButton, &QPushButton::pressed, this, &MainWindow::initProcess);

    //Menu buttons
    connect(ui_->actionOpen_log_file, &QAction::triggered, this, [] {
        QDesktopServices::openUrl(QUrl("file:///" + currentProfile().logPath(), QUrl::TolerantMode));
    });

    connect(ui_->actionDocumentation, &QAction::triggered, this, [&] {
        QDesktopServices::openUrl(QUrl("https://www.nexusmods.com/skyrimspecialedition/mods/23316"));
    });

    connect(ui_->actionDiscord, &QAction::triggered, this, [&] {
        QDesktopServices::openUrl(QUrl("https://discordapp.com/invite/B9abN8d"));
    });

    connect(ui_->actionDonate, &QAction::triggered, this, [&] {
        QDesktopServices::openUrl(QUrl("https://ko-fi.com/guekka"));
    });

    connect(ui_->actionAbout, &QAction::triggered, this, [&] {
        constexpr char message[]
            = "\nMade by G'k\nThis program is distributed in the hope that it will be useful "
              "but WITHOUT ANY WARRANTLY. See the Mozilla Public License";

        const QString text = QString("%1 %2 %3")
                                 .arg(QCoreApplication::applicationName(),
                                      QCoreApplication::applicationVersion(),
                                      tr(message));

        QMessageBox::about(this, tr("About"), text);
    });

    connect(ui_->actionAbout_Qt, &QAction::triggered, this, [&] { QMessageBox::aboutQt(this); });

    //Profiles
    setProfile(getProfiles().currentProfileName());

    firstStart();
}

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
    mod.disconnectAll();
    mod.connectAll(currentProfile().getPatterns().getDefaultSettings(), currentProfile().getGeneralSettings());
    //FIXME Change patterns to the current selected pattern
}

void MainWindow::reconnectModules()
{
    for (auto *mod : getModules())
        connectModule(*mod);
}

void MainWindow::freezeModules(bool state)
{
    for (auto *mod : getModules())
        mod->setDisabled(state);
}

void MainWindow::connectAll()
{
    auto &commonSettings  = getProfiles().commonSettings();
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

        if (!dir.isEmpty())
            generalSettings.sInputPath = dir;
    });

    selectText(*ui_->profiles, commonSettings.sProfile());
    connect(ui_->profiles,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            &commonSettings.sProfile,
            [this, &commonSettings](int idx) { commonSettings.sProfile = ui_->profiles->itemText(idx); });

    connect(&commonSettings.sProfile,
            &detail::QValueWrapperHelper::valueChanged,
            this,
            [this, &commonSettings] {
                auto profile = commonSettings.sProfile();
                profile      = getProfiles().exists(profile) ? profile : Profiles::defaultProfile;

                selectText(*ui_->profiles, profile);
                getProfiles().setCurrent(profile);
            });

    const int currentModeIndex = ui_->modeChooserComboBox->findData(generalSettings.eMode());
    ui_->modeChooserComboBox->setCurrentIndex(currentModeIndex);

    connect(ui_->modeChooserComboBox,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            &generalSettings.eMode,
            [&generalSettings, this](int idx) {
                auto data             = ui_->modeChooserComboBox->itemData(idx);
                generalSettings.eMode = data.value<OptimizationMode>();
            });

    connect(&generalSettings.eMode, &detail::QValueWrapperHelper::valueChanged, this, [this, &generalSettings] {
        int idx = ui_->modeChooserComboBox->findData(generalSettings.eMode());
        ui_->modeChooserComboBox->setCurrentIndex(idx);
    });

    ui_->actionEnableDarkTheme->setChecked(commonSettings.bDarkMode.value_or(true));
    setDarkTheme(commonSettings.bDarkMode.value_or(true));

    connect(ui_->actionEnableDarkTheme,
            &QAction::triggered,
            &commonSettings.bDarkMode,
            [this, &commonSettings](bool state) {
                commonSettings.bDarkMode = state;
                setDarkTheme(state);
            });

    connect(&commonSettings.bDarkMode,
            &decltype(commonSettings.bDarkMode)::valueChanged,
            this,
            [this, &commonSettings] {
                ui_->actionEnableDarkTheme->setChecked(commonSettings.bDarkMode());
                setDarkTheme(commonSettings.bDarkMode());
            });

    connectWrapper(*ui_->dryRunCheckBox, generalSettings.bDryRun);
    connectWrapper(*ui_->inputDirTextEdit, generalSettings.sInputPath);
    connectWrapper(*ui_->actionRedirect_output, generalSettings.bEnableOutputPath);

    connect(ui_->actionSet_output_path, &QAction::triggered, this, [&generalSettings, this] {
        const QString &inPath      = generalSettings.sInputPath.value_or(QDir::currentPath());
        const QString &currentPath = generalSettings.sOutputPath.value_or(inPath);

        const QString &dir = QFileDialog::getExistingDirectory(this,
                                                               tr("Open Directory"),
                                                               currentPath,
                                                               QFileDialog::ShowDirsOnly
                                                                   | QFileDialog::DontResolveSymlinks);

        if (!dir.isEmpty())
            generalSettings.sOutputPath = dir;
    });

    connectWrapper(*ui_->actionShow_tutorials, commonSettings.bShowTutorials);
    connectWrapper(*ui_->actionEnable_debug_log, commonSettings.bDebugLog);
}

void MainWindow::loadUi()
{
    auto &commonSettings = getProfiles().commonSettings();

    setDarkTheme(commonSettings.bDarkMode());

    ui_->profiles->setCurrentIndex(ui_->profiles->findText(getProfiles().currentProfileName()));
    ui_->actionShow_tutorials->setChecked(commonSettings.bShowTutorials());
}

void MainWindow::resetUi()
{
    ui_->tabWidget->clear();
}


void MainWindow::refreshProfiles()
{
    ui_->profiles->clear();
    ui_->profiles->addItems(getProfiles().list());
}

void MainWindow::createProfile()
{
    bool ok = false;
    const QString &text
        = QInputDialog::getText(this, tr("New profile"), tr("Name:"), QLineEdit::Normal, "", &ok);
    if (!ok || text.isEmpty())
        return;

    //Choosing base profile

    QStringList profilesList;
    for (int i = 0; i < ui_->profiles->count(); ++i)
        profilesList << ui_->profiles->itemText(i);

    const QString &baseProfile = QInputDialog::getItem(this,
                                                       tr("Base profile"),
                                                       tr("Which profile do you want to use as a base?"),
                                                       profilesList,
                                                       ui_->profiles->currentIndex(),
                                                       false,
                                                       &ok);

    if (!ok)
        return;

    getProfiles().create(text, baseProfile);
    setProfile(text);
}

void MainWindow::setProfile(const QString &name)
{
    refreshProfiles();

    ui_->profiles->setCurrentIndex(ui_->profiles->findText(name));

    connectAll();
    reconnectModules();
}

void MainWindow::setDarkTheme(const bool &enabled)
{
    if (enabled)
    {
        QFile f(":qdarkstyle/style.qss");
        f.open(QFile::ReadOnly | QFile::Text);
        qApp->setStyleSheet(f.readAll());
        f.close();
    }
    else
        qApp->setStyleSheet("");
}

void MainWindow::initProcess()
{
    getProfiles().saveCommonSettings();
    currentProfile().saveToJSON();
    ui_->processButton->setDisabled(true);

    try
    {
        caoProcess_     = std::make_unique<Manager>();
        progressWindow_ = std::make_unique<ProgressWindow>(currentProfile().logPath());

        connect(caoProcess_.get(),
                &Manager::progressBarTextChanged,
                progressWindow_.get(),
                &ProgressWindow::update);

        connect(caoProcess_.get(), &Manager::end, this, &MainWindow::endProcess);

        progressWindow_->show();

        freezeModules();

        QtConcurrent::run(caoProcess_.get(), &Manager::runOptimization);
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

    getProfiles().saveCommonSettings();
    currentProfile().saveToJSON();

    freezeModules(false);
}

void MainWindow::showTutorialWindow(const QString &title, const QString &text)
{
    if (getProfiles().commonSettings().bShowTutorials())
        QMessageBox::information(this, title, text);
}

void MainWindow::firstStart()
{
    constexpr char welcome[]
        = "It appears you are running CAO for the first time. All options have tooltips explaining what "
          "they do. If you need help, you can also <a href=\"https://discordapp.com/invite/B9abN8d\">join us "
          "on Discord</a>. A dark theme is also available."
          "\n\nIf you like my work, <a href=\"https://ko-fi.com/guekka\">please consider supporting me</a>. "
          "Thanks for using CAO!";

    if (getProfiles().commonSettings().bFirstStart.value_or(true))
    {
        QMessageBox box(QMessageBox::Information,
                        tr("Welcome to %1 %2")
                            .arg(QCoreApplication::applicationName(), QCoreApplication::applicationVersion()),
                        tr(welcome));
        box.setTextFormat(Qt::TextFormat::RichText);
        box.exec();

        getProfiles().commonSettings().bFirstStart = false;
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
} // namespace CAO
