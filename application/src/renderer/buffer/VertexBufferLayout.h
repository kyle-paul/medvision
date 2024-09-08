#pragma once
#include "../../exlibs.h"
#include "../../standard.h"
#include "../../debug/debug.h"

struct VertexBufferAttrib
{
    unsigned int type;
    unsigned int attribCount;
    unsigned int normalized;

    static unsigned int GetSizeOfType(unsigned int type)
    {
        switch (type)
        {
        case GL_FLOAT:
            return sizeof(GLfloat);
        case GL_UNSIGNED_INT:
            return sizeof(GLuint);
        case GL_UNSIGNED_BYTE:
            return sizeof(GLbyte);
        }
        ASSERT(false);
        return 0;
    }
};

class VertexBufferLayout
{
public:
    VertexBufferLayout() : m_Stride(0){};

    void AddFloat(unsigned int attribCount) { Push(GL_FLOAT, attribCount, GL_FALSE); }
    void addUnsignedInt(unsigned int attribCount) { Push(GL_UNSIGNED_INT, attribCount, GL_FALSE); }
    void addUnsignedByte(unsigned int attribCount) { Push(GL_UNSIGNED_BYTE, attribCount, GL_TRUE); }

    inline const std::vector<VertexBufferAttrib> GetAttribs() const
    {
        return VertexBufferAttribs;
    }
    inline unsigned int GetStride() const
    {
        return m_Stride;
    }

private:
    unsigned int m_Stride;
    std::vector<VertexBufferAttrib> VertexBufferAttribs;
    void Push(unsigned int type, unsigned int attribCount, unsigned char normalized)
    {
        VertexBufferAttribs.push_back({type, attribCount, normalized});
        m_Stride += attribCount * VertexBufferAttrib::GetSizeOfType(type); // stride = micro_size = attribCount * type
    }
};
