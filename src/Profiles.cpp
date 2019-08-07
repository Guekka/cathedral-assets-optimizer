/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "Profiles.h"
#include "Manager.h"

Profiles Profiles::_instance = Profiles();

Profiles::Profiles()
{
    findProfiles(QDir("profiles"));
    loadProfile("Default");
};

size_t Profiles::findProfiles(const QDir &dir)
{
    _profileDir = dir;
    size_t counter = 0;
    for (const auto &subDir : _profileDir.entryList())
    {
        QString ini = dir.filePath(subDir + "/config.ini");
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
    if (!exists(newProfile))
        throw std::runtime_error("This profile does not exist: " + newProfile.toStdString());

    _currentProfile = newProfile;
    QSettings s("settings/common.ini", QSettings::IniFormat);
    s.setValue("profile", _currentProfile);

    _logPath = QDir::toNativeSeparators(QDir::currentPath() + "/logs/" + _currentProfile + ".html");
    QString iniPath = _profileDir.absoluteFilePath(_currentProfile + "/config.ini");
    _settings = new QSettings(iniPath, QSettings::IniFormat, this);
    _resourcePath = _profileDir.absoluteFilePath(_currentProfile);

    readFromIni();
}

bool Profiles::exists(const QString &profile)
{
    return getInstance()._profiles.contains(profile);
}

QStringList Profiles::list()
{
    return getInstance()._profiles;
}

void Profiles::saveToIni()
{
    _settings->setValue("bsaFormat", _bsaFormat);
    _settings->setValue("bsaTexturesFormat", _bsaTexturesFormat);
    _settings->setValue("maxBsaUncompressedSize", _maxBsaUncompressedSize);
    _settings->setValue("hasBsaTextures", _hasBsaTextures);
    _settings->setValue("maxBsaTexturesSize", _maxBsaTexturesSize);
    _settings->setValue("bsaExtension", _bsaExtension);
    _settings->setValue("bsaSuffix", _bsaSuffix);
    _settings->setValue("bsaTexturesSuffix", _bsaTexturesSuffix);
    _settings->setValue("meshesFileVersion", _meshesFileVersion);
    _settings->setValue("meshesStream", _meshesStream);
    _settings->setValue("meshesUser", _meshesUser);
    _settings->setValue("animationFormat", _animationFormat);
    _settings->setValue("texturesFormat", _texturesFormat);
    _settings->setValue("texturesConvertTga", _texturesConvertTga);
    _settings->setValue("texturesUnwantedFormats", _texturesUnwantedFormats);
    _settings->setValue("texturesCompressInterface", _texturesCompressInterface);
}

void Profiles::readFromIni()
{
    _bsaFormat = static_cast<bsa_archive_type_t>(_settings->value("bsaFormat").toInt());
    _bsaTexturesFormat = static_cast<bsa_archive_type_t>(_settings->value("bsaTexturesFormat").toInt());
    _maxBsaUncompressedSize = _settings->value("maxBsaUncompressedSize").toDouble();
    _hasBsaTextures = _settings->value("hasBsaTextures").toBool();
    _maxBsaTexturesSize = _settings->value("maxBsaTexturesSize").toDouble();
    _bsaExtension = _settings->value("bsaExtension").toString();
    _bsaSuffix = _settings->value("bsaSuffix").toString();
    _bsaTexturesSuffix = _settings->value("bsaTexturesSuffix").toString();
    _meshesFileVersion = static_cast<NiFileVersion>(_settings->value("meshesFileVersion").toInt());
    _meshesStream = _settings->value("meshesStream").toUInt();
    _meshesUser = _settings->value("meshesUser").toUInt();
    _animationFormat = static_cast<hkPackFormat>(_settings->value("animationFormat").toInt());
    _texturesFormat = _settings->value("texturesFormat").value<DXGI_FORMAT>();
    _texturesConvertTga = _settings->value("texturesConvertTga").toBool();
    _texturesUnwantedFormats = _settings->value("texturesUnwantedFormats").toList();
    _texturesCompressInterface = _settings->value("texturesCompressInterface").toBool();
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
    for (const QVariant &variant : _texturesUnwantedFormats)
    {
        DXGI_FORMAT format = variant.value<DXGI_FORMAT>();
        unwantedFormats << QString::fromStdString(dxgiFormatToString(format));
    }
    unwantedFormats.removeDuplicates();
    ui->texturesUnwantedFormats->setPlainText(unwantedFormats.join('\n'));
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
    for (const auto &line : ui->texturesUnwantedFormats->toPlainText().split('\n'))
    {
        const DXGI_FORMAT format = stringToDxgiFormat(line.toStdString());
        if (!_texturesUnwantedFormats.contains(format) && format != DXGI_FORMAT_UNKNOWN)
            _texturesUnwantedFormats += format;
    }
}
#endif

Profiles &Profiles::getInstance()
{
    return _instance;
}
