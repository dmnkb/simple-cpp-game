#include "Cube.h"
#include "Camera.h"
#include <filesystem>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <iostream>

Cube::Cube(const GLuint& textureID, Shader* shader, glm::vec3 position)
    : m_Shader(shader), m_Texture(textureID), m_Position(position)
{
    glGenVertexArrays(1, &m_VertexArray);
    glGenBuffers(1, &m_VertexBuffer);
    glGenBuffers(1, &m_IndexBuffer);

    // Bind the VAO first, then bind and set VBO and attribute ponter
    glBindVertexArray(m_VertexArray);

    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    const size_t POSITION_SIZE = 3;
    const size_t UV_SIZE = 2;
    const size_t STRIDE = (POSITION_SIZE + UV_SIZE) * sizeof(GLfloat);

    // Position attribute
    glVertexAttribPointer(0, POSITION_SIZE, GL_FLOAT, GL_FALSE, STRIDE, (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // UV attribute
    glVertexAttribPointer(1, UV_SIZE, GL_FLOAT, GL_FALSE, STRIDE, (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Unbind VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Cube::draw(const Camera& camera)
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
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Cube::remove()
{
    glDeleteBuffers(1, &m_VertexBuffer);
    glDeleteBuffers(1, &m_IndexBuffer);
    glDeleteVertexArrays(1, &m_VertexArray);
    std::cout << "Cube removed" << std::endl;
}