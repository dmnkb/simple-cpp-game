#pragma once

#include <glad/glad.h>
#include <iostream>

namespace Engine
{
inline void GLClearErrors()
{
    while (glGetError() != GL_NO_ERROR)
    {
    }
}

inline const char* GLErrorToString(GLenum error)
{
    switch (error)
    {
    case GL_INVALID_ENUM:
        return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE:
        return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION:
        return "GL_INVALID_OPERATION";
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        return "GL_INVALID_FRAMEBUFFER_OPERATION";
    case GL_OUT_OF_MEMORY:
        return "GL_OUT_OF_MEMORY";
    default:
        return "UNKNOWN_ERROR";
    }
}

inline bool GLLogCall(const char* function, const char* file, int line)
{
    bool ok = true;
    while (GLenum error = glGetError())
    {
        std::cerr << "[OpenGL Error] " << GLErrorToString(error) << " (0x" << std::hex << error << std::dec << ") in "
                  << function << " at " << file << ":" << line << '\n';
        ok = false;
    }
    return ok;
}
} // namespace Engine

#ifndef NDEBUG
#define GLCall(x)                                                                                                      \
    do                                                                                                                 \
    {                                                                                                                  \
        ::Engine::GLClearErrors();                                                                                     \
        x;                                                                                                             \
        ::Engine::GLLogCall(#x, __FILE__, __LINE__);                                                                   \
    } while (false)
#else
#define GLCall(x) x
#endif
