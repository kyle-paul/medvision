#pragma once
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

class VertexArray
{
public:
    VertexArray();
    ~VertexArray();

    void receiveBuffer(const unsigned int &start_index, const VertexBuffer &vbo, const VertexBufferLayout &layout);
    void Bind() const;
    void Unbind() const;

    unsigned int vertID;

private:
};
