#pragma once

#define GLAD_GL_IMPLEMENTATION
#include <functional>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#include <map>

struct SShaderLocation
{
    GLint location;
    GLint type;
    const char* name;
};
class Shader
{
  public:
    Shader(const char* vertexShader, const char* fragmentShader);
    ~Shader();

    void bind();
    void unbind();

    void setVertexAttribute(const char* name, GLint size, GLenum type, GLboolean normalized, GLsizei stride,
                            const void* pointer);

    void setUniformMatrix4fv(const char* name, const glm::mat4 value);
    void setUniform1i(const char* name, GLint v0);

  private:
    const GLuint m_ProgramID;
    GLuint m_VertexShaderID;
    GLuint m_FragmentShaderID;

    GLint getCachedLocation(const char* name);
    void checkShaderCompilation(GLuint shader);
    void checkProgramLinking(GLuint shader);

    std::unordered_map<std::string, GLint> m_UniformLocationCache;
};