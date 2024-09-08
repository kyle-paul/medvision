#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(const std::vector<float> &array)
{
    glGenBuffers(1, &bufferID);
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
    glBufferData(GL_ARRAY_BUFFER, array.size() * sizeof(float), array.data(), GL_STATIC_DRAW); // macro_size = elementCount x type
}

void VertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
}

void VertexBuffer::Unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &bufferID);
}