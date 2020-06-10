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
void ConfigServer::LoadFromConfig(const rapidjson::Document& document)
{
    Config::LoadFromConfig(document);

    if (document.HasMember("server") && document["server"].IsObject()) {
        const rapidjson::Value& server = document["server"];
        g_gameServer->GetServerComponent().lock()->LoadFromConfig(server);
    }
}
}
