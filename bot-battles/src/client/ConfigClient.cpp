#include "ConfigClient.h"

#include "ClientComponent.h"
#include "GameClient.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ConfigClient::ConfigClient(const std::string& configPath)
    : Config(configPath)
{
}

//----------------------------------------------------------------------------------------------------
void ConfigClient::LoadFromConfig(const rapidjson::Document& document)
{
    Config::LoadFromConfig(document);

    assert(document.HasMember("client"));
    assert(document["client"].IsObject());
    const rapidjson::Value& client = document["client"];
    g_gameClient->GetClientComponent().lock()->LoadFromConfig(client);
}
}
