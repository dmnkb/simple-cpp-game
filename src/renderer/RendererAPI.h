#include "ClearColor.h"
#include "Mesh.h"
#include "pch.h"

struct RendererAPIData
{
    GLuint instanceBuffer = -1;
    SClearColor clearColor = clearColorValues[static_cast<int>(EClearColor::CharcoalGray)];
    int drawCallCount = 0;
};

class RendererAPI
{
  public:
    static void init();
    static void shutdown();
    static void drawInstanced(const Ref<Mesh>& mesh, const std::vector<glm::mat4>& transforms);
    static void setClearColor(const SClearColor& color);
    static int getDrawCallCount();
    static void resetDrawCallCount();

  private:
    static void bindInstanceBuffer(const std::vector<glm::mat4>& transforms);
    static void unbindInstanceBuffer();

    // Debugging
    static void GLAPIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                         const GLchar* message, const void* userParam);
    static void enableOpenGLDebugOutput();

  private:
};