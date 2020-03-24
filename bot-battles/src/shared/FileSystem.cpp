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

//----------------------------------------------------------------------------------------------------
std::vector<std::string> FileSystem::GetFilesFromDirectory(const std::string& path) const
{
    std::vector<std::string> entries;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
        std::string entryPath = entry.path().string();
        std::string extension = GetExtension(entryPath);
        if (extension == DEFAULT_EXTENSION) {
            std::size_t i = entryPath.find(MAPS_DIR);
            if (i != std::string::npos) {
                entryPath.erase(i, strlen(MAPS_DIR));
                entries.emplace_back(entryPath);
            }
        }
    }

    return entries;
}

//----------------------------------------------------------------------------------------------------
std::string FileSystem::GetExtension(const std::string& path) const
{
    std::string extension;

    std::size_t i = path.find_last_of(".");
    if (i != std::string::npos) {
        extension = path.substr(i);
    }

    return extension;
}
}
