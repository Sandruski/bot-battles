#include "Config.h"

#include "Game.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
Config::Config(const char* configPath)
    : m_configPath(configPath)
    , m_name()
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

    assert(document.HasMember("window"));
    assert(document["window"].IsObject());
    const rapidjson::Value& window = document["window"];
    g_game->GetWindowComponent().LoadFromConfig(window);
}
}
