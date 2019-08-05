/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "Games.h"
#include "Manager.h"

Games::Games(){};

Games *Games::_INSTANCE = nullptr;

void Games::setGame(const GameMode &newGame)
{
    _game = newGame;
    QSettings s("settings/common/config.ini", QSettings::IniFormat);
    s.setValue("game", _game);

    _texturesUnwantedFormats.clear();

    switch (_game)
    {
    case Invalid: throw std::runtime_error("Invalid game"); break;

    case SSE:
        _bsaFormat = baSSE;
        _bsaTexturesFormat = baSSE;
        _maxBsaUncompressedSize = 2 * GigaByte;
        _hasBsaTextures = true;
        //Using 2.37GB for max textures size since this bsa will always be compressed.
        //After some experiments, it is the maximum size to ensure the bsa will always
        //stay under the maximum size : ~2.1gb
        _maxBsaTexturesSize = 2.37 * GigaByte;
        _bsaExtension = ".bsa";
        _bsaSuffix = ".bsa";
        _bsaTexturesSuffix = " - Textures.bsa";
        _meshesFileVersion = V20_2_0_7;
        _meshesStream = 100;
        _meshesUser = 12;
        _animationFormat = HKPF_AMD64;
        _texturesFormat = DXGI_FORMAT_BC7_UNORM;
        _texturesConvertTga = true;
        _texturesUnwantedFormats << DXGI_FORMAT_B5G5R5A1_UNORM << DXGI_FORMAT_B5G6R5_UNORM;
        _texturesCompressInterface = true;
        _iniPath = QDir::currentPath() + "/settings/SkyrimSE/config.ini";
        _logPath = QDir::toNativeSeparators(QDir::currentPath() + "/logs/SkyrimSE.html");
        _resourcePath = QDir::currentPath() + "/resources/SkyrimSE/";
        break;

    case TES5:
        _bsaFormat = baFO3;
        _bsaTexturesFormat = baNone;
        _maxBsaUncompressedSize = 2 * GigaByte;
        _hasBsaTextures = false;
        _maxBsaTexturesSize = 0;
        _bsaExtension = ".bsa";
        _bsaSuffix = ".bsa";
        _bsaTexturesSuffix = ".bsa";
        _meshesFileVersion = V20_2_0_7;
        _meshesStream = 83;
        _meshesUser = 12;
        _animationFormat = HKPF_WIN32;
        _texturesFormat = DXGI_FORMAT_BC3_UNORM;
        _texturesConvertTga = false;
        _texturesUnwantedFormats << DXGI_FORMAT_BC7_UNORM;
        _texturesCompressInterface = true;
        _iniPath = QDir::currentPath() + "/settings/TES5/config.ini";
        _logPath = QDir::toNativeSeparators(QDir::currentPath() + "/logs/TES5.html");
        _resourcePath = QDir::currentPath() + "/resources/TES5/";
        break;

    case FO4:
        _bsaFormat = baFO4;
        _bsaTexturesFormat = baFO4dds;
        _maxBsaUncompressedSize = 3.9 * GigaByte;
        _hasBsaTextures = false;
        //TODO enable bsa textures
        //DirectXTex will have to be used as a callback for libbsarch
        _maxBsaTexturesSize = 4.5 * GigaByte;
        _bsaExtension = ".ba2";
        _bsaSuffix = " - Main.ba2";
        _bsaTexturesSuffix = " - Textures.ba2";
        _meshesFileVersion = V20_2_0_7;
        _meshesStream = 130;
        _meshesUser = 12;
        _animationFormat = HKPF_AMD64;
        _texturesFormat = DXGI_FORMAT_BC7_UNORM;
        _texturesConvertTga = true;
        _texturesUnwantedFormats << DXGI_FORMAT_B5G5R5A1_UNORM << DXGI_FORMAT_B5G6R5_UNORM;
        _texturesCompressInterface = true;
        _iniPath = QDir::currentPath() + "/settings/FO4/config.ini";
        _logPath = QDir::toNativeSeparators(QDir::currentPath() + "/logs/FO4.txt");
        _resourcePath = QDir::currentPath() + "/resources/FO4/";
        break;

    case Custom:
        QSettings settings("settings/Custom/config.ini", QSettings::IniFormat);
        _resourcePath = QDir::currentPath() + "/resources/Custom/";
        _iniPath = QDir::currentPath() + "/settings/Custom/config.ini";
        _logPath = QDir::toNativeSeparators(QDir::currentPath() + "/logs/Custom.html");

        if (!QFile("settings/Custom/config.ini").exists())
            saveToIni(&settings);

        readFromIni(&settings);
        break;
    }
}

void Games::setGame(const QString &gameString)
{
    setGame(stringToGame(gameString));
}

Games::GameMode Games::stringToGame(const QString &string)
{
    if (string == "SSE")
        return SSE;
    else if (string == "TES5")
        return TES5;
    else if (string == "FO4")
        return FO4;
    else if (string == "Custom")
        return Custom;
    else
        return Invalid;
}

void Games::saveToIni(QSettings *settings)
{
    settings->setValue("bsaFormat", _bsaFormat);
    settings->setValue("bsaTexturesFormat", _bsaTexturesFormat);
    settings->setValue("maxBsaUncompressedSize", _maxBsaUncompressedSize);
    settings->setValue("hasBsaTextures", _hasBsaTextures);
    settings->setValue("maxBsaTexturesSize", _maxBsaTexturesSize);
    settings->setValue("bsaExtension", _bsaExtension);
    settings->setValue("bsaSuffix", _bsaSuffix);
    settings->setValue("bsaTexturesSuffix", _bsaTexturesSuffix);
    settings->setValue("meshesFileVersion", _meshesFileVersion);
    settings->setValue("meshesStream", _meshesStream);
    settings->setValue("meshesUser", _meshesUser);
    settings->setValue("animationFormat", _animationFormat);
    settings->setValue("texturesFormat", _texturesFormat);
    settings->setValue("texturesConvertTga", _texturesConvertTga);
    settings->setValue("texturesUnwantedFormats", _texturesUnwantedFormats);
    settings->setValue("texturesCompressInterface", _texturesCompressInterface);
}

void Games::readFromIni(QSettings *settings)
{
    _bsaFormat = static_cast<bsa_archive_type_t>(settings->value("bsaFormat").toInt());
    _bsaTexturesFormat = static_cast<bsa_archive_type_t>(settings->value("bsaTexturesFormat").toInt());
    _maxBsaUncompressedSize = settings->value("maxBsaUncompressedSize").toDouble();
    _hasBsaTextures = settings->value("hasBsaTextures").toBool();
    _maxBsaTexturesSize = settings->value("maxBsaTexturesSize").toDouble();
    _bsaExtension = settings->value("bsaExtension").toString();
    _bsaSuffix = settings->value("bsaSuffix").toString();
    _bsaTexturesSuffix = settings->value("bsaTexturesSuffix").toString();
    _meshesFileVersion = static_cast<NiFileVersion>(settings->value("meshesFileVersion").toInt());
    _meshesStream = settings->value("meshesStream").toUInt();
    _meshesUser = settings->value("meshesUser").toUInt();
    _animationFormat = static_cast<hkPackFormat>(settings->value("animationFormat").toInt());
    _texturesFormat = settings->value("texturesFormat").value<DXGI_FORMAT>();
    _texturesConvertTga = settings->value("texturesConvertTga").toBool();
    _texturesUnwantedFormats = settings->value("texturesUnwantedFormats").toList();
    _texturesCompressInterface = settings->value("texturesCompressInterface").toBool();
}
#ifdef GUI
void Games::setGame(Ui::MainWindow *ui)
{
    _game = uiToGame(ui);
    saveToUi(ui);

    if (ui->advancedSettingsCheckbox->isChecked())
        _game = Custom;
    else
        setGame(_game);
}

Games::GameMode Games::uiToGame(Ui::MainWindow *ui)
{
    if (ui->presets->currentData().canConvert<GameMode>())
        return ui->presets->currentData().value<GameMode>();
    else
        return Invalid;
}

void Games::saveToUi(Ui::MainWindow *ui)
{
  const auto iterateComboBox = [](QComboBox *box, QVariant data) {
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

void Games::readFromUi(Ui::MainWindow *ui)
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

bool Games::getTexturesCompressInterface() const
{
    return _texturesCompressInterface;
}

Games::GameMode Games::getGame() const
{
    return _game;
}

Games *Games::getInstance()
{
    if (!Games::_INSTANCE)
        Games::_INSTANCE = new Games();

    return Games::_INSTANCE;
}

bsa_archive_type_e Games::GetBsaFormat() const
{
    return _bsaFormat;
}

bsa_archive_type_t Games::getBsaTexturesFormat() const
{
    return _bsaTexturesFormat;
}

double Games::getBsaUncompressedMaxSize() const
{
    return _maxBsaUncompressedSize;
}

double Games::getBsaTexturesMaxSize() const
{
    return _maxBsaTexturesSize;
}

QString Games::getBsaExtension() const
{
    return _bsaExtension;
}

bool Games::getHasBsaTextures() const
{
    return _hasBsaTextures;
}

QString Games::getBsaSuffix() const
{
    return _bsaSuffix;
}

QString Games::getBsaTexturesSuffix() const
{
    return _bsaTexturesSuffix;
}

uint Games::getMeshesUser() const
{
    return _meshesUser;
}

uint Games::getMeshesStream() const
{
    return _meshesStream;
}

NiFileVersion Games::getMeshesFileVersion() const
{
    return _meshesFileVersion;
}

DXGI_FORMAT Games::getTexturesFormat() const
{
    return _texturesFormat;
}

hkPackFormat Games::getAnimationsFormat() const
{
    return _animationFormat;
}

bool Games::getTexturesConvertTga() const
{
    return _texturesConvertTga;
}

QList<DXGI_FORMAT> Games::getTexturesUnwantedFormats() const
{
    QList<DXGI_FORMAT> list;
    for (auto &format : _texturesUnwantedFormats)
        list << QVariant::fromValue(format).value<DXGI_FORMAT>();
    return list;
}

QString Games::getIniPath() const
{
    return _iniPath;
}

QString Games::getLogPath() const
{
    return _logPath;
}

QString Games::getResourcePath() const
{
    return _resourcePath;
}
