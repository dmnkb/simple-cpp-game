#include "Shader.h"
#include "pch.h"

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath) : m_ProgramID(glCreateProgram())
{
    auto shaders = readShaderFiles(vertexShaderPath, fragmentShaderPath);
    auto vertexShaderText = ((*shaders).vertexShader).c_str();
    auto fragmentShaderText = (*shaders).fragmentShader.c_str();
    assert(vertexShaderText && "[ERROR] in Shader::Shader: vertexShader is falsy.");
    assert(fragmentShaderText && "[ERROR] in Shader::Shader: fragmentShader is falsy.");

    printf("[Shader] Create vertex shader\n");
    m_VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(m_VertexShaderID, 1, &vertexShaderText, NULL);
    glCompileShader(m_VertexShaderID);
    checkShaderCompilation(m_VertexShaderID);

    printf("[Shader] Create fragment shader\n");
    m_FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(m_FragmentShaderID, 1, &fragmentShaderText, NULL);
    glCompileShader(m_FragmentShaderID);
    checkShaderCompilation(m_FragmentShaderID);

    printf("[Shader] Link shader\n");
    glAttachShader(m_ProgramID, m_VertexShaderID);
    glAttachShader(m_ProgramID, m_FragmentShaderID);
    glLinkProgram(m_ProgramID);
    checkProgramLinking(m_ProgramID);
}

Shader::~Shader()
{
    unbind();
    glDeleteShader(m_FragmentShaderID);
    glDeleteShader(m_VertexShaderID);
    glDeleteProgram(m_ProgramID);
}

std::optional<ShaderSource> Shader::readShaderFiles(const char* vertexShaderPath, const char* fragmentShaderPath)
{
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        vShaderFile.open(vertexShaderPath);
        fShaderFile.open(fragmentShaderPath);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();

        shaderNames = {.vertex = vertexShaderPath, .fragment = fragmentShaderPath};
    }
    catch (std::ifstream::failure e)
    {
        assert(false && "Shader::readShaderFiles: Shader file not succesfully read.");
        return std::nullopt;
    }

    return ShaderSource({vertexCode, fragmentCode});
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
        std::cerr << "[Shader] ERROR: (" << shaderNames.vertex << "): Attribute '" << name
                  << "' not found in shader program." << std::endl;
    else
    {
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, size, type, normalized, stride, pointer);
    }
}

void Shader::setUniformFloat(const char* name, const float value)
{
    auto location = getCachedLocation(name);
    if (location >= 0)
        glUniform1f(location, value);
}

void Shader::setUniformMatrix4fv(const char* name, const glm::mat4 value)
{
    auto location = getCachedLocation(name);
    if (location >= 0)
        glUniformMatrix4fv(location, 1, GL_FALSE, (const GLfloat*)&value);
}

void Shader::setUniformMatrix4fvArray(const char* name, const int count, glm::f32* pointer)
{
    auto location = getCachedLocation(name);
    if (location >= 0)
        glUniformMatrix4fv(location, count, GL_FALSE, pointer);
}

void Shader::setUniformMatrix3fv(const char* name, const glm::mat3 value)
{
    auto location = getCachedLocation(name);
    if (location >= 0)
        glUniformMatrix3fv(location, 1, GL_FALSE, (const GLfloat*)&value);
}

void Shader::setUniform3fv(const char* name, const glm::vec3 value)
{
    auto location = getCachedLocation(name);
    if (location >= 0)
        glUniform3fv(location, 1, (const GLfloat*)&value);
}

void Shader::setUniform4fv(const char* name, const glm::vec4 value)
{
    auto location = getCachedLocation(name);
    if (location >= 0)
        glUniform4fv(location, 1, (const GLfloat*)&value);
}

void Shader::setUniform1i(const char* name, GLint value)
{
    auto location = getCachedLocation(name);
    if (location >= 0)
        glUniform1i(location, value);
}

void Shader::setUniform1f(const char* name, float value)
{
    auto location = getCachedLocation(name);
    if (location >= 0)
        glUniform1f(location, value);
}

void Shader::setUniform1iv(const char* name, GLint samplerIDs[16])
{
    auto location = getCachedLocation(name);
    if (location >= 0)
        glUniform1iv(location, 16, samplerIDs);
}

void Shader::setIntArray(const char* name, GLint* values, GLsizei count)
{
    GLint location = getCachedLocation(name);
    if (location >= 0)
        glUniform1iv(location, count, values);
}

GLint Shader::getCachedLocation(const char* name)
{
    auto it = m_UniformLocationCache.find(name);
    if (it != m_UniformLocationCache.end())
        return it->second;

    GLint location = glGetUniformLocation(m_ProgramID, name);
    // Cache exact name; for arrays, callers must pass "name[index]" or "name[0]"
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

const bool Shader::hasUniform(const char* name)
{
    return getCachedLocation(name) >= 0;
}
