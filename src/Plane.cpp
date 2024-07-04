#include "Plane.h"
#include "Camera.h"
#define STB_IMAGE_IMPLEMENTATION
#include <filesystem>
#include <glm/glm.hpp>
#include <stb_image.h>

Plane::Plane(const int width, const int height) : m_Shader(vertex_shader_text, fragment_shader_text)
{
    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint index_buffer;
    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &m_VertexArray);
    glBindVertexArray(m_VertexArray);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

    m_Shader.setVertexAttribute("vPos", 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
    m_Shader.setVertexAttribute("vUV", 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

    std::string texturePath = "assets/texture_02.png";
    if (!std::filesystem::exists(texturePath))
    {
        fprintf(stderr, "[ERROR] Texture file not found: %s\n", texturePath.c_str());
        return;
    }

    int texWidth, texHeight, nrChannels;
    unsigned char* data = stbi_load(texturePath.c_str(), &texWidth, &texHeight, &nrChannels, 0);

    if (!data)
    {
        fprintf(stderr, "[ERROR] Failed to load texture\n");
        return;
    }

    glGenTextures(1, &m_Texture);
    glBindTexture(GL_TEXTURE_2D, m_Texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}

void Plane::draw(const Camera& camera)
{
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::mat4 projectionMatrix = camera.getProjectionMatrix();
    glm::mat4 viewMatrix = camera.getViewMatrix();
    glm::mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;

    m_Shader.useProgram();
    m_Shader.setUniformMatrix4fv("MVP", 1, GL_FALSE, (const GLfloat*)&modelViewProjectionMatrix);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_Texture);
    m_Shader.setUniform1i("myTextureSampler", 0);

    glBindVertexArray(m_VertexArray);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
