#ifndef __CONFIG_H__
#define __CONFIG_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
struct Config {

    Config(const std::string& configPath);
    virtual ~Config() = default;

    bool LoadFromJson();
    virtual void ReadDocument(const rapidjson::Document& document);

    std::string m_configPath;
    std::string m_name;
    std::string m_offlineSceneName;
    std::string m_onlineSceneName;
};
}

#endif
