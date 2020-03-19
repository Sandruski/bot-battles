#include "FileSystem.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
bool FileSystem::ParseJsonFromFile(const std::string& path, rapidjson::Document& document) const
{
    errno_t err = 0;
    FILE* file = nullptr;
    err = fopen_s(&file, path.c_str(), "rb");
    if (err != 0) {
        ELOG("File %s could not be opened", path.c_str());
        return false;
    }

    char readBuffer[65536];
    rapidjson::FileReadStream readStream(file, readBuffer, sizeof(readBuffer));

    err = fclose(file);
    if (err != 0) {
        ELOG("File %s could not be closed", path.c_str());
        return false;
    }

    document.ParseStream(readStream);

    return true;
}
}
