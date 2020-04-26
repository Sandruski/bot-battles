#ifndef __FILE_SYSTEM_H__
#define __FILE_SYSTEM_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
class FileSystem {
public:
    bool ParseJsonFromFile(const std::string& path, rapidjson::Document& document) const;
    std::vector<std::string> GetFilesFromDirectory(const std::string& path, const std::string& extension) const;

    std::string GetName(const std::string& path) const;
    std::string GetExtension(const std::string& path) const;
};
}

#endif
