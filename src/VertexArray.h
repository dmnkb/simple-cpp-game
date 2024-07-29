#include "Buffer.h"
#include "pch.h"

class VertexArray
{
  public:
    // clang-format off
    VertexArray() { glGenVertexArrays(1, &m_VertexArray); }
    void bind() { glBindVertexArray(m_VertexArray); }
    void unbind() { glBindVertexArray(0); }

    void addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);
    void setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);
    
    std::vector<std::shared_ptr<VertexBuffer>> GetVertexBuffers() const { return m_VertexBuffers; }
    std::shared_ptr<IndexBuffer> getIndexBuffer() const { return m_IndexBuffer; }
    // clang-format on

  private:
    GLuint m_VertexArray;
    GLuint m_VertexBufferIndex = 0;
    std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
    std::shared_ptr<IndexBuffer> m_IndexBuffer;
};