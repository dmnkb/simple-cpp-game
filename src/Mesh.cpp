#include "Mesh.h"
#include "pch.h"
#include <glm/glm.hpp>

struct Vertex
{
    glm::vec3 Position;
    glm::vec2 UV;
    glm::vec3 Normal;
};

// Constructor
Mesh::Mesh(const std::string& modelPath)
{
    glGenVertexArrays(1, &m_VertexArray);
    glBindVertexArray(m_VertexArray);

    // Vertex buffer
    glGenBuffers(1, &m_VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Index buffer
    glGenBuffers(1, &m_IndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, UV));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

// Bind with instance buffer
void Mesh::bind(GLuint instanceBuffer)
{
    glBindVertexArray(m_VertexArray);

    // Check the current VAO binding
#if 0
    GLuint currentVAO;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, (GLint*)&currentVAO);
    std::cout << "Current VAO: " << currentVAO << std::endl;
    endif;
#endif
    // Bind instance buffer and set up attributes
    glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);

    // Check the current array buffer binding
#if 0
    GLuint currentBuffer;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (GLint*)&currentBuffer);
    std::cout << "Current Array Buffer: " << currentBuffer << std::endl;
#endif

    // Instance matrix attributes (column-major)
    for (int i = 0; i < 4; ++i)
    {
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
        glEnableVertexAttribArray(3 + i);
        glVertexAttribDivisor(3 + i, 1);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind for safety
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
