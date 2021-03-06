#ifndef __CONFIG_H__
#define __CONFIG_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
struct Config {

    Config(const std::string& configPath);
    virtual ~Config() = default;

    bool LoadFromJson();
    virtual void LoadFromConfig(const rapidjson::Document& document);

    std::string m_configPath;
};
}

#endif
