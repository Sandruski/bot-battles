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
void ConfigClient::ReadDocument(const rapidjson::Document& document)
{
    Config::ReadDocument(document);

    assert(document.HasMember("client"));
    assert(document["client"].IsObject());
    const rapidjson::Value& client = document["client"];
    g_gameClient->GetClientComponent().LoadFromConfig(client);
}
}
