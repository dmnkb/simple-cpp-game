#include "Mesh.h"
#include "pch.h"

Mesh::Mesh(const std::string modelPath)
{
    // TODO: Replace the hard-coded cube vertex data with dynamically loaded meshes

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

    // Unbind VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::bind()
{
    glBindVertexArray(m_VertexArray);
}

void Mesh::unbind()
{
    glBindVertexArray(0);
}

void Mesh::draw()
{

    bind();
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    unbind();
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &m_VertexBuffer);
    glDeleteBuffers(1, &m_IndexBuffer);
    glDeleteVertexArrays(1, &m_VertexArray);
}
