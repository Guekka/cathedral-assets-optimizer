/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "Profiles.h"
#include "Manager.h"

const QString defaultProfile = "SSE";

Profiles *Profiles::_instance;

Profiles::Profiles(){};

void Profiles::init()
{
    _commonSettings = new QSettings("profiles/common.ini", QSettings::IniFormat, this);
    findProfiles(QDir("profiles"));
    const QString &mode = _commonSettings->value("profile").toString();
    loadProfile(mode);
}

size_t Profiles::findProfiles(const QDir &dir)
{
    _profileDir = dir;
    size_t counter = 0;
    _profiles.clear();
    for (const auto &subDir : _profileDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        const QString &ini = dir.absoluteFilePath(subDir + "/profile.ini");
        if (QFile::exists(ini))
        {
            _profiles << subDir;
            ++counter;
        }
    }
    PLOG_VERBOSE << "Profiles found: " << _profiles.join('\n');
    return counter;
}

void Profiles::loadProfile(const QString &newProfile)
{
    _currentProfile = exists(newProfile) ? newProfile : defaultProfile;
    _commonSettings->setValue("profile", _currentProfile);

    _logPath = QDir::toNativeSeparators(
        QString("%1/logs/%2/%3.html")
            .arg(QDir::currentPath(), _currentProfile, QDateTime::currentDateTime().toString("yy.MM.dd.hh.mm")));
    const QString &folder = _profileDir.absoluteFilePath(_currentProfile);
    _profileSettings = new QSettings(folder + "/profile.ini", QSettings::IniFormat, this);
    _optionsSettings = new QSettings(folder + "/settings.ini", QSettings::IniFormat, this);

    _isBaseProfile = QFile::exists(folder + "/isBase");

    readFromIni();
}

bool Profiles::exists(const QString &profile)
{
    getInstance().findProfiles(QDir("profiles"));
    return getInstance()._profiles.contains(profile) && !profile.isEmpty();
}

QStringList Profiles::list()
{
    return getInstance()._profiles;
}

void Profiles::create(const QString &name, const QString &baseProfile)
{
    const QString &baseFolder = getInstance()._profileDir.absoluteFilePath(exists(baseProfile) ? baseProfile
                                                                                               : defaultProfile);
    const QString &newFolder = getInstance()._profileDir.absoluteFilePath(name);
    FilesystemOperations::copyDir(baseFolder, newFolder, false);
    QFile::remove(newFolder + "/isBase");
    getInstance().findProfiles(getInstance()._profileDir);
}

QFile Profiles::getFile(const QString &filename)
{
    const QDir &currentProfileDir(getInstance()._profileDir.absoluteFilePath(getInstance().currentProfile()));
    const QDir &defaultProfileDir(getInstance()._profileDir.absoluteFilePath(defaultProfile));

    if (currentProfileDir.exists(filename))
        return QFile(currentProfileDir.filePath(filename));
    else if (defaultProfileDir.exists(filename))
        return QFile(defaultProfileDir.filePath(filename));
    else
        return QFile();
}

void Profiles::saveToIni()
{
    _profileSettings->beginGroup("BSA");
    _profileSettings->setValue("bsaEnabled", _bsaEnabled);
    _profileSettings->setValue("bsaFormat", _bsaFormat);
    _profileSettings->setValue("bsaTexturesFormat", _bsaTexturesFormat);
    _profileSettings->setValue("maxBsaUncompressedSize", _maxBsaUncompressedSize);
    _profileSettings->setValue("hasBsaTextures", _hasBsaTextures);
    _profileSettings->setValue("maxBsaTexturesSize", _maxBsaTexturesSize);
    _profileSettings->setValue("bsaExtension", _bsaExtension);
    _profileSettings->setValue("bsaSuffix", _bsaSuffix);
    _profileSettings->setValue("bsaTexturesSuffix", _bsaTexturesSuffix);
    _profileSettings->endGroup();
    _profileSettings->beginGroup("Meshes");
    _profileSettings->setValue("meshesEnabled", _meshesEnabled);
    _profileSettings->setValue("meshesFileVersion", _meshesFileVersion);
    _profileSettings->setValue("meshesStream", _meshesStream);
    _profileSettings->setValue("meshesUser", _meshesUser);
    _profileSettings->endGroup();
    _profileSettings->beginGroup("Animations");
    _profileSettings->setValue("animationsEnabled", _animationsEnabled);
    _profileSettings->setValue("animationFormat", _animationFormat);
    _profileSettings->endGroup();
    _profileSettings->beginGroup("Textures");
    _profileSettings->setValue("texturesEnabled", _texturesEnabled);
    _profileSettings->setValue("texturesFormat", _texturesFormat);
    _profileSettings->setValue("texturesConvertTga", _texturesConvertTga);
    _profileSettings->setValue("texturesUnwantedFormats", _texturesUnwantedFormats);
    _profileSettings->setValue("texturesCompressInterface", _texturesCompressInterface);
    _profileSettings->endGroup();
}

void Profiles::readFromIni()
{
    _profileSettings->beginGroup("BSA");
    _bsaEnabled = _profileSettings->value("bsaEnabled").toBool();
    _bsaFormat = static_cast<bsa_archive_type_t>(_profileSettings->value("bsaFormat").toInt());
    _bsaTexturesFormat = static_cast<bsa_archive_type_t>(_profileSettings->value("bsaTexturesFormat").toInt());
    _maxBsaUncompressedSize = _profileSettings->value("maxBsaUncompressedSize").toDouble();
    _hasBsaTextures = _profileSettings->value("hasBsaTextures").toBool();
    _maxBsaTexturesSize = _profileSettings->value("maxBsaTexturesSize").toDouble();
    _bsaExtension = _profileSettings->value("bsaExtension").toString();
    _bsaSuffix = _profileSettings->value("bsaSuffix").toString();
    _bsaTexturesSuffix = _profileSettings->value("bsaTexturesSuffix").toString();
    _profileSettings->endGroup();
    _profileSettings->beginGroup("Meshes");
    _meshesEnabled = _profileSettings->value("meshesEnabled").toBool();
    _meshesFileVersion = static_cast<NiFileVersion>(_profileSettings->value("meshesFileVersion").toInt());
    _meshesStream = _profileSettings->value("meshesStream").toUInt();
    _meshesUser = _profileSettings->value("meshesUser").toUInt();
    _profileSettings->endGroup();
    _profileSettings->beginGroup("Animations");
    _animationsEnabled = _profileSettings->value("animationsEnabled").toBool();
    _animationFormat = static_cast<hkPackFormat>(_profileSettings->value("animationFormat").toInt());
    _profileSettings->endGroup();
    _profileSettings->beginGroup("Textures");
    _texturesEnabled = _profileSettings->value("texturesEnabled").toBool();
    _texturesFormat = _profileSettings->value("texturesFormat").value<DXGI_FORMAT>();
    _texturesConvertTga = _profileSettings->value("texturesConvertTga").toBool();
    _texturesUnwantedFormats = _profileSettings->value("texturesUnwantedFormats").toList();
    _texturesCompressInterface = _profileSettings->value("texturesCompressInterface").toBool();
    _profileSettings->endGroup();
}
#ifdef GUI
void Profiles::loadProfile(Ui::MainWindow *ui)
{
    _currentProfile = uiToGame(ui);
    saveToUi(ui);
    loadProfile(_currentProfile);
}

QString Profiles::uiToGame(Ui::MainWindow *ui)
{
    return ui->presets->currentText();
}

void Profiles::saveToUi(Ui::MainWindow *ui)
{
    const auto iterateComboBox = [](QComboBox *box, const QVariant data) {
        for (int i = 0; i < box->count(); ++i)
        {
            if (box->itemData(i) == data)
            {
                box->setCurrentIndex(i);
                break;
            }
        }
    };

    iterateComboBox(ui->bsaFormat, _bsaFormat);
    iterateComboBox(ui->bsaTexturesFormat, _bsaTexturesFormat);
    ui->bsaMaximumSize->setValue(_maxBsaUncompressedSize / GigaByte);
    ui->bsaTexturesAdvancedGroupBox->setChecked(_hasBsaTextures);
    ui->bsaTexturesMaximumSize->setValue(_maxBsaTexturesSize / GigaByte);
    ui->bsaExtension->setText(_bsaExtension);
    ui->bsaSuffix->setText(_bsaSuffix);
    ui->bsaTexturesSuffix->setText(_bsaTexturesSuffix);

    iterateComboBox(ui->meshesUser, _meshesUser);
    iterateComboBox(ui->meshesStream, _meshesStream);
    iterateComboBox(ui->meshesVersion, _meshesFileVersion);

    //Animation format is not working when converting from amd64, thus not added to UI

    iterateComboBox(ui->texturesOutputFormat, _texturesFormat);
    ui->texturesTgaConversionCheckBox->setChecked(_texturesConvertTga);
    ui->texturesCompressInterfaceCheckBox->setChecked(_texturesCompressInterface);

    QStringList unwantedFormats;
    ui->texturesUnwantedFormatsList->clear();
    for (const QVariant &variant : _texturesUnwantedFormats)
    {
        const DXGI_FORMAT &format = variant.value<DXGI_FORMAT>();
        ui->texturesUnwantedFormatsList->addItem(dxgiFormatToString(format));
    }
}

void Profiles::readFromUi(Ui::MainWindow *ui)
{
    _bsaFormat = ui->bsaFormat->currentData().value<bsa_archive_type_e>();
    _bsaTexturesFormat = ui->bsaTexturesFormat->currentData().value<bsa_archive_type_e>();

    _maxBsaUncompressedSize = ui->bsaMaximumSize->value() * GigaByte;
    _hasBsaTextures = ui->bsaTexturesAdvancedGroupBox->isChecked();
    _maxBsaTexturesSize = ui->bsaTexturesMaximumSize->value() * GigaByte;
    _bsaExtension = ui->bsaExtension->text();
    _bsaSuffix = ui->bsaSuffix->text();
    _bsaTexturesSuffix = ui->bsaTexturesSuffix->text();

    _meshesUser = ui->meshesUser->currentData().toUInt();
    _meshesStream = ui->meshesStream->currentData().toUInt();
    _meshesFileVersion = ui->meshesVersion->currentData().value<NiFileVersion>();
    //Animation format is not working currently, thus not added to UI

    _texturesFormat = ui->texturesOutputFormat->currentData().value<DXGI_FORMAT>();
    _texturesConvertTga = ui->texturesTgaConversionCheckBox->isChecked();
    _texturesCompressInterface = ui->texturesCompressInterfaceCheckBox->isChecked();

    _texturesUnwantedFormats.clear();
    for (int i = 0; i < ui->texturesUnwantedFormatsList->count(); ++i)
    {
        const auto &entry = ui->texturesUnwantedFormatsList->item(i);
        const DXGI_FORMAT &format = stringToDxgiFormat(entry->text());
        if (!_texturesUnwantedFormats.contains(format) && format != DXGI_FORMAT_UNKNOWN)
            _texturesUnwantedFormats += format;
    }
}
#endif

Profiles &Profiles::getInstance()
{
    if (!_instance)
    {
        _instance = new Profiles();
        _instance->init();
    }

    return *_instance;
}
