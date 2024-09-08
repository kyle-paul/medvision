#pragma once
#include "../exlibs.h"

struct TransformComponent
{
    glm::vec3 *position;
    glm::vec3 *eulers;
    glm::f32 *zoom_factor;
};