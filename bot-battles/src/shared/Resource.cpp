#include "Resource.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
Resource::Resource(U32 id, const char* file, const char* dir)
    : m_id(id)
    , m_references(0)
    , m_file(file)
    , m_dir(dir)
    , m_path()
{
    m_path.append(dir);
    m_path.append(file);
}

//----------------------------------------------------------------------------------------------------
const std::string& Resource::GetFile() const
{
    return m_file;
}

//----------------------------------------------------------------------------------------------------
const std::string& Resource::GetDir() const
{
    return m_dir;
}

//----------------------------------------------------------------------------------------------------
const std::string& Resource::GetPath() const
{
    return m_path;
}

//----------------------------------------------------------------------------------------------------
U32 Resource::GetID() const
{
    return m_id;
}

//----------------------------------------------------------------------------------------------------
U32 Resource::GetReferences() const
{
    return m_references;
}

//----------------------------------------------------------------------------------------------------
bool Resource::HasReferences() const
{
    return m_references > 0;
}

//----------------------------------------------------------------------------------------------------
U32 Resource::IncreaseReferences()
{
    ++m_references;
    return m_references;
}

//----------------------------------------------------------------------------------------------------
U32 Resource::DecreaseReferences()
{
    --m_references;
    return m_references;
}
}
