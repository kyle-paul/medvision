#include "VertexArray.h"

VertexArray::VertexArray()
{
    glGenVertexArrays(1, &vertID);
}

void VertexArray::Bind() const
{
    glBindVertexArray(vertID);
}

void VertexArray::Unbind() const
{
    glBindVertexArray(0);
}

void VertexArray::receiveBuffer(const unsigned int &start_index, const VertexBuffer &vbo, const VertexBufferLayout &layout)
{
    Bind();
    vbo.Bind();
    const std::vector<VertexBufferAttrib> attribs = layout.GetAttribs();
    unsigned int offset = 0;
    for (unsigned int i = start_index; i < attribs.size() + start_index; i++)
    {
        const VertexBufferAttrib attrib = attribs[i - start_index];
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, attrib.attribCount, attrib.type, attrib.normalized, layout.GetStride(), INT2VOIDP(offset));
        offset += attrib.attribCount * VertexBufferAttrib::GetSizeOfType(attrib.type);
    }
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &vertID);
}