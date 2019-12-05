#include "pch.hpp"

#include "Settings/AdvancedSettings.hpp"
#include "Settings/StandardSettings.hpp"

namespace CAO {

void initJSONStandard(nlohmann::json &json);
void initJSONAdvanced(nlohmann::json &json);

/*!
 * \brief initJSON Will initialize a JSON with CAO default settings
 * \param json The JSON to init
 */
inline void initJSON(nlohmann::json &json)
{
    auto init = [](const std::string &key, nlohmann::json &json) {
        if (!json.contains(key))
            json[key] = nlohmann::json::value_t::object;
    };

    //Root
    init("General", json);
    init("BSA", json);
    init("Textures", json);
    init("Meshes", json);
    init("Animations", json);
    init("Advanced", json);

    nlohmann::json &Textures = json.at("Textures");
    init("Resizing", Textures);

    nlohmann::json &Resizing = Textures.at("Resizing");
    init("BySize", Resizing);
    init("ByRatio", Resizing);

    nlohmann::json &Advanced = json.at("Advanced");
    init("BSA", Advanced);
    init("Textures", Advanced);
    init("Meshes", Advanced);
    init("Animations", Advanced);

    initJSONStandard(json);
    initJSONAdvanced(json);
}

inline void initJSONStandard(nlohmann::json &json)
{
    auto init = [](nlohmann::json &json, auto value) {
        if (json.is_null())
            json = value;
    };

    init(json.at("General")["bDebugLog"], false);
    init(json.at("General")["bDryRun"], false);
    init(json.at("General")["eMode"], StandardSettings::OptimizationMode::SingleMod);
    init(json.at("General")["sUserPath"], "");

    init(json.at("BSA")["bBsaExtract"], false);
    init(json.at("BSA")["bBsaCreate"], false);
    init(json.at("BSA")["bBsaDeleteBackup"], false);
    init(json.at("BSA")["bBsaProcessContent"], false);
    init(json.at("BSA")["bBsaLeastBsaPossible"], false);
    init(json.at("BSA")["bBsaCreateDummies"], false);

    init(json.at("Textures")["bTexturesNecessary"], true);
    init(json.at("Textures")["bTexturesCompress"], false);
    init(json.at("Textures")["TexturesMipmaps"], false);

    init(json.at("Textures").at("Resizing").at("BySize")["Enabled"], false);
    init(json.at("Textures").at("Resizing").at("BySize")["Width"], false);
    init(json.at("Textures").at("Resizing").at("BySize")["Height"], false);

    init(json.at("Textures").at("Resizing").at("ByRatio")["Enabled"], false);
    init(json.at("Textures").at("Resizing").at("ByRatio")["Width"], false);
    init(json.at("Textures").at("Resizing").at("ByRatio")["Height"], false);

    init(json.at("Meshes")["iMeshesOptimizationLevel"], 1);
    init(json.at("Meshes")["bMeshesHeadparts"], true);
    init(json.at("Meshes")["bMeshesResave"], false);

    init(json.at("Animations")["bAnimationsOptimization"], true);
}

inline void initJSONAdvanced(nlohmann::json &json)
{
    auto init = [](nlohmann::json &json, auto value) {
        if (json.is_null())
            json = value;
    };

    init(json.at("Advanced").at("BSA")["bBSATabEnabled"], true);
    init(json.at("Advanced").at("BSA")["sBSAExtension"], ".bsa");

    init(json.at("Advanced").at("BSA")["eBSAFormat"], bsa_archive_type_e::baSSE);
    init(json.at("Advanced").at("BSA")["iBSAMaxSize"], GigaByte * 2);
    init(json.at("Advanced").at("BSA")["sBSASuffix"], ".bsa");

    init(json.at("Advanced").at("BSA")["bBSATexturesEnabled"], true);
    init(json.at("Advanced").at("BSA")["eBSATexturesFormat"], bsa_archive_type_e::baSSE);
    init(json.at("Advanced").at("BSA")["iBSATexturesMaxSize"], GigaByte * 2);
    init(json.at("Advanced").at("BSA")["sBSATexturesSuffix"], "- Textures.bsa");

    init(json.at("Advanced").at("Textures")["bTexturesTabEnabled"], true);
    init(json.at("Advanced").at("Textures")["eTexturesFormat"], DXGI_FORMAT_BC7_UNORM);
    init(json.at("Advanced").at("Textures")["bTexturesTGAConvertEnabled"], true);
    init(json.at("Advanced").at("Textures")["bTexturesInterfaceConvert"], true);
    if (auto &j = json.at("Advanced").at("Textures")["slTexturesUnwantedFormats"]; !j.is_array())
        j = std::vector<DXGI_FORMAT>{DXGI_FORMAT_B5G6R5_UNORM, DXGI_FORMAT_B5G5R5A1_UNORM};

    init(json.at("Advanced").at("Meshes")["bMeshesTabEnabled"], true);
    init(json.at("Advanced").at("Meshes")["eMeshesFileVersion"], NiFileVersion::V20_2_0_7);
    init(json.at("Advanced").at("Meshes")["iMeshesStream"], 100);
    init(json.at("Advanced").at("Meshes")["iMeshesUser"], 12);

    init(json.at("Advanced").at("Animations")["bAnimationsTabEnabled"], true);
    init(json.at("Advanced").at("Animations")["eAnimationsFormat"], hkPackFormat::HKPF_AMD64);
}

} // namespace CAO
