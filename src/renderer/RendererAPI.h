#include "Mesh.h"
#include "pch.h"

struct RendererAPIData
{
    GLuint instanceBuffer;
};

class RendererAPI
{
  public:
    static void init();
    static void shutdown();
    static void drawInstanced(const Ref<Mesh>& mesh, const std::vector<glm::mat4>& transforms);

  private:
    static void bindInstanceBuffer(const std::vector<glm::mat4>& transforms);
    static void unbindInstanceBuffer();

    // Debugging
    static void GLAPIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                         const GLchar* message, const void* userParam);
    static void enableOpenGLDebugOutput();

  private:
};