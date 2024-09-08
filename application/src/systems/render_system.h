#pragma once

#include "../exlibs.h"
#include "../standard.h"
#include "../renderer/shaders/shader.h"

#include "../components/transform_component.h"
#include "../components/render_component.h"

class RenderSystem
{
public:
    RenderSystem(Shader *shader, GLFWwindow *window);
    void update(std::unordered_map<unsigned int, TransformComponent> &transform_components,
                std::unordered_map<unsigned int, RenderComponent> &render_components);

private:
    Shader *shader;
    GLFWwindow *window;
};