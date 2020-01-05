#include "Config.h"

#include "Game.h"
#include "RendererComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
Config::Config(const std::string& configPath)
    : m_configPath(configPath)
    , m_name()
    , m_initialSceneName()
#ifdef _CLIENT
    , m_offlineSceneName()
#endif
    , m_onlineSceneName()
{
}

//----------------------------------------------------------------------------------------------------
bool Config::LoadFromJson()
{
    errno_t err = 0;
    FILE* file = nullptr;
    err = fopen_s(&file, m_configPath.c_str(), "rb");
    if (err != 0) {
        ELOG("File %s could not be opened", m_configPath.c_str());
        return false;
    }

    char readBuffer[65536];
    rapidjson::FileReadStream readStream(file, readBuffer, sizeof(readBuffer));

    rapidjson::Document document;
    document.ParseStream(readStream);

    fclose(file);

    ReadDocument(document);

    return true;
}

//----------------------------------------------------------------------------------------------------
void Config::ReadDocument(const rapidjson::Document& document)
{
    assert(document.IsObject());

    assert(document.HasMember("game"));
    assert(document["game"].IsObject());
    const rapidjson::Value& game = document["game"];

    assert(game.HasMember("name"));
    assert(game["name"].IsString());
    m_name = game["name"].GetString();

    assert(game.HasMember("initialScene"));
    assert(game["initialScene"].IsString());
    m_initialSceneName = game["initialScene"].GetString();

#ifdef _CLIENT
    assert(game.HasMember("offlineScene"));
    assert(game["offlineScene"].IsString());
    m_offlineSceneName = game["offlineScene"].GetString();
#endif

    assert(game.HasMember("onlineScene"));
    assert(game["onlineScene"].IsString());
    m_onlineSceneName = game["onlineScene"].GetString();

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
