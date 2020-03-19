#ifndef __FILE_SYSTEM_H__
#define __FILE_SYSTEM_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
class FileSystem {
public:
    bool ParseJsonFromFile(const std::string& path, rapidjson::Document& document) const;
};
}

#endif
