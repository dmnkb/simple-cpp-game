#include "Shader.h"
#include <iostream>
#include <stdio.h>

Shader::Shader(const char* vertexShader, const char* fragmentShader)
    : m_Program(glCreateProgram()), m_Vertex_shader(0), m_Fragment_shader(0)
{
    m_Vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(m_Vertex_shader, 1, &vertexShader, NULL);
    glCompileShader(m_Vertex_shader);
    checkShaderCompilation(m_Vertex_shader);

    m_Fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(m_Fragment_shader, 1, &fragmentShader, NULL);
    glCompileShader(m_Fragment_shader);
    checkShaderCompilation(m_Fragment_shader);

    glAttachShader(m_Program, m_Vertex_shader);
    glAttachShader(m_Program, m_Fragment_shader);
    glLinkProgram(m_Program);
    checkProgramLinking(m_Program);
}

Shader::~Shader()
{
    glDeleteShader(m_Vertex_shader);
    glDeleteShader(m_Fragment_shader);
    glDeleteProgram(m_Program);
}

void Shader::useProgram()
{
    glUseProgram(m_Program);
}

void Shader::setUniformMatrix4fv(const char* name, GLsizei count, GLboolean transpose, const GLfloat* value)
{
    auto location = getCachedLocation(name, ELocationType::UNIFORM);
    if (location != -1)
        glUniformMatrix4fv(location, count, transpose, value);
}

void Shader::setUniform1i(const char* name, GLint v0)
{
    auto location = getCachedLocation(name, ELocationType::UNIFORM);
    if (location != -1)
        glUniform1i(location, v0);
}

void Shader::setVertexAttribute(const char* name, GLint size, GLenum type, GLboolean normalized, GLsizei stride,
                                const void* pointer)
{
    auto location = getCachedLocation(name, ELocationType::ATTRIBUTE);
    if (location != -1)
    {
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, size, type, normalized, stride, pointer);
    }
}

GLint Shader::getCachedLocation(const char* name, ELocationType locationType)
{
    // Return cached location if available
    auto& locationMap = (locationType == ELocationType::ATTRIBUTE) ? m_AttributeLocations : m_UniformLocations;
    auto it = locationMap.find(name);
    if (it != locationMap.end())
        return it->second;
    // Get location from OpenGL otherwise and cache it
    GLint location = locationType == ELocationType::ATTRIBUTE ? glGetAttribLocation(m_Program, name)
                                                              : glGetUniformLocation(m_Program, name);
    if (location == -1)
    {
        std::cerr << "[ERROR] " << locationTypeToString.at(locationType) << " '" << name
                  << "' not found in shader program." << std::endl;
    }
    else
    {
        locationMap[name] = location;
    }
    return location;
}

void Shader::checkShaderCompilation(GLuint shader)
{
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLchar infoLog[1024];
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        fprintf(stderr, "[ERROR] Error compiling shader: %s\n", infoLog);
    }
}

void Shader::checkProgramLinking(GLuint program)
{
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        GLchar infoLog[1024];
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        fprintf(stderr, "[ERROR] Error linking program: %s\n", infoLog);
    }
}
