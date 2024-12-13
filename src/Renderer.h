#pragma once

#include "Camera.h"
#include "Light.h"
#include "Mesh.h"
#include "Shader.h"

#define MAX_MESH_COUNT 256
#define MAX_TRANSFORMS_PER_MESH 256

/**
 * An atomic, stateless POD (plain old data) that contains mesh, material and transform.
 */
struct Renderable
{
    std::shared_ptr<Mesh> mesh; // Geometry data
    // std::shared_ptr<Material> material; // TODO: Shader, textures, material properties
    std::shared_ptr<Shader> shader; // TODO: replace with material
    glm::mat4 transform;
};

class Renderer
{
  public:
    static void init();

    // Setup viewMatrix & viewProjectionMatrix
    static void beginScene(Camera& camera);
    static void update();

    static void submitLights(const std::vector<Light>& lights);
    static void submitRenderable(Renderable renderable);

  private:
    static void drawInstanced();

    static void GLAPIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                         const GLchar* message, const void* userParam);
    static void enableOpenGLDebugOutput();
};