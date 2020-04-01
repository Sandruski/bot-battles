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
    std::string m_name;
    std::string m_mainMenuSceneName;
    std::string m_gameplaySceneName;
    std::string m_scoreboardSceneName;
};
}

#endif
