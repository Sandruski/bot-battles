#ifndef __CONFIG_SERVER_H__
#define __CONFIG_SERVER_H__

#include "Config.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
struct ConfigServer : public Config {

    ConfigServer(const std::string& configPath);

    void LoadFromConfig(const rapidjson::Document& document) override;
};
}

#endif
