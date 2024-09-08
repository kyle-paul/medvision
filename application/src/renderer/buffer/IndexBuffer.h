#pragma once
#include "../../exlibs.h"
#include "../../standard.h"

class IndexBuffer
{
public:
    IndexBuffer();
    IndexBuffer(const std::vector<unsigned int> &array);
    ~IndexBuffer();
    void Bind() const;
    void Unbind() const;
    inline unsigned int GetVertexCount() const
    {
        return vertexCount;
    }
    unsigned int bufferID;
    unsigned int vertexCount;

private:
};