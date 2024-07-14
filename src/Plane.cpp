#include "Plane.h"
#include "Camera.h"
#include <filesystem>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <iostream>

Plane::Plane(const GLuint& textureID, Shader* shader, glm::vec3 position)
    : m_Shader(shader), m_Texture(textureID), m_Position(position)
{
    glGenBuffers(1, &m_VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &m_IndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &m_VertexArray);
    glBindVertexArray(m_VertexArray);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
    m_Shader->setVertexAttribute("vPos", 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
    m_Shader->setVertexAttribute("vUV", 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
}

void Plane::draw(const Camera& camera)
{
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), m_Position);
    glm::mat4 projectionMatrix = camera.getProjectionMatrix();
    glm::mat4 viewMatrix = camera.getViewMatrix();
    glm::mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;

    m_Shader->bind();
    m_Shader->setUniformMatrix4fv("MVP", modelViewProjectionMatrix);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_Texture);
    m_Shader->setUniform1i("myTextureSampler", 0);

    glBindVertexArray(m_VertexArray);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Plane::remove()
{
    glDeleteBuffers(1, &m_VertexBuffer);
    glDeleteBuffers(1, &m_IndexBuffer);
    glDeleteVertexArrays(1, &m_VertexArray);
    std::cout << "Plane removed" << std::endl;
}