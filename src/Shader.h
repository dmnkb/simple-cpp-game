#pragma once

#include "pch.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader
{
  public:
    // TODO: Read in shader file
    Shader(const char* vertexShader, const char* fragmentShader);
    ~Shader();

    void bind();
    void unbind();

    void setVertexAttribute(const char* name, GLint size, GLenum type, GLboolean normalized, GLsizei stride,
                            const void* pointer);

    void setUniformMatrix4fv(const char* name, const glm::mat4 value);
    void setUniformMatrix3fv(const char* name, const glm::mat3 value);
    void setUniform3fv(const char* name, const glm::vec3 value);
    void setUniform1i(const char* name, GLint v0);
    void setUniform1iv(const char* name, GLint samplerIDs[16]);
    void setIntArray(const char* name, GLint* values, GLsizei count);

  private:
    GLuint m_ProgramID;
    GLuint m_VertexShaderID;
    GLuint m_FragmentShaderID;

    GLint getCachedLocation(const char* name);
    void checkShaderCompilation(GLuint shader);
    void checkProgramLinking(GLuint shader);

    std::unordered_map<const char*, GLint> m_UniformLocationCache;
};