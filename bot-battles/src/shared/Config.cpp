#include "Config.h"

#include "Game.h"
#include "RendererComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
Config::Config(const std::string& configPath)
    : m_configPath(configPath)
    , m_name()
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

    assert(document.HasMember("game"));
    assert(document["game"].IsObject());
    const rapidjson::Value& game = document["game"];

    assert(game.HasMember("name"));
    assert(game["name"].IsString());
    m_name = game["name"].GetString();

    assert(document.HasMember("window"));
    assert(document["window"].IsObject());
    const rapidjson::Value& window = document["window"];
    g_game->GetWindowComponent().LoadFromConfig(window);

    assert(document.HasMember("renderer"));
    assert(document["renderer"].IsObject());
    const rapidjson::Value& renderer = document["renderer"];
    g_game->GetRendererComponent().LoadFromConfig(renderer);
}
}
