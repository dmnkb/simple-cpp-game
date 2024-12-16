#include "Mesh.h"
#include "pch.h"

// Constructor
Mesh::Mesh(const std::string& modelPath)
{
    glGenVertexArrays(1, &m_VertexArray);
    glBindVertexArray(m_VertexArray);

    glGenBuffers(1, &m_VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &m_IndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, UV));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(2);

    // Unbind VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// Bind with instance buffer
void Mesh::bind(GLuint instanceBuffer)
{
    glBindVertexArray(m_VertexArray);
}

void Mesh::unbind()
{
    glBindVertexArray(0);
}

GLsizei Mesh::getIndexCount() const
{
    constexpr GLsizei IndexCount = sizeof(indices) / sizeof(indices[0]);
    return IndexCount;
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &m_VertexBuffer);
    glDeleteBuffers(1, &m_IndexBuffer);
    glDeleteVertexArrays(1, &m_VertexArray);
}
