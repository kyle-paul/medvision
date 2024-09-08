#include "texture.h"
#include "stb_image/stb_image.h"

Texture::Texture(const std::string &filepath)
    : textureID(0), filepath(filepath), data(nullptr), width(0), height(0), channels(0)
{
    stbi_set_flip_vertically_on_load(1);
    data = stbi_load(filepath.c_str(), &width, &height, &channels, 4);

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    Unbind();

    if (data)
        stbi_image_free(data);
}

Texture::~Texture()
{
    glDeleteTextures(1, &textureID);
}

void Texture::Bind(unsigned int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::Unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}