#pragma once

#define GLAD_GL_IMPLEMENTATION
#include <functional>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#include <map>

enum ELocationType
{
    UNIFORM,
    ATTRIBUTE,
};

typedef std::map<ELocationType, std::string> TlocationTypeMap;

static TlocationTypeMap locationTypeToString = {{ELocationType::UNIFORM, "Uniform"},
                                                {ELocationType::ATTRIBUTE, "Attribute"}};

class Shader
{
  public:
    Shader(const char* vertexShader, const char* fragmentShader);
    ~Shader();

    void useProgram();
    void setUniformMatrix4fv(const char* name, GLsizei count, GLboolean transpose, const GLfloat* value);
    void setUniform1i(const char* name, GLint v0);
    void setVertexAttribute(const char* name, GLint size, GLenum type, GLboolean normalized, GLsizei stride,
                            const void* pointer);

  private:
    const GLuint m_Program;

    GLuint m_Vertex_shader;
    GLuint m_Fragment_shader;

    GLint getLocation(const char* name, ELocationType locationType);
    void checkShaderCompilation(GLuint shader);
    void checkProgramLinking(GLuint shader);

    std::unordered_map<std::string, GLint> m_UniformLocations;
    std::unordered_map<std::string, GLint> m_AttributeLocations;
};