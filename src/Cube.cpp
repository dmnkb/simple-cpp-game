#include "Cube.h"
#include "Camera.h"
#include "pch.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

Cube::Cube(const GLuint textureID, std::shared_ptr<Shader> shader, glm::vec3 position)
    : m_Shader(shader), m_TextureID(textureID), m_Position(position)
{
    glGenVertexArrays(1, &m_VertexArray);
    glBindVertexArray(m_VertexArray);

    glGenBuffers(1, &m_VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &m_IndexBuffer);
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

    // Bind texture and set uniform
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
    m_Shader->setUniform1i("myTextureSampler", 0);

    // Unbind VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Cube::draw(const Camera& camera)
{
    // Create transformation matrices
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), m_Position);

    // Apply rotations
    modelMatrix = glm::rotate(modelMatrix, m_Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, m_Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, m_Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

    // Apply scale
    modelMatrix = glm::scale(modelMatrix, m_Scale);

    // Combine with view and projection matrices
    glm::mat4 projectionMatrix = camera.getProjectionMatrix();
    glm::mat4 viewMatrix = camera.getViewMatrix();
    glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;

    // Bind the shader and set uniforms
    m_Shader->bind();
    m_Shader->setUniformMatrix4fv("u_ViewProjection", viewProjectionMatrix);
    m_Shader->setUniformMatrix4fv("u_Transform", modelMatrix);

    // Draw the cube
    glBindVertexArray(m_VertexArray);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Cube::remove()
{
    glDeleteBuffers(1, &m_VertexBuffer);
    glDeleteBuffers(1, &m_IndexBuffer);
    glDeleteVertexArrays(1, &m_VertexArray);
}
