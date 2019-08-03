/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "Games.h"
#include "Manager.h"

Games::Games(){};

Games *Games::INSTANCE = nullptr;

void Games::setGame(const GameMode &newGame)
{
    game = newGame;
    QSettings s("settings/common/config.ini", QSettings::IniFormat);
    s.setValue("game", game);

    texturesUnwantedFormats.clear();

    switch (game)
    {
    case Invalid: throw std::runtime_error("Invalid game"); break;

    case SSE:
        bsaFormat = baSSE;
        bsaTexturesFormat = baSSE;
        maxBsaUncompressedSize = 2 * GigaByte;
        hasBsaTextures = true;
        //Using 2.37GB for max textures size since this bsa will always be compressed.
        //After some experiments, it is the maximum size to ensure the bsa will always
        //stay under the maximum size : ~2.1gb
        maxBsaTexturesSize = 2.37 * GigaByte;
        bsaExtension = ".bsa";
        bsaSuffix = ".bsa";
        bsaTexturesSuffix = " - Textures.bsa";
        meshesFileVersion = V20_2_0_7;
        meshesStream = 100;
        meshesUser = 12;
        animationFormat = HKPF_AMD64;
        texturesFormat = DXGI_FORMAT_BC7_UNORM;
        texturesConvertTga = true;
        texturesUnwantedFormats << DXGI_FORMAT_B5G5R5A1_UNORM << DXGI_FORMAT_B5G6R5_UNORM;
        texturesCompressInterface = true;
        iniPath = QDir::currentPath() + "/settings/SkyrimSE/config.ini";
        logPath = QDir::toNativeSeparators(QDir::currentPath() + "/logs/SkyrimSE.html");
        resourcePath = QDir::currentPath() + "/resources/SkyrimSE/";
        break;

    case TES5:
        bsaFormat = baFO3;
        bsaTexturesFormat = baNone;
        maxBsaUncompressedSize = 2 * GigaByte;
        hasBsaTextures = false;
        maxBsaTexturesSize = 0;
        bsaExtension = ".bsa";
        bsaSuffix = ".bsa";
        bsaTexturesSuffix = ".bsa";
        meshesFileVersion = V20_2_0_7;
        meshesStream = 83;
        meshesUser = 12;
        animationFormat = HKPF_WIN32;
        texturesFormat = DXGI_FORMAT_BC3_UNORM;
        texturesConvertTga = false;
        texturesUnwantedFormats << DXGI_FORMAT_BC7_UNORM;
        texturesCompressInterface = true;
        iniPath = QDir::currentPath() + "/settings/TES5/config.ini";
        logPath = QDir::toNativeSeparators(QDir::currentPath() + "/logs/TES5.html");
        resourcePath = QDir::currentPath() + "/resources/TES5/";
        break;

    case FO4:
        bsaFormat = baFO4;
        bsaTexturesFormat = baFO4dds;
        maxBsaUncompressedSize = 3.9 * GigaByte;
        hasBsaTextures = false;
        //TODO enable bsa textures
        //DirectXTex will have to be used as a callback for libbsarch
        maxBsaTexturesSize = 4.5 * GigaByte;
        bsaExtension = ".ba2";
        bsaSuffix = " - Main.ba2";
        bsaTexturesSuffix = " - Textures.ba2";
        meshesFileVersion = V20_2_0_7;
        meshesStream = 130;
        meshesUser = 12;
        animationFormat = HKPF_AMD64;
        texturesFormat = DXGI_FORMAT_BC7_UNORM;
        texturesConvertTga = true;
        texturesUnwantedFormats << DXGI_FORMAT_B5G5R5A1_UNORM << DXGI_FORMAT_B5G6R5_UNORM;
        texturesCompressInterface = true;
        iniPath = QDir::currentPath() + "/settings/FO4/config.ini";
        logPath = QDir::toNativeSeparators(QDir::currentPath() + "/logs/FO4.txt");
        resourcePath = QDir::currentPath() + "/resources/FO4/";
        break;

    case Custom:
        QSettings settings("settings/Custom/config.ini", QSettings::IniFormat);
        resourcePath = QDir::currentPath() + "/resources/Custom/";
        iniPath = QDir::currentPath() + "/settings/Custom/config.ini";
        logPath = QDir::toNativeSeparators(QDir::currentPath() + "/logs/Custom.html");

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
    settings->setValue("bsaFormat", bsaFormat);
    settings->setValue("bsaTexturesFormat", bsaTexturesFormat);
    settings->setValue("maxBsaUncompressedSize", maxBsaUncompressedSize);
    settings->setValue("hasBsaTextures", hasBsaTextures);
    settings->setValue("maxBsaTexturesSize", maxBsaTexturesSize);
    settings->setValue("bsaExtension", bsaExtension);
    settings->setValue("bsaSuffix", bsaSuffix);
    settings->setValue("bsaTexturesSuffix", bsaTexturesSuffix);
    settings->setValue("meshesFileVersion", meshesFileVersion);
    settings->setValue("meshesStream", meshesStream);
    settings->setValue("meshesUser", meshesUser);
    settings->setValue("animationFormat", animationFormat);
    settings->setValue("texturesFormat", texturesFormat);
    settings->setValue("texturesConvertTga", texturesConvertTga);
    settings->setValue("texturesUnwantedFormats", texturesUnwantedFormats);
    settings->setValue("texturesCompressInterface", texturesCompressInterface);
}

void Games::readFromIni(QSettings *settings)
{
    bsaFormat = static_cast<bsa_archive_type_t>(settings->value("bsaFormat").toInt());
    bsaTexturesFormat = static_cast<bsa_archive_type_t>(settings->value("bsaTexturesFormat").toInt());
    maxBsaUncompressedSize = settings->value("maxBsaUncompressedSize").toDouble();
    hasBsaTextures = settings->value("hasBsaTextures").toBool();
    maxBsaTexturesSize = settings->value("maxBsaTexturesSize").toDouble();
    bsaExtension = settings->value("bsaExtension").toString();
    bsaSuffix = settings->value("bsaSuffix").toString();
    bsaTexturesSuffix = settings->value("bsaTexturesSuffix").toString();
    meshesFileVersion = static_cast<NiFileVersion>(settings->value("meshesFileVersion").toInt());
    meshesStream = settings->value("meshesStream").toUInt();
    meshesUser = settings->value("meshesUser").toUInt();
    animationFormat = static_cast<hkPackFormat>(settings->value("animationFormat").toInt());
    texturesFormat = settings->value("texturesFormat").value<DXGI_FORMAT>();
    texturesConvertTga = settings->value("texturesConvertTga").toBool();
    texturesUnwantedFormats = settings->value("texturesUnwantedFormats").toList();
    texturesCompressInterface = settings->value("texturesCompressInterface").toBool();
}
#ifdef GUI
void Games::setGame(Ui::MainWindow *ui)
{
    game = uiToGame(ui);
    saveToUi(ui);

    if (ui->advancedSettingsCheckbox->isChecked())
        game = Custom;
    else
        setGame(game);
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
    auto iterateComboBox = [](QComboBox *box, QVariant data) {
        for (int i = 0; i < box->count(); ++i)
        {
            if (box->itemData(i) == data)
            {
                box->setCurrentIndex(i);
                break;
            }
        }
    };

    iterateComboBox(ui->bsaFormat, bsaFormat);
    iterateComboBox(ui->bsaTexturesFormat, bsaTexturesFormat);
    ui->bsaMaximumSize->setValue(maxBsaUncompressedSize / GigaByte);
    ui->bsaTexturesAdvancedGroupBox->setChecked(hasBsaTextures);
    ui->bsaTexturesMaximumSize->setValue(maxBsaTexturesSize / GigaByte);
    ui->bsaExtension->setText(bsaExtension);
    ui->bsaSuffix->setText(bsaSuffix);
    ui->bsaTexturesSuffix->setText(bsaTexturesSuffix);

    iterateComboBox(ui->meshesUser, meshesUser);
    iterateComboBox(ui->meshesStream, meshesStream);
    iterateComboBox(ui->meshesVersion, meshesFileVersion);

    //Animation format is not working when converting from amd64, thus not added to UI

    iterateComboBox(ui->texturesOutputFormat, texturesFormat);
    ui->texturesTgaConversionCheckBox->setChecked(texturesConvertTga);
    ui->texturesCompressInterfaceCheckBox->setChecked(texturesCompressInterface);

    QStringList unwantedFormats;
    for (const QVariant &variant : texturesUnwantedFormats)
    {
        DXGI_FORMAT format = variant.value<DXGI_FORMAT>();
        unwantedFormats << QString::fromStdString(dxgiFormatToString(format));
    }
    unwantedFormats.removeDuplicates();
    ui->texturesUnwantedFormats->setPlainText(unwantedFormats.join('\n'));
}

void Games::readFromUi(Ui::MainWindow *ui)
{
    bsaFormat = ui->bsaFormat->currentData().value<bsa_archive_type_e>();
    bsaTexturesFormat = ui->bsaTexturesFormat->currentData().value<bsa_archive_type_e>();

    maxBsaUncompressedSize = ui->bsaMaximumSize->value() * GigaByte;
    hasBsaTextures = ui->bsaTexturesAdvancedGroupBox->isChecked();
    maxBsaTexturesSize = ui->bsaTexturesMaximumSize->value() * GigaByte;
    bsaExtension = ui->bsaExtension->text();
    bsaSuffix = ui->bsaSuffix->text();
    bsaTexturesSuffix = ui->bsaTexturesSuffix->text();

    meshesUser = ui->meshesUser->currentData().toUInt();
    meshesStream = ui->meshesStream->currentData().toUInt();
    meshesFileVersion = ui->meshesVersion->currentData().value<NiFileVersion>();
    //Animation format is not working currently, thus not added to UI

    texturesFormat = ui->texturesOutputFormat->currentData().value<DXGI_FORMAT>();
    texturesConvertTga = ui->texturesTgaConversionCheckBox->isChecked();
    texturesCompressInterface = ui->texturesCompressInterfaceCheckBox->isChecked();

    for (const auto &line : ui->texturesUnwantedFormats->toPlainText().split('\n'))
    {
        DXGI_FORMAT format = stringToDxgiFormat(line.toStdString());
        if (!texturesUnwantedFormats.contains(format) && format != DXGI_FORMAT_UNKNOWN)
            texturesUnwantedFormats += format;
    }
}
#endif

bool Games::getTexturesCompressInterface() const
{
    return texturesCompressInterface;
}

Games::GameMode Games::getGame() const
{
    return game;
}

Games *Games::getInstance()
{
    if (!Games::INSTANCE)
        Games::INSTANCE = new Games();

    return Games::INSTANCE;
}

bsa_archive_type_e Games::GetBsaFormat() const
{
    return bsaFormat;
}

bsa_archive_type_t Games::getBsaTexturesFormat() const
{
    return bsaTexturesFormat;
}

double Games::getBsaUncompressedMaxSize() const
{
    return maxBsaUncompressedSize;
}

double Games::getBsaTexturesMaxSize() const
{
    return maxBsaTexturesSize;
}

QString Games::getBsaExtension() const
{
    return bsaExtension;
}

bool Games::getHasBsaTextures() const
{
    return hasBsaTextures;
}

QString Games::getBsaSuffix() const
{
    return bsaSuffix;
}

QString Games::getBsaTexturesSuffix() const
{
    return bsaTexturesSuffix;
}

uint Games::getMeshesUser() const
{
    return meshesUser;
}

uint Games::getMeshesStream() const
{
    return meshesStream;
}

NiFileVersion Games::getMeshesFileVersion() const
{
    return meshesFileVersion;
}

DXGI_FORMAT Games::getTexturesFormat() const
{
    return texturesFormat;
}

hkPackFormat Games::getAnimationsFormat() const
{
    return animationFormat;
}

bool Games::getTexturesConvertTga() const
{
    return texturesConvertTga;
}

QList<DXGI_FORMAT> Games::getTexturesUnwantedFormats() const
{
    QList<DXGI_FORMAT> list;
    for (auto &format : texturesUnwantedFormats)
        list << QVariant::fromValue(format).value<DXGI_FORMAT>();
    return list;
}

QString Games::getIniPath() const
{
    return iniPath;
}

QString Games::getLogPath() const
{
    return logPath;
}

QString Games::getResourcePath() const
{
    return resourcePath;
}
