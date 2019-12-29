#ifndef __RESOURCE_H__
#define __RESOURCE_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
class Resource {
public:
    Resource(U32 id, const char* file, const char* dir);

    const std::string& GetFile() const;
    const std::string& GetDir() const;
    const std::string& GetPath() const;

    U32 GetID() const;
    U32 GetReferences() const;

    bool HasReferences() const;

    U32 IncreaseReferences();
    U32 DecreaseReferences();

    virtual bool Load() = 0;
    virtual bool UnLoad() = 0;

private:
    std::string m_file;
    std::string m_dir;
    std::string m_path;

    U32 m_id;
    U32 m_references;
};
}

#endif
