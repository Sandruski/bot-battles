#include "ShaderImporter.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
U32 ShaderImporter::Load(const char* vertexShaderSource, const char* fragmentShaderSource) const
{
    assert(vertexShaderSource != nullptr && fragmentShaderSource != nullptr);

    U32 vertexShader = CreateAndCompileVertexShader(vertexShaderSource);
    U32 fragmentShader = CreateAndCompileFragmentShader(fragmentShaderSource);
    U32 program = CreateAndLinkProgram(vertexShader, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

//----------------------------------------------------------------------------------------------------
void ShaderImporter::UnLoad(U32& program) const
{
    glDeleteProgram(program);
    program = 0;
}

//----------------------------------------------------------------------------------------------------
U32 ShaderImporter::CreateAndCompileVertexShader(const char* vertexShaderSource) const
{
    assert(vertexShaderSource != nullptr);

    U32 vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    CheckCompileStatus(vertexShader);

    return vertexShader;
}

//----------------------------------------------------------------------------------------------------
U32 ShaderImporter::CreateAndCompileFragmentShader(const char* fragmentShaderSource) const
{
    assert(fragmentShaderSource != nullptr);

    U32 fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    CheckCompileStatus(fragmentShader);

    return fragmentShader;
}

//----------------------------------------------------------------------------------------------------
U32 ShaderImporter::CreateAndLinkProgram(U32 vertexShader, U32 fragmentShader) const
{
    U32 program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    CheckLinkStatus(program);

    return program;
}

//----------------------------------------------------------------------------------------------------
void ShaderImporter::CheckCompileStatus(U32 shader) const
{
    I32 success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        ELOG("Shader could not be compiled. ERROR: %s", infoLog);
    }
}

//----------------------------------------------------------------------------------------------------
void ShaderImporter::CheckLinkStatus(U32 program) const
{
    I32 success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        ELOG("Program could not be linked. ERROR: %s", infoLog);
    }
}
}
