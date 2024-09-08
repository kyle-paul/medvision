#pragma once
#include "../exlibs.h"
#include "../renderer/shaders/shader.h"
#include "../renderer/texture/texture.h"
#include "../factory/factory.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_internal.h"

#include "../components/transform_component.h"
#include "../components/physics_component.h"
#include "../components/render_component.h"
#include "../components/object_component.h"

#include "../systems/render_system.h"

class App
{
public:
    App();
    ~App();
    void run();

    // Components
    std::unordered_map<unsigned int, TransformComponent> transform_components;
    std::unordered_map<unsigned int, PhysicsComponent> physics_components;
    std::unordered_map<unsigned int, RenderComponent> render_components;

    std::unordered_map<unsigned int, objectInit> objectArrays;
    unsigned int objectMade = 0;

private:
    GLFWwindow *window;
    Shader *shader;

    // Init functions
    void setup_glfw();
    void setup_opengl();
    void setup_imgui();
    void setup_systems();

    // Systems
    RenderSystem *renderSystem;
};