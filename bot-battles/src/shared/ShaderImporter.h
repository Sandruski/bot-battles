#ifndef __SHADER_IMPORTER_H__
#define __SHADER_IMPORTER_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
class ShaderImporter {
public:
    U32 Load(const char* vertexShaderSource, const char* fragmentShaderSource) const;
    void UnLoad(U32& program) const;

private:
    U32 CreateAndCompileVertexShader(const char* vertexShaderSource) const;
    U32 CreateAndCompileFragmentShader(const char* fragmentShaderSource) const;
    U32 CreateAndLinkProgram(U32 vertexShader, U32 fragmentShader) const;
    void CheckCompileStatus(U32 shader) const;
    void CheckLinkStatus(U32 program) const;
};
}

#endif
