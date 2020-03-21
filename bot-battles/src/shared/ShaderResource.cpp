#include "ShaderResource.h"

#include "Game.h"
#include "ShaderImporter.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ShaderResource::ShaderResource(U32 id, const char* dir, const char* file)
    : Resource(id, dir, file)
    , m_program(0)
{
}

//----------------------------------------------------------------------------------------------------
bool ShaderResource::Load()
{
    return true;
}

//----------------------------------------------------------------------------------------------------
bool ShaderResource::UnLoad()
{
    if (m_program > 0) {
        g_game->GetShaderImporter().UnLoad(m_program);
    }

    return (m_program == 0);
}

//----------------------------------------------------------------------------------------------------
bool ShaderResource::ForceLoad(const char* vertexShaderSource, const char* fragmentShaderSource)
{
    assert(vertexShaderSource != nullptr && fragmentShaderSource != nullptr);

    UnLoad();

    m_program = g_game->GetShaderImporter().Load(vertexShaderSource, fragmentShaderSource);

    return (m_program > 0);
}

//----------------------------------------------------------------------------------------------------
U32 ShaderResource::GetProgram() const
{
    return m_program;
}
}
