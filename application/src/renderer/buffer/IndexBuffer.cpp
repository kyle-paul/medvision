#include "IndexBuffer.h"

/*
INDEX_BUFFER -> BIND + UNBIND -> DELETE
*/

IndexBuffer::IndexBuffer()
    : bufferID(0), vertexCount(0)
{
}

IndexBuffer::IndexBuffer(const std::vector<unsigned int> &array) : vertexCount(array.size())
{
    glGenBuffers(1, &bufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, array.size() * sizeof(unsigned int), array.data(), GL_STATIC_DRAW);
}

void IndexBuffer::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
}

void IndexBuffer::Unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

IndexBuffer::~IndexBuffer()
{
    glDeleteBuffers(1, &bufferID);
}