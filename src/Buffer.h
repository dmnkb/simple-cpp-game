#include <glad/glad.h>

class VertexBuffer
{
  public:
    VertexBuffer(uint32_t size)
    {
        glGenBuffers(1, &m_VertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    }

    VertexBuffer(GLfloat* vertices, uint32_t size)
    {
        glGenBuffers(1, &m_VertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_DYNAMIC_DRAW);
    }

    // clang-format off
    ~VertexBuffer() { glDeleteBuffers(1, &m_VertexBuffer); }

    void bind() { glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer); }
    void unbind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }
    // clang-format on

    void SetData(const void* data, uint32_t size)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
    }

  private:
    GLuint m_VertexBuffer;
};

class IndexBuffer
{
  public:
    IndexBuffer(GLuint* indices, uint32_t count) : m_Count(count)
    {
        glGenBuffers(1, &m_IndexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
    }

    // clang-format off
    ~IndexBuffer() { glDeleteBuffers(1, &m_IndexBuffer); }

    virtual uint32_t GetCount() const { return m_Count; }

    void bind() { glBindBuffer(GL_ARRAY_BUFFER, m_IndexBuffer); }
    void unbind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }
    // clang-format on

    void setData(const void* data, uint32_t size)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_IndexBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
    }

  private:
    GLuint m_IndexBuffer;
    uint32_t m_Count;
};