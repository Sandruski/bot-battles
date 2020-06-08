#include "Config.h"

#include "FileSystem.h"
#include "Game.h"
#include "RendererComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
Config::Config(const std::string& configPath)
    : m_configPath(configPath)
{
}

//----------------------------------------------------------------------------------------------------
bool Config::LoadFromJson()
{
    rapidjson::Document document;
    bool ret = g_game->GetFileSystem().ParseJsonFromFile(m_configPath, document);
    if (!ret) {
        ELOG("Config file could not be loaded");
        return false;
    }

    LoadFromConfig(document);

    return true;
}

//----------------------------------------------------------------------------------------------------
void Config::LoadFromConfig(const rapidjson::Document& document)
{
    assert(document.IsObject());

    assert(document.HasMember("window"));
    assert(document["window"].IsObject());
    const rapidjson::Value& window = document["window"];
    g_game->GetWindowComponent().lock()->LoadFromConfig(window);

    assert(document.HasMember("renderer"));
    assert(document["renderer"].IsObject());
    const rapidjson::Value& renderer = document["renderer"];
    g_game->GetRendererComponent().lock()->LoadFromConfig(renderer);
}
}
