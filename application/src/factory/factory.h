#pragma once
#include "../exlibs.h"
#include "../standard.h"

#include "../renderer/buffer/VertexArray.h"
#include "../renderer/buffer/VertexBuffer.h"
#include "../renderer/buffer/IndexBuffer.h"
#include "../renderer/texture/texture.h"

#include "../components/transform_component.h"
#include "../components/physics_component.h"
#include "../components/render_component.h"
#include "../components/object_component.h"

class Factory
{
public:
    Factory(std::unordered_map<unsigned int, TransformComponent> &transform_components,
            std::unordered_map<unsigned int, PhysicsComponent> &physics_components,
            std::unordered_map<unsigned int, RenderComponent> &render_components);
    ~Factory();

    void create_cube(glm::vec3 size, const std::string &texture_path,               // entity properties
                     glm::vec3 &position, glm::vec3 &eulers, glm::f32 &zoom_factor, // transform properties
                     glm::vec3 &velocity, glm::vec3 &eulerVelocity);                // physics properties

    void create_mesh(const objectInit &object);

private:
    unsigned int EntityMade;
    std::unordered_map<unsigned int, TransformComponent> &transform_components;
    std::unordered_map<unsigned int, PhysicsComponent> &physics_components;
    std::unordered_map<unsigned int, RenderComponent> &render_components;

    std::vector<std::string> split(const std::string &line,
                                   const std::string &delimiter,
                                   std::vector<std::string> &result);

    RenderComponent make_cube(glm::vec3 &size);
    RenderComponent make_mesh(const char *obj_path, const float &scale, const bool &onTexCoords);
};