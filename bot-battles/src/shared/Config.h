#ifndef __CONFIG_H__
#define __CONFIG_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
class Config {
public:
    Config(const char* path);
    ~Config();

    bool LoadFromJson();

private:
    char* m_path;
};
}

#endif
