#pragma once
#include "../../exlibs.h"
#include "../../standard.h"

class VertexBuffer
{
public:
    VertexBuffer(const std::vector<float> &array);
    ~VertexBuffer();
    void Bind() const;
    void Unbind() const;

private:
    unsigned int bufferID;
};
