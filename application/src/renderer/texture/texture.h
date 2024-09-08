#pragma once
#include "../../exlibs.h"
#include "../../standard.h"

class Texture
{
public:
    unsigned int textureID;
    Texture(const std::string &filepath);
    ~Texture();

    void Bind(unsigned int slot = 0) const;
    void Unbind() const;

    inline int GetWidth() const
    {
        return width;
    }
    inline int GetHeight() const
    {
        return height;
    }

private:
    std::string filepath;
    unsigned char *data;
    int width, height, channels;
};