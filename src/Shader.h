#pragma once

#include "pch.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

struct ShaderSource
{
    std::string vertexShader;
    std::string fragmentShader;
};

class Shader
{
  public:
    struct ShaderNames
    {
        const char* vertex = "";
        const char* fragment = "";
    } shaderNames;

    Shader(const char* vertexShaderPath, const char* fragmentShaderPath);
    ~Shader();

    std::optional<ShaderSource> readShaderFiles(const char* vertexShaderPath, const char* fragmentShaderPath);

    void bind();
    void unbind();

    GLuint getProgramID()
    {
        return m_ProgramID;
    }

    void setVertexAttribute(const char* name, GLint size, GLenum type, GLboolean normalized, GLsizei stride,
                            const void* pointer);

    void setUniformFloat(const char* name, const float value);
    void setUniformMatrix4fv(const char* name, const glm::mat4 value);
    void setUniformMatrix3fv(const char* name, const glm::mat3 value);
    void setUniform3fv(const char* name, const glm::vec3 value);
    void setUniform1i(const char* name, GLint value);
    void setUniform1iv(const char* name, GLint samplerIDs[16]);
    void setIntArray(const char* name, GLint* values, GLsizei count);

    const bool hasUniform(const char* name);

  private:
    GLuint m_ProgramID;
    GLuint m_VertexShaderID;
    GLuint m_FragmentShaderID;

    GLint getCachedLocation(const char* name);
    void checkShaderCompilation(GLuint shader);
    void checkProgramLinking(GLuint shader);

    std::unordered_map<const char*, GLint> m_UniformLocationCache;
};