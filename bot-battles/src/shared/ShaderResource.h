#ifndef __SHADER_RESOURCE_H__
#define __SHADER_RESOURCE_H__

#include "Resource.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class ShaderResource : public Resource {
public:
    ShaderResource(U32 id, const char* dir, const char* file);

    bool Load() override;
    bool UnLoad() override;
    bool ReLoad(const char* vertexShaderSource, const char* fragmentShaderSource);

    U32 GetProgram() const;

private:
    U32 m_program;
};
}

#endif
