#include "Config.h"

#include "document.h"
#include "filereadstream.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
Config::Config(const char* path)
{
    assert(path != nullptr);

    size_t pathSize = strlen(path);
    m_path = new char[pathSize + 1];
    memcpy(m_path, path, (pathSize + 1) * sizeof(char));
}

//----------------------------------------------------------------------------------------------------
Config::~Config()
{
    SAFE_DELETE_POINTER(m_path);
}

//----------------------------------------------------------------------------------------------------
bool Config::LoadFromJson()
{
    errno_t err = 0;
    FILE* file = nullptr;
    err = fopen_s(&file, m_path, "rb");
    if (err != 0) {
        ELOG("File %s could not be opened", m_path);
        return false;
    }

    char readBuffer[65536];
    rapidjson::FileReadStream readStream(file, readBuffer, sizeof(readBuffer));

    rapidjson::Document document;
    document.ParseStream(readStream);

    fclose(file);

    return true;
}
}
