#include "ConfigServer.h"

#include "GameServer.h"
#include "ServerComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ConfigServer::ConfigServer(const std::string& configPath)
    : Config(configPath)
{
}

//----------------------------------------------------------------------------------------------------
void ConfigServer::ReadDocument(const rapidjson::Document& document)
{
    Config::ReadDocument(document);

    assert(document.HasMember("server"));
    assert(document["server"].IsObject());
    const rapidjson::Value& server = document["server"];
    g_gameServer->GetServerComponent().LoadFromConfig(server);
}
}
