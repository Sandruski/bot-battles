#ifndef __CONFIG_CLIENT_H__
#define __CONFIG_CLIENT_H__

#include "Config.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
struct ConfigClient : public Config {

    ConfigClient(const char* m_configPath);

    void ReadDocument(const rapidjson::Document& document) override;
};
}

#endif
