#pragma once
#include "../exlibs.h"
#include "../renderer/buffer/VertexArray.h"
#include "../renderer/buffer/IndexBuffer.h"
#include "../renderer/texture/texture.h"

struct RenderComponent
{
    Texture *texture;
    VertexArray *vao;
    IndexBuffer *fbo;
};