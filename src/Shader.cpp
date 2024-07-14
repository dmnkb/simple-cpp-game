#include "Shader.h"
#include <iostream>
#include <stdio.h>

Shader::Shader(const char* vertexShader, const char* fragmentShader) : m_ProgramID(glCreateProgram())
{
    printf("Create vertex shader\n");
    m_VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(m_VertexShaderID, 1, &vertexShader, NULL);
    glCompileShader(m_VertexShaderID);
    checkShaderCompilation(m_VertexShaderID);

    printf("Create fragment shader\n");
    m_FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(m_FragmentShaderID, 1, &fragmentShader, NULL);
    glCompileShader(m_FragmentShaderID);
    checkShaderCompilation(m_FragmentShaderID);

    printf("Link shader\n");
    glAttachShader(m_ProgramID, m_VertexShaderID);
    glAttachShader(m_ProgramID, m_FragmentShaderID);
    glLinkProgram(m_ProgramID);
    checkProgramLinking(m_ProgramID);
}

Shader::~Shader()
{
    unbind();
    glDeleteShader(m_VertexShaderID);
    glDeleteShader(m_VertexShaderID);
    glDeleteProgram(m_ProgramID);
}

void Shader::bind()
{
    glUseProgram(m_ProgramID);
}

void Shader::unbind()
{
    glUseProgram(0);
}

void Shader::setVertexAttribute(const char* name, GLint size, GLenum type, GLboolean normalized, GLsizei stride,
                                const void* pointer)
{
    GLint location = glGetAttribLocation(m_ProgramID, name);
    if (location == -1)
        std::cerr << "[ERROR] Uniform '" << name << "' not found in shader program." << std::endl;
    else
    {
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, size, type, normalized, stride, pointer);
    }
}

void Shader::setUniformMatrix4fv(const char* name, const glm::mat4 value)
{
    auto location = getCachedLocation(name);
    if (location != -1)
        glUniformMatrix4fv(location, 1, GL_FALSE, (const GLfloat*)&value);
}

void Shader::setUniform1i(const char* name, GLint v0)
{
    auto location = getCachedLocation(name);
    if (location != -1)
        glUniform1i(location, v0);
}

GLint Shader::getCachedLocation(const char* name)
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        return m_UniformLocationCache[name];

    GLint location = glGetUniformLocation(m_ProgramID, name);
    if (location == -1)
        std::cerr << "[ERROR] Uniform '" << name << "' not found in shader program." << std::endl;

    m_UniformLocationCache[name] = location;
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
