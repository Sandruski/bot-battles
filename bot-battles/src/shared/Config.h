#ifndef __CONFIG_H__
#define __CONFIG_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
struct Config {

    Config(const char* m_configPath);
    virtual ~Config() = default;

    bool LoadFromJson();
    virtual void ReadDocument(const rapidjson::Document& document);

    std::string m_configPath;
    std::string m_name;
};
}

#endif
